#include "A2CBOutput.hh"
#include "G4RunManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace CLHEP;

A2CBOutput::A2CBOutput(){
  fFile=NULL;
  fTree=NULL;
  fPGA=const_cast<A2PrimaryGeneratorAction*>(static_cast<const A2PrimaryGeneratorAction*>(G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()));

  fDET=const_cast<A2DetectorConstruction*>(static_cast<const A2DetectorConstruction*>(G4RunManager::GetRunManager()->GetUserDetectorConstruction()));
  //Need to get the number of initial particles
  fnpart=fPGA->GetNGenParticles()-1; //-1 for beam
  if(fnpart>100){
    G4cerr<<"A2CBOutput::A2CBOutput() can only store 100 particles. Need to edit fdircos[100][3] in A2CBOutput.hh if you want more particles."<<G4endl;
    exit(1);
  }
  //Get the LorentzVectors of the initial particles
  fGenLorentzVec=(fPGA->GetGenLorentzVecs()); //Only exists if ntuple input
  fBeamLorentzVec=fPGA->GetBeamLorentzVec();//Will take the default beam if no ntuple
  fGenPartType=fPGA->GetGenPartType();
  fvertex=fPGA->GetVertex();
  //Initialise arrays dependent on number of particles
  //Note can't initialize 2 dimesional arrays like this when writing 
  //to a ROOT TTree, you only get the correct info for the first particle
  //fdircos now declared as a 2D array with 100 and 3 dimensions 
  //fdircos=new Float_t*[fnpart];
  //for(Int_t i=0;i<fnpart;i++) fdircos[i]=new Float_t[3];
  felab=new Float_t[fnpart]; 
  fplab=new Float_t[fnpart]; 
  fidpart=new Int_t[fnpart]; 

  //create Tree
  fTree=new TTree("h12","Crystals");
  fTree->SetAutoSave();

  //Collection IDs for decoding hit collections
  fCBCollID = -1;
  fTAPSCollID = -1;

  // store IDs of primary particles
  fStorePrimaries = true;

  //TOF stuff
  if(fDET->GetNToFbars()>0){
    fToFTot=fDET->GetNToFbars();
    ftofi=new Int_t[fToFTot];
    ftofe=new Float_t[fToFTot];
    ftoft=new Float_t[fToFTot];
    ftofx=new Float_t[fToFTot];
    ftofy=new Float_t[fToFTot];
    ftofz=new Float_t[fToFTot];
  }

}
A2CBOutput::~A2CBOutput(){
  delete fidpart;
  delete fplab;
  delete felab;
  // for(Int_t i=0;i<fnpart;i++) delete fdircos[i];
  if(fTree)delete fTree;
}
//Float_t dircos[4][3];
void A2CBOutput::SetBranches(){

  if(!fTree){
    G4cout<<"A2CBOutput::SetBranches() Can't set branches have to set fTree first!"<<G4endl;
    return;
  }
  Int_t basket =64000;

  fTree->Branch("nhits",&fnhits,"fnhits/I",basket);
  fTree->Branch("npart",&fnpart,"fnpart/I",basket);
  fTree->Branch("ntaps",&fntaps,"fntaps/I",basket);
  fTree->Branch("nvtaps",&fnvtaps,"fnvtaps/I",basket);
  fTree->Branch("vhits",&fvhits,"fvhits/I",basket);
  fTree->Branch("plab",fplab,"fplab[fnpart]/F",basket);
  fTree->Branch("tctaps",ftctaps,"ftctaps[fntaps]/F",basket);
  fTree->Branch("vertex",fvertex,"fvertex[3]/F",basket);
  fTree->Branch("beam",fbeam,"fbeam[5]/F",basket);
  fTree->Branch("dircos",fdircos,"fdircos[fnpart][3]/F",basket);
  fTree->Branch("ecryst",fecryst,"fecryst[fnhits]/F",basket);
  fTree->Branch("tcryst",ftcryst,"ftcryst[fnhits]/F",basket);
  fTree->Branch("ectapfs",fectapfs,"fectapfs[fntaps]/F",basket);
  fTree->Branch("ectapsl",fectapsl,"fectapsl[fntaps]/F",basket);
  fTree->Branch("elab",felab,"felab[fnpart]/F",basket);
  fTree->Branch("eleak",&feleak,"feleak/F",basket);
  fTree->Branch("enai",&fenai,"fenai/F",basket);
  fTree->Branch("etot",&fetot,"fetot/F",basket);
  fTree->Branch("eveto",feveto,"feveto[fvhits]/F",basket);
  fTree->Branch("tveto",ftveto,"ftveto[fvhits]/F",basket);
  fTree->Branch("evtaps",fevtaps,"fevtaps[fnvtaps]/F",basket);
  fTree->Branch("icryst",ficryst,"ficryst[fnhits]/I",basket);
  fTree->Branch("ictaps",fictaps,"fictaps[fntaps]/I",basket);
  if (fStorePrimaries)
  {
    G4cout << "Storing IDs of primary particles" << G4endl;
    fTree->Branch("pcryst",fpcryst,"fpcryst[fnhits]/I",basket);
    fTree->Branch("pctaps",fpctaps,"fpctaps[fntaps]/I",basket);
  }
  fTree->Branch("ivtaps",fivtaps,"fictaps[fnvtaps]/I",basket);
  fTree->Branch("idpart",fidpart,"fidpart[fnpart]/I",basket);
  fTree->Branch("iveto",fiveto,"fiveto[fvhits]/I",basket);
  if (fDET->GetUseMWPC() && fDET->GetUseMWPC() / 10 == 0)
  {
    fTree->Branch("nmwpc",&fnmwpc,"fnmwpc/I",basket);
    fTree->Branch("imwpc",fimwpc,"fimwpc[fnmwpc]/I",basket);
    fTree->Branch("mposx",fmposx,"fmposx[fnmwpc]/F",basket);
    fTree->Branch("mposy",fmposy,"fmposy[fnmwpc]/F",basket);
    fTree->Branch("mposz",fmposz,"fmposz[fnmwpc]/F",basket);
    fTree->Branch("emwpc",femwpc,"femwpc[fnmwpc]/F",basket);
  }
  else
    G4cout<<"A2CBOutput::SetBranches() Disabling MWPC readout"<<G4endl;
  //tof stuff
  if(fToFTot>0){
    fTree->Branch("ntof",&fntof,"fntof/I",basket);
    fTree->Branch("tofi",ftofi,"ftofi[fntof]/I",basket);
    fTree->Branch("tofe",ftofe,"ftofe[fntof]/F",basket);
    fTree->Branch("toft",ftoft,"ftoft[fntof]/F",basket);
    fTree->Branch("tofx",ftofx,"ftofx[fntof]/F",basket);
    fTree->Branch("tofy",ftofy,"ftofy[fntof]/F",basket);
    fTree->Branch("tofz",ftofz,"ftofz[fntof]/F",basket);
  }
  fTree->Branch("npiz",&fnpiz,"fnpiz/I",basket);
  fTree->Branch("ipiz",fipiz,"fipiz[fnpiz]/I",basket);
  fTree->Branch("epiz",fepiz,"fepiz[fnpiz]/F",basket);
  fTree->Branch("tpiz",ftpiz,"ftpiz[fnpiz]/F",basket);
 }
void A2CBOutput::WriteHit(G4HCofThisEvent* HitsColl){
  G4int CollSize=HitsColl->GetNumberOfCollections();
  //G4cout<<"Collection size "<<CollSize<<" "<<HitsColl->GetHC(0)->GetName()<<" "<<HitsColl->GetHC(1)->GetName()<<G4endl;
  //G4cout<<"Collection size "<<CollSize<<G4endl;
  fnhits=fntaps=fnvtaps=fvhits=fntof=fnpiz=fnmwpc=0;
  fetot=0;
  G4int hci=0;
  for(G4int i=0;i<CollSize;i++){
    //Get the ball hit info to be written to output
    A2HitsCollection* hc;
    //Look for the hits collections with entries 
    while(!(hc=static_cast<A2HitsCollection*>(HitsColl->GetHC(hci++))));
    //if(!hc)continue; //no hits in that detector
    G4int hc_nhits=hc->entries();
    //G4cout<<i<<" "<<hc->GetName()<< " "<<hc_nhits<<G4endl;
    if(hc->GetName()=="A2SDHitsCBSD"||hc->GetName()=="A2SDHitsVisCBSD"){
      fnhits=hc_nhits;
      for(Int_t ii=0;ii<fnhits;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	fecryst[ii]=hit->GetEdep()/GeV;
	ftcryst[ii]=hit->GetTime()/ns;
	ficryst[ii]=hit->GetID();
	fpcryst[ii]=hit->GetParticle();
	fetot+=fecryst[ii];
      }
    }
    if(hc->GetName()=="A2SDHitsTAPSSD"||hc->GetName()=="A2SDHitsTAPSVisSD"){
      fntaps=hc_nhits;
      for(Int_t ii=0;ii<fntaps;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	fectapsl[ii]=hit->GetEdep()/GeV;
	fictaps[ii]=hit->GetID();
	ftctaps[ii]=hit->GetTime()/ns;
	fpctaps[ii]=hit->GetParticle();
	//fetot+=fectapsl[i];//***TEMP!!!!
      }
    }
    if(hc->GetName()=="A2SDHitsTAPSVSD"||hc->GetName()=="A2SDHitsTAPSVVisSD"){
      fnvtaps=hc_nhits;
      for(Int_t ii=0;ii<fnvtaps;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	fevtaps[ii]=hit->GetEdep()/GeV;
	fivtaps[ii]=hit->GetID();
      }
    }
    if(hc->GetName()=="A2SDHitsPIDSD"){
      fvhits=hc_nhits;
      for(Int_t ii=0;ii<fvhits;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	feveto[ii]=hit->GetEdep()/GeV;
	ftveto[ii]=hit->GetTime()/ns;
	fiveto[ii]=hit->GetID();
      }
    }
    if(hc->GetName().contains("A2MWPCSD")){
      // fnmwpc=hc_nhits;
      for(Int_t ii=0;ii<hc_nhits;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	fimwpc[fnmwpc+ii]   = hit->GetID();
	fmposx[fnmwpc+ii] = (Float_t)hit->GetPos().getX();
	fmposy[fnmwpc+ii] = (Float_t)hit->GetPos().getY();
	fmposz[fnmwpc+ii] = (Float_t)hit->GetPos().getZ();
	femwpc[fnmwpc+ii] = (Float_t)hit->GetEdep()/GeV;
	//	G4cout<<fnmwpc+i<<" "<<fimwpc[fnmwpc+i]<<" "<<fmpos[fnmwpc+i][0]<<" "<<fmpos[fnmwpc+i][1]<<" "<<fmpos[fnmwpc+i][2]<<G4endl;
      }
      fnmwpc+=hc_nhits;
    }
    if(hc->GetName()=="A2SDHitsTOFSD"){
      fntof=hc_nhits;
      for(Int_t ii=0;ii<fntof;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	ftofe[ii]=hit->GetEdep()/GeV;
	ftoft[ii]=hit->GetTime()/ns;
	ftofx[ii]=hit->GetPos().x()/cm;
	ftofy[ii]=hit->GetPos().y()/cm;
	ftofz[ii]=hit->GetPos().z()/cm;
	ftofi[ii]=hit->GetID();
      }
    }
    if(hc->GetName()=="A2SDHitsPizzaSD" || hc->GetName()=="A2SDHitsPizzaVisSD"){
      fnpiz=hc_nhits;
      for(Int_t ii=0;ii<fnpiz;ii++){
	A2Hit* hit=static_cast<A2Hit*>(hc->GetHit(ii));
	fepiz[ii]=hit->GetEdep()/GeV;
	ftpiz[ii]=hit->GetTime()/ns;
	fipiz[ii]=hit->GetID();
      }
    }
  }
  
}
void A2CBOutput::WriteGenInput(){
  //Note fvertex is already the pointer to fPGA::fGenPosition 
  //Get the generated input info to be written to output 
  TVector3 vec=fBeamLorentzVec->Vect().Unit();
  fbeam[0]=vec.X();
  fbeam[1]=vec.Y();
  fbeam[2]=vec.Z();
  fbeam[3]=fBeamLorentzVec->Rho()/GeV;
  fbeam[4]=fBeamLorentzVec->E()/GeV;

  //Loop over the input particles and write their real kinematics
  for(Int_t i=0;i<fnpart;i++){
    //+1 , index 0 is empty
    //if(!fGenLorentzVec[i+1])continue;
    vec=fGenLorentzVec[i+1]->Vect().Unit();
    // G4cout<<i<<" "<<vec.X()<<" "<<vec.Y()<<" "<<vec.Z()<<G4endl;
    // G4cout<<" "<<fGenLorentzVec[i+1]->E()<<" "<<fGenLorentzVec[i+1]->Rho()<<G4endl;
    fdircos[i][0]=static_cast<Float_t>(vec.X());
    fdircos[i][1]=static_cast<Float_t>(vec.Y());
    fdircos[i][2]=static_cast<Float_t>(vec.Z());
    // G4cout<<fdircos[i][0]<<" "<<fdircos[i][1]<<" "<<fdircos[i][2]<<G4endl;
    felab[i]=fGenLorentzVec[i+1]->E()/GeV;
    fplab[i]=fGenLorentzVec[i+1]->Rho()/GeV;
    fidpart[i]=fGenPartType[i+1];
  }
}
