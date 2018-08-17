
#include "A2PrimaryGeneratorAction.hh"

#include "A2PrimaryGeneratorMessenger.hh"
#include "A2FileGeneratorMkin.hh"
#include "A2FileGeneratorPluto.hh"

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
  fBeamXSigma=0.5*cm;
  fBeamYSigma=0.5*cm;
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

  fDetCon=NULL;
  fInFileName="";
}



A2PrimaryGeneratorAction::~A2PrimaryGeneratorAction()
{
  if(fGenLorentzVec)
  {
    for(Int_t i=0;i<fNGenParticles;i++) delete fGenLorentzVec[i];
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
    fGenLorentzVec[1]->SetXYZM(pvec.x(),pvec.y(),pvec.z(),Mass);
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
    if (fFileGen && fNToBeTracked > 0){

      //
      // set tracking flag (only for first event)
      //

      // check for first event
      if (fNevent == 0)
      {
        // loop over tracked particles
        for (G4int i = 0; i < fNToBeTracked; i++)
        {
          G4int p_index = fTrackThis[i]-1;
          fFileGen->SetParticleIsTrack(p_index);
          if (fFileGen->IsParticleTrack(p_index))
          {
            G4cout << "Will track " << fFileGen->GetParticleDefinition(p_index)->GetParticleName()
                   << " with index " << fTrackThis[i] << G4endl;
          }
        }
      }

      // get the event from input tree
      fFileGen->ReadEvent(fNevent);

      //
      // set arrays for CB output reader
      //

      // set vertex
      fGenPosition[0] = fFileGen->GetVertex().x()/cm;
      fGenPosition[1] = fFileGen->GetVertex().y()/cm;
      fGenPosition[2] = fFileGen->GetVertex().z()/cm;

      // set generated beam 4-vector to store in output file
      const A2FileGenerator::A2GenParticle_t& beam = fFileGen->GetBeam();
      fBeamLorentzVec->SetPxPyPzE(beam.fP.x(), beam.fP.y(), beam.fP.z(), beam.fE);

      // set generated 4-vectors to store in output file
      for (G4int i = 1; i < fNGenParticles; i++)
      {
	const G4ThreeVector& mom = fFileGen->GetParticleMomentum(i-1);
        fGenLorentzVec[i]->SetPxPyPzE(mom.x(), mom.y(), mom.z(), fFileGen->GetParticleEnergy(i-1));
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
  if(fGenPartType[1]==-1){fGenPartType[1]= PDGtoG3(fParticleGun->GetParticleDefinition()->GetPDGEncoding());G4cout<<"P type "<<fGenPartType[1]<<G4endl;}
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
  fGenLorentzVec[1]->SetXYZM(p3.x(),p3.y(),p3.z(),Mass);
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
  if (ftest && !ftest->IsZombie())
  {
    tree_mkin = (TTree*)ftest->Get("h1");
    tree_pluto = (TTree*)ftest->Get("data");
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
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening mkin event-file" << G4endl;
    fFileGen = new A2FileGeneratorMkin(fInFileName);
  }
  else if (tree_pluto)
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): Opening Pluto event-file" << G4endl;
    fFileGen = new A2FileGeneratorPluto(fInFileName);
  }
  else
  {
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): No supported ROOT event-tree found!" << G4endl;
    exit(1);
  }

  // init the file
  fFileGen->Init();

  fNGenParticles = fFileGen->GetNParticles() + 1; // for beam
  fGenLorentzVec=new TLorentzVector*[fNGenParticles];
  for(Int_t i=0;i<fNGenParticles;i++)
    fGenLorentzVec[i]=new TLorentzVector();
  fGenPartType=new Int_t[fNGenParticles];
  for(Int_t i=1;i<fNGenParticles;i++)
  {
    fGenPartType[i] = fFileGen->GetParticleDefinition(i-1) ?
                      PDGtoG3(fFileGen->GetParticleDefinition(i-1)->GetPDGEncoding()) : 0;
  }
  if(fNToBeTracked==0){
    G4cout << "A2PrimaryGeneratorAction::SetUpFileInput(): No particle marked for tracking!" << G4endl;
    exit(1);
  }
  else if(fNToBeTracked!=fNToBeTcount){
    G4cout<<"A2PrimaryGeneratorAction::SetUpRootInput(): You have not set enough particles to be tracked!"<<G4endl;
    exit(1);
  }
  else G4cout<<"A2PrimaryGeneratorAction::SetUpFileInput(): You have chosen to track "<<fNToBeTracked<<" generated particles."<<G4endl;
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
    fGenLorentzVec=new TLorentzVector*[2];
    fGenPartType=new G4int[2];
    fNGenParticles=2;//to write the A2CBoutput into dircos branch
    fGenLorentzVec[0]=new TLorentzVector(0,0,0,0);
    fGenLorentzVec[1]=new TLorentzVector(0,0,0,0);
    fGenPartType[0]=-1;
    fGenPartType[1]=-1;
    fTrackThis = new Int_t[1];
    fTrackThis[0] = 1;
  }
  G4cout<<"Set Mode done "<<G4endl;
}

