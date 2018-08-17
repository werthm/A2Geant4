
#include "A2EventAction.hh"

#include "A2Hit.hh"
#include "A2VisHit.hh"
#include "A2RunAction.hh"
#include "A2EventActionMessenger.hh"
#include "A2Version.hh"
#include "A2FileGenerator.hh"

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "G4VTrajectory.hh"
#include "G4VVisManager.hh"
#include "G4UnitsTable.hh"
#include "G4Trajectory.hh"
#include "G4UImanager.hh"
#include "CLHEP/Units/SystemOfUnits.h"
#include "G4Version.hh"

#include "Randomize.hh"
#include "TString.h"
#include "TSystem.h"
#include "TStopwatch.h"
#include <iomanip>
#include <sys/utsname.h>
#include <fstream>

using namespace CLHEP;

A2EventAction::A2EventAction(A2RunAction* run, A2PrimaryGeneratorAction* pga,
                             int argc, char** argv, const char* detSetup)
{
  frunAct = run;
  fPGA = pga;
  fdrawFlag = "all";
  fprintModulo = 1;
  fEventRate = 0;
  fReqEvents = 0;
  feventMessenger = new A2EventActionMessenger(this);
  fIsInteractive=1;
  // hits collections
  fCBCollID = -1;
  fHitDrawOpt="edep";

  fOutFile=NULL;
  fOutTree=NULL;
  fOutFileName=TString("");

  fprintModulo=1000;
  fTimer = new TStopwatch();
  fCBOut=NULL;
  fOverwriteFile=false;
  fStorePrimaries=true;
  for (int i = 0; i < argc; i++)
  {
    fInvokeCmd += argv[i];
    fInvokeCmd += " ";
  }

  ReadDetectorSetup(detSetup);
}



A2EventAction::~A2EventAction()
{
  delete feventMessenger;
  if (fTimer) delete fTimer;
}



void A2EventAction::BeginOfEventAction(const G4Event* evt)
{
  if (evt->GetEventID() == 0) fTimer->Start();
  if (fPGA->GetMode() == EPGA_FILE && evt->GetEventID() == fReqEvents - 1)
  {
    FormatTimeSec(fTimer->RealTime(), fDuration);
    G4cout << TString::Format("Total tracking time: %s", fDuration.Data()) << G4endl;
  }
}


void A2EventAction::EndOfEventAction(const G4Event* evt)
{
  G4int evtNb = evt->GetEventID();
  if (evtNb && evtNb % fprintModulo == 0)
  {
    //CLHEP::HepRandom::showEngineStatus();
    fEventRate = evtNb / fTimer->RealTime();
    fTimer->Continue();
    G4cout << TString::Format("%7d events tracked (%.2f events/s)", evtNb, fEventRate);
    if (fPGA->GetMode() == EPGA_FILE)
    {
      TString timeFmt;
      FormatTimeSec((fReqEvents - evtNb) / (Double_t)fEventRate, timeFmt);
      G4cout << TString::Format(", %s remaining for %d events", timeFmt.Data(), fReqEvents) << G4endl;
    }
    else
      G4cout << G4endl;
  }

  //   G4cout<<" A2EventAction::EndOfEventAction"<<G4endl;
  //  A2VisHitsCollection* VisCBHC = NULL;
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  //In montecarlo mode
  //write to the output ntuple if it exists
  //if not need to set file via /A2/event/setOutputFile XXX.root
  if(fCBOut){
    fCBOut->WriteHit(HCE);
    fCBOut->WriteGenInput();
    fCBOut->GetTree()->Fill();
  }

  //Draw hits for interactive mode
  //Not for CB will change colour of each crystal hit
  //Currently all of TAPS and PID will change colour
  else if(fIsInteractive==1&&HCE){
    G4int CollSize=HCE->GetNumberOfCollections();
    G4int hci=0;
    //G4cout<<CollSize<<G4endl;
    for(G4int i=0;i<CollSize;i++){
      //Get the ball hit info to be written to output
      A2VisHitsCollection* hc;
    //Look for the hits collections with entries 
      while(!(hc=static_cast<A2VisHitsCollection*>(HCE->GetHC(hci++))));
      //G4cout<<hc->GetName()<<G4endl;
      //if(!hc)continue; //no hits in that detector
      G4int hc_nhits=hc->entries();
      //      if(hc->GetName()=="A2SDHitsVisCBSD"){
      if(hc->GetName().contains("Vis")){
	//	G4cout<<hc_nhits <<" hits"<<G4endl;
	for(G4int ii=0;ii<hc_nhits;ii++){
	  A2VisHit* hit=static_cast<A2VisHit*>(hc->GetHit(ii));
	  // G4cout<<"Got hit "<<hit<<G4endl;
	  hit->Draw(1*MeV,fHitDrawOpt);
	  
	}
	
      }
    }
  } 
  




  // extract the trajectories and draw them

  // You can get a default drawing without this code by using, e.g.,
  // /vis/scene/add/trajectories 1000
  // The code here adds sophistication under control of drawFlag.

  // See comments in G4VTrajectory::DrawTrajectory for the
  // interpretation of the argument, 1000.
  //G4cout <<"Draw Trajectories"<<G4endl; 
  if(G4VVisManager::GetConcreteInstance()) {
    G4UImanager::GetUIpointer()->ApplyCommand("/vis/scene/notifyHandlers");    
    G4TrajectoryContainer* trajContainer = evt->GetTrajectoryContainer();
    G4int n_trajectories = 0;

    if(trajContainer) n_trajectories = trajContainer->entries();
    for (G4int i=0; i<n_trajectories; i++) {
      G4Trajectory* trj = (G4Trajectory*)(*trajContainer)[i];
      if (fdrawFlag == "all") 
        trj->DrawTrajectory();
      else if ((fdrawFlag == "charged") && (trj->GetCharge() != 0.))
        trj->DrawTrajectory();
      else if ((fdrawFlag == "neutral") && (trj->GetCharge() == 0.))
        trj->DrawTrajectory();
    }
    
    // G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");    
  } 
  // G4VVisManager* pVisManager = G4VVisManager::GetConcreteInstance();
  // if (pVisManager)
  //   {
  //    G4TrajectoryContainer* trajectoryContainer = evt->GetTrajectoryContainer();
  //    G4int n_trajectories = 0;
  //    if (trajectoryContainer) n_trajectories = trajectoryContainer->entries();

  //    for (G4int i=0; i<n_trajectories; i++) 
  //       { G4VTrajectory* trj = ((*(evt->GetTrajectoryContainer()))[i]);
  // 	  //trj->ShowTrajectory();
  // 	  //G4cout<<G4endl;
  // 	  if (fdrawFlag == "all") pVisManager->Draw(*trj,100);
  //         else if ((fdrawFlag == "charged")&&(trj->GetCharge() != 0.))
  //                                 pVisManager->Draw(*trj,100);
  //         else if ((fdrawFlag == "neutral")&&(trj->GetCharge() == 0.))
  //                                 pVisManager->Draw(*trj,100);
  //       }
  // }
}  

G4int A2EventAction::PrepareOutput(){
  //If no filename don't save output
  //  fOutFileName=TString("test.root");
  if(fOutFileName==TString("")) {
    G4cout<<"G4int A2EventAction::PrepareOutput() no out put file to be written"<<G4endl;
    G4cout<<"Please supply filename if output required, try :"<<G4endl;
    G4cout<<"/A2/event/SetOutputFile XXX.root"<<G4endl;
    return 0;
  }
  //if filename try to open the file
  fOutFile=new TFile(fOutFileName,"CREATE");
  //if file aready exists make a new name by adding XXXA2copy#.root
  while (!fOutFile->IsOpen()){
    int pos1=fOutFileName.Index("A2copy");
    int pos2= fOutFileName.Index(".root");
 
    if(pos1>0){//already made a copy, make another and increment the counter
      const int leng=pos2-pos1-6;//length of number, 6 digits in A2copy
      TString numb=fOutFileName(pos1+6,leng);//get the number string
      int numbi=numb.Atoi();//get number as integer
      numbi++; //Add 1
      char *newnumb=new char[leng];
      sprintf(newnumb,"%d",numbi);
      fOutFileName.Replace(pos1+6,leng,newnumb);
    }
    else if(pos2>0){//this is the first copy
      fOutFileName.Insert(pos2,"A2copy1");
    }
    else {
      G4cout<<"Input file is not .root, I will exit"<<G4endl;
      exit(0);
    }
    //    fOutFileName.Insert(fOutFileName.Index(".root"),"_1");   
    G4cout<<"A2EventAction::PrepareOutput() Output File already exists will save to "<<fOutFileName<<G4endl;
    fOutFile=new TFile(fOutFileName,"CREATE");
    
  }
  //  while (!fOutFile->IsOpen()){
  //   G4cout<<"A2EventAction::PrepareOutput() Output File already exists do you want to overwrite? y/n"<<G4endl;
  //   G4String ans;
  //   G4cin>>ans;
  //   if(ans==G4String("y"))  fOutFile=new TFile(fOutFileName,"RECREATE");
  //   else {
  //     G4cout<<"You can give a new name or type n to exit A2 simulation:"<<G4endl;
  //     G4cin>>ans;
  //     if(ans=="n") exit(1);
  //     else  {
  // 	fOutFileName=ans;
  // 	fOutFile=new TFile(fOutFileName,"CREATE");
  //     }
  //   }
  // }
  G4cout<<"A2EventAction::PrepareOutput() Output will be written to "<<fOutFileName<<G4endl;

  TDatime date;
  fStartTime = date.AsString();

  //Create output tree
  //This is curently made in the same format as the cbsim output
  fCBOut=new A2CBOutput();
  fCBOut->SetFile(fOutFile);
  fCBOut->SetStorePrimaries(fStorePrimaries);
  fCBOut->SetBranches();
  return 1;
}
void  A2EventAction::CloseOutput(){
  if(!fCBOut) return;
  fCBOut->WriteTree();
  delete fCBOut;

  // write metadata
#if defined(__clang__)
    TString compiler("clang ");
    compiler += __clang_version__;
#elif defined(__GNUC__)
    TString compiler("gcc ");
    compiler += __VERSION__;
#else
    TString compiler("unknown");
#endif
  TString version(G4Version.c_str());
  version.ReplaceAll("$", "");
  version.ReplaceAll(" ", "");
  version.ReplaceAll("Name:", "");
  SysInfo_t sysInfo;
  gSystem->GetSysInfo(&sysInfo);
  struct utsname unameBuffer;
  uname(&unameBuffer);
  TDatime date;
  TString inputFile("none");
  if (fPGA->GetFileGen())
    inputFile = fPGA->GetFileGen()->GetFileName().c_str();
  TString trackedPart("unknown");
  if (fPGA->GetFileGen())
  {
    trackedPart = "";
    for (G4int i = 0; i < fPGA->GetFileGen()->GetNParticles(); i++)
    {
      if (fPGA->GetFileGen()->IsParticleTrack(i))
      {
          if (trackedPart != "")
            trackedPart += ", ";
          trackedPart += TString::Format("%d (%s)", i+1, fPGA->GetFileGen()->GetParticleDefinition(i)->GetParticleName().c_str());
      }
    }
  }

  TNamed meta("A2Geant4 Metadata", TString::Format("\n"
              "       Version            : %s\n"
              "       Geant4 Version     : %s\n"
              "       Compiler           : %s\n"
              "       Hostname           : %s\n"
              "       System model       : %s\n"
              "       OS name            : %s\n"
              "       OS release         : %s\n"
              "       OS version         : %s\n"
              "       OS architecture    : %s\n"
              "       Command            : %s\n"
              "       Detector setup     : %s"
              "       Input file         : %s\n"
              "       Output file        : %s\n"
              "       Tracked particles  : %s\n"
              "       Start time         : %s\n"
              "       Stop time          : %s\n"
              "       Tracking time      : %s\n"
              "       Tracked events     : %d\n"
              "       Average events/sec : %.2f",
              A2_VERSION,
              version.Data(),
              compiler.Data(),
              unameBuffer.nodename,
              sysInfo.fModel.Data(),
              unameBuffer.sysname,
              unameBuffer.release,
              unameBuffer.version,
              unameBuffer.machine,
              fInvokeCmd.Data(),
              fDetSetup.Data(),
              inputFile.Data(),
              fOutFile->GetName(),
              trackedPart.Data(),
              fStartTime.Data(),
              date.AsString(),
              fDuration.Data(),
              fReqEvents,
              fEventRate
              ).Data());
  meta.Write();

  fOutFile->Close();
  if(fOutFile)delete fOutFile;
}

void A2EventAction::FormatTimeSec(double seconds, TString& out)
{
  // convert seconds
  Int_t hours = Int_t(seconds / 3600);
  seconds -= hours * 3600;
  Int_t min = Int_t(seconds / 60);
  seconds -= min * 60;
  Int_t sec = Int_t(seconds);

  // format string
  out.Form("%02d:%02d:%02d", hours, min, sec);
}

void A2EventAction::ReadDetectorSetup(const char* detSetup)
{
  fDetSetup = detSetup;
  fDetSetup += "\n";

  G4String line;
  std::ifstream file;
  file.open(detSetup);
  if (file.is_open())
  {
    while (line.readLine(file))
    {
      if (line[0] == '#') continue;
      fDetSetup += "                            ";
      fDetSetup += line.c_str();
      fDetSetup += "\n";
      G4cout << line << G4endl;
    }
    file.close();
  }
}

