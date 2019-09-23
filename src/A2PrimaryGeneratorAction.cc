
#include "A2PrimaryGeneratorAction.hh"

#include "A2PrimaryGeneratorMessenger.hh"
#include "A2DetectorConstruction.hh"
#include "A2FileGeneratorMkin.hh"
#include "A2FileGeneratorPluto.hh"
#include "A2FileGeneratorGiBUU.hh"

#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "TLorentzVector.h"
#include "TFile.h"

#include "MCNtuple.h"

using namespace CLHEP;

A2PrimaryGeneratorAction::A2PrimaryGeneratorAction()

{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  fGunMessenger = new A2PrimaryGeneratorMessenger(this);
  fGenPartType=NULL;
  fTrackThis=NULL;
  fGenLorentzVec=NULL;
  fBeamLorentzVec=new TLorentzVector(0,0,0,0);//for beam or interactive events
  fFileGen = 0;

  //default phase space limits
  fTmin=0;
  fTmax=400*MeV;
  fThetamin=0;
  fThetamax=180*deg;
  fBeamEnergy=0;
  fBeamXSigma=0.5*cm;
  fBeamYSigma=0.5*cm;
  fBeamDiameter = 0;
  fTargetZ0=0*cm;
  fTargetThick=5*cm;
  fTargetRadius=2*cm;
  //overlap
  fSplitTheta=0;

  //default mode is g4 command line input
  fMode=EPGA_g4;
  fNevent=0;
  fNToBeTcount=0;
  fNToBeTracked=0;
  fNGenParticles=1;//for interactive use
  fNGenMaxParticles=1;//for interactive use

  fDetCon=NULL;
  fInFileName="";
}



A2PrimaryGeneratorAction::~A2PrimaryGeneratorAction()
{
  if(fGenLorentzVec)
  {
    for(Int_t i=0;i<fNGenMaxParticles;i++) delete fGenLorentzVec[i];
    delete [] fGenLorentzVec;
  }
  if (fGenPartType)
    delete [] fGenPartType;
  if (fTrackThis)
    delete [] fTrackThis;
  if (fFileGen)
      delete fFileGen;
  delete fParticleGun;
  delete fGunMessenger;
  delete fBeamLorentzVec;
}



void A2PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //This function is called at the begining of event
  Float_t Mass;
  Float_t P;
  G4ThreeVector pvec;
  switch(fMode){
  case EPGA_g4:
    if(!fGenLorentzVec)SetMode(EPGA_g4);//in case not called in macro
    //standard g4 command line event generator
    fParticleGun->GeneratePrimaryVertex(anEvent);
    Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
    P=sqrt(fParticleGun->GetParticleEnergy()*fParticleGun->GetParticleEnergy()-Mass*Mass);
    pvec=(fParticleGun->GetParticleMomentumDirection())*P;
    fBeamLorentzVec->SetXYZM(pvec.x(),pvec.y(),pvec.z(),Mass);
    fGenLorentzVec[0]->SetXYZM(pvec.x(),pvec.y(),pvec.z(),Mass);
    break;

  case EPGA_phase_space:
    //Phase space generator
    PhaseSpaceGenerator(anEvent);
    break;

  case EPGA_Overlap:
    //Phase space generator for overlapping paticles
    OverlapGenerator(anEvent);
    break;

  case EPGA_FILE:
    if (fFileGen)
    {
      // generate vertex for pluto/GiBUU input
      if (fFileGen->GetType() == A2FileGenerator::kPluto ||
          fFileGen->GetType() == A2FileGenerator::kPlutoCocktail ||
          fFileGen->GetType() == A2FileGenerator::kGiBUU)
      {
        fFileGen->GenerateVertexCylinder(fDetCon->GetTarget()->GetLength(),
                                         fDetCon->GetTarget()->GetCenter().z(),
                                         fBeamDiameter);
      }

      // get the event from input tree
      fFileGen->ReadEvent(fNevent);
      //fFileGen->Print();

      //
      // show added particles
      //

      // check for first event
      if (fNevent == 0 &&
          fFileGen->GetType() != A2FileGenerator::kPlutoCocktail &&
          fFileGen->GetType() != A2FileGenerator::kGiBUU)
      {
        for (G4int i = 0; i < fFileGen->GetNParticles(); i++)
        {
          TString fmt = TString::Format("Particle %2d : ", i+1);
          if (fFileGen->GetParticleDefinition(i))
          {
            fmt += TString::Format("%-12s  PDG ID: %d",
                                   fFileGen->GetParticleDefinition(i)->GetParticleName().c_str(),
                                   fFileGen->GetParticleDefinition(i)->GetPDGEncoding());
          }
          else
            fmt += "unknown";
          G4cout << fmt << G4endl;
        }
      }

      //
      // set tracking flag
      // overwrite tracking flags possibly set in the file
      // event generator (Pluto)
      //

      // check for manually specified tracking indices
      if (fNToBeTracked)
      {
        // loop over particles
        for (G4int i = 0; i < fFileGen->GetNParticles(); i++)
        {
          // check if it was marked as tracked
          if (IsTracked(i+1))
          {
            // try to set flag for tracking
            fFileGen->SetParticleIsTrack(i);

            // user info
            if (fNevent == 0)
            {
              if (fFileGen->IsParticleTrack(i))
              {
                G4cout << "Particle " << i+1 << " ("
                       << fFileGen->GetParticleDefinition(i)->GetParticleName()
                       << ") will be tracked" << G4endl;
              }
              else
              {
                G4cout << "Unknown particle " << i+1 << " cannot be tracked!" << G4endl;
              }
            }
          }
          else
          {
            fFileGen->SetParticleIsTrack(i, false);
          }
        }
      }
      else
      {
        if (fNevent == 0)
          G4cout << "All (stable) particles will be tracked" << G4endl;
      }

      //
      // one-time user info
      //

      if (fNevent == 0)
      {
        if (fBeamEnergy != 0)
          G4cout << "Using " << fBeamEnergy/GeV << " GeV as constant photon beam energy" << G4endl;
      }

      //
      // set arrays for CB output reader
      //

      // set vertex
      fGenPosition[0] = fFileGen->GetVertex().x()/cm;
      fGenPosition[1] = fFileGen->GetVertex().y()/cm;
      fGenPosition[2] = fFileGen->GetVertex().z()/cm;

      // set generated beam 4-vector to store in output file
      const A2FileGenerator::A2GenParticle_t& beam = fFileGen->GetBeam();
      if (fBeamEnergy != 0)
        fBeamLorentzVec->SetPxPyPzE(0, 0, fBeamEnergy, fBeamEnergy);
      else
        fBeamLorentzVec->SetPxPyPzE(beam.fP.x(), beam.fP.y(), beam.fP.z(), beam.fE);

      // set generated 4-vectors to store in output file
      fNGenParticles = fFileGen->GetNParticles();
      for (G4int i = 0; i < fFileGen->GetNParticles(); i++)
      {
	const G4ThreeVector& mom = fFileGen->GetParticleMomentum(i);
        fGenLorentzVec[i]->SetPxPyPzE(mom.x(), mom.y(), mom.z(), fFileGen->GetParticleEnergy(i));
        fGenPartType[i] = fFileGen->GetParticleDefinition(i) ?
                          PDGtoG3(fFileGen->GetParticleDefinition(i)->GetPDGEncoding()) : 0;
      }

      //
      // configure particle gun
      //

      // loop over all particles
      for (G4int i = 0; i < fFileGen->GetNParticles(); i++)
      {
        if (fFileGen->IsParticleTrack(i))
        {
          //G4cout << i+1 << "  " << fFileGen->GetParticleDefinition(i)->GetParticleName() << G4endl
          //       << "Vertex : " << fFileGen->GetParticleVertex(i) << G4endl
          //       << "Time : " << fFileGen->GetParticleTime(i) << G4endl
          //       << "Direction : " << fFileGen->GetParticleDirection(i) << G4endl
          //       << "Energy : " << fFileGen->GetParticleKineticEnergy(i) << G4endl;
          fParticleGun->SetParticleDefinition(fFileGen->GetParticleDefinition(i));
          fParticleGun->SetParticleMomentumDirection(fFileGen->GetParticleDirection(i));
          fParticleGun->SetParticleEnergy(fFileGen->GetParticleKineticEnergy(i));
          fParticleGun->SetParticlePosition(fFileGen->GetParticleVertex(i));
          fParticleGun->SetParticleTime(fFileGen->GetParticleTime(i));
          fParticleGun->GeneratePrimaryVertex(anEvent);
        }
      }
      //G4cout << G4endl;

      // increment event counter
      fNevent++;
    }
    else{ G4cerr<<"ROOT input mode specified but no input file given"<<G4endl; exit(1);}
    break;
  default:
    G4cerr<<"Unknown mode given to A2PrimiaryGeneratorAction"<<G4endl;
    exit(1);

  }
}

void A2PrimaryGeneratorAction::PhaseSpaceGenerator(G4Event* anEvent){
  if(fGenPartType[0]==-1){fGenPartType[0]= PDGtoG3(fParticleGun->GetParticleDefinition()->GetPDGEncoding());G4cout<<"P type "<<fGenPartType[0]<<G4endl;}
  //4-momenta
  G4float theta=acos((cos(fThetamax)-cos(fThetamin))*G4UniformRand()+cos(fThetamin));
  G4float phi=2*3.141592653589*G4UniformRand();
  G4float T=fTmin+(fTmax-fTmin)*G4UniformRand();
  G4float Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4float E=T+Mass;
  G4float P=sqrt(E*E-Mass*Mass);
  G4ThreeVector p3(0, 0, 1);
  p3.setMag(P);
  p3.setTheta(theta);
  p3.setPhi(phi);
  fParticleGun->SetParticleMomentumDirection(p3.unit());
  fParticleGun->SetParticleEnergy(T);
  //save root output
  fBeamLorentzVec->SetXYZM(p3.x(),p3.y(),p3.z(),Mass);
  fGenLorentzVec[0]->SetXYZM(p3.x(),p3.y(),p3.z(),Mass);
  //position vertex
  G4float tx=1E10*m;
  G4float ty=1E10*m;
  while(tx*tx+ty*ty>fTargetRadius*fTargetRadius){
    tx=G4RandGauss::shoot(0,fBeamXSigma);
    ty=G4RandGauss::shoot(0,fBeamYSigma);
    p3.setX(tx);
    p3.setY(ty);
    p3.setZ(fTargetZ0+fTargetThick/2*(2*G4UniformRand()-1));
  }

  fParticleGun->SetParticlePosition(p3);
  fGenPosition[0]=p3.x()/cm;
  fGenPosition[1]=p3.y()/cm;
  fGenPosition[2]=p3.z()/cm;

  //produce event
  fParticleGun->GeneratePrimaryVertex(anEvent);

}
void A2PrimaryGeneratorAction::OverlapGenerator(G4Event* anEvent){
  //phase space genreator + creates an additional particle at angle fSplitTheta to the first

  G4float tx=1E10*m;
  G4float ty=1E10*m;
  while(tx*tx+ty*ty>fTargetRadius*fTargetRadius){
    tx=G4RandGauss::shoot(0,fBeamXSigma);
    ty=G4RandGauss::shoot(0,fBeamYSigma);
  }
  G4ThreeVector p3;
  p3.setX(tx);
  p3.setY(ty);
  p3.setZ(fTargetZ0+fTargetThick/2*(2*G4UniformRand()-1));
  //G4cout<<"PGA "<<fTargetZ0/cm<<" "<<fTargetThick/cm<<" "<<(2*G4UniformRand()-1)<<G4endl;
  fParticleGun->SetParticlePosition(p3);
  fGenPosition[0]=p3.x()/cm;
  fGenPosition[1]=p3.y()/cm;
  fGenPosition[2]=p3.z()/cm;
  G4float theta=acos((cos(fThetamax)-cos(fThetamin))*G4UniformRand()+cos(fThetamin));
  G4float phi=2*3.141592653589*G4UniformRand();
  G4float T=fTmin+(fTmax-fTmin)*G4UniformRand();
  G4float Mass=fParticleGun->GetParticleDefinition()->GetPDGMass();
  G4float E=T+Mass;
  G4float P=sqrt(E*E-Mass*Mass);
  //theta=90*deg;
  //phi=30.*deg;
  p3.setMag(P);
  p3.setTheta(theta);
  p3.setPhi(phi);
  fParticleGun->SetParticleMomentumDirection(p3.unit());
  fParticleGun->SetParticleEnergy(T);

  //intitial particle
  fParticleGun->GeneratePrimaryVertex(anEvent);

  //and overlap
  T=fTmin+(fTmax-fTmin)*G4UniformRand();
  E=T+Mass;
  P=sqrt(E*E-Mass*Mass);
  p3.setMag(P);

  p3.setTheta(theta+fSplitTheta);
  p3.rotate(fParticleGun->GetParticleMomentumDirection(),2*3.141592653589*G4UniformRand());
  fParticleGun->SetParticleEnergy(T);
  fParticleGun->SetParticleMomentumDirection(p3.unit());
  fParticleGun->GeneratePrimaryVertex(anEvent);

}
void A2PrimaryGeneratorAction::SetUpFileInput(){
  if(fInFileName==TString(""))return;
  G4cout<<"A2PrimaryGeneratorAction::SetUpFileInput(): input file set as "<<fInFileName<<G4endl;

  fMode=EPGA_FILE;

  // check for ROOT file
  if (!fInFileName.EndsWith(".root"))
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Unknown input-file ending!" << G4endl;
    exit(1);
  }

  // look for supported event trees in ROOT file
  TFile* ftest = new TFile(fInFileName);
  TTree* tree_mkin = 0;
  TTree* tree_pluto = 0;
  TTree* tree_gibuu = 0;
  if (ftest && !ftest->IsZombie())
  {
    tree_mkin = (TTree*)ftest->Get("h1");
    tree_pluto = (TTree*)ftest->Get("data");
    tree_gibuu = (TTree*)ftest->Get("RootTuple");
    delete ftest;
  }
  else
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Could not open ROOT file " << fInFileName << G4endl;
    exit(1);
  }

  // open file
  if (tree_mkin)
  {
    fFileGen = new A2FileGeneratorMkin(fInFileName);
  }
  else if (tree_pluto)
  {
#ifdef WITH_PLUTO
    fFileGen = new A2FileGeneratorPluto(fInFileName);
#else
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Support for Pluto event files was not activated at compile time!" << G4endl;
    exit(1);
#endif
  }
  else if (tree_gibuu)
  {
    fFileGen = new A2FileGeneratorGiBUU(fInFileName);
  }
  else
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): ROOT event-tree format is not supported!" << G4endl;
    exit(1);
  }

  // init the file
  fFileGen->Init();

  // user info
  if (fFileGen->GetType() == A2FileGenerator::kMkin)
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening mkin event-file" << G4endl;
  else if (fFileGen->GetType() == A2FileGenerator::kPluto)
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening Pluto single-event file" << G4endl;
  else if (fFileGen->GetType() == A2FileGenerator::kPlutoCocktail)
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening Pluto cocktail-event file" << G4endl;
  else if (fFileGen->GetType() == A2FileGenerator::kGiBUU)
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening GiBUU-event file" << G4endl;

  // create data structures for generated particles
  fNGenMaxParticles = fFileGen->GetMaxParticles();
  fGenLorentzVec=new TLorentzVector*[fNGenMaxParticles];
  for(Int_t i=0;i<fNGenMaxParticles;i++)
    fGenLorentzVec[i]=new TLorentzVector();
  fGenPartType=new Int_t[fNGenMaxParticles];
  for(Int_t i=1;i<fNGenMaxParticles;i++)
    fGenPartType[i] = -1;

  // perform a few checks
  if (fNToBeTracked == 0)
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): No particles marked for tracking so, depending on the event file format, " <<
              "all (stable) particles will be tracked" << G4endl;
  }
  else
  {
    if (fFileGen->GetType() == A2FileGenerator::kPlutoCocktail || fFileGen->GetType() == A2FileGenerator::kGiBUU)
    {
      G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Particle tracking should not be specified if input is Pluto cocktail!" << G4endl;
      exit(1);
    }
  }
  if (fNToBeTracked != fNToBeTcount)
  {
    G4cout<<"A2PrimaryGeneratorAction::SetUpFileInput(): Mismatch between number of tracked particles and particles marked for tracking!"<<G4endl;
    exit(1);
  }
  if (fFileGen->GetType() == A2FileGenerator::kPluto ||
      fFileGen->GetType() == A2FileGenerator::kPlutoCocktail ||
      fFileGen->GetType() == A2FileGenerator::kGiBUU)
  {
    if (fBeamDiameter == 0)
    {
      G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Pluto/GiBUU-input requires a beam diameter set via /A2/generator/SetBeamDiameter" << G4endl;
      exit(1);
    }
  }

}

G4int A2PrimaryGeneratorAction::GetNEvents()
{
  if (fFileGen)
    return fFileGen->GetNEvents();
  else
    return -1;
}

void A2PrimaryGeneratorAction::SetMode(G4int mode)
{
  fMode=mode;
  G4cout<<"Set Mode "<<G4endl;
  if(fMode==EPGA_phase_space||fMode==EPGA_g4)
  {
    fGenLorentzVec=new TLorentzVector*[1];
    fGenPartType=new G4int[1];
    fNGenParticles=1;//to write the A2CBoutput into dircos branch
    fNGenMaxParticles=1;//to write the A2CBoutput into dircos branch
    fGenLorentzVec[0]=new TLorentzVector(0,0,0,0);
    fGenPartType[0]=-1;
    fTrackThis = new Int_t[1];
    fTrackThis[0] = 1;
  }
  G4cout<<"Set Mode done "<<G4endl;
}

