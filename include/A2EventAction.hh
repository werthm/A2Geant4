
#ifndef A2EventAction_h
#define A2EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"

#include "A2CBOutput.hh"

class A2RunAction;
class A2PrimaryGeneratorAction;
class A2EventActionMessenger;
class TStopwatch;

class A2EventAction : public G4UserEventAction
{
 public:
   A2EventAction(A2RunAction*, A2PrimaryGeneratorAction*,
                 int argc, char** argv, const char* detSetup);
  ~A2EventAction();

 public:
   void  BeginOfEventAction(const G4Event*);
   void    EndOfEventAction(const G4Event*);
    
   void SetDrawFlag   (G4String val)  {fdrawFlag = val;}
   void SetOverwriteFile   (G4bool val)  {fOverwriteFile = val;}
   void SetPrintModulo(G4int    val)  {fprintModulo = val;}
   void SetReqEvents(G4int ev) { fReqEvents = ev; }
  void SetCBCollID(G4int val){fCBCollID=val;}
  void SetIsInteractive(G4int is){fIsInteractive=is;}
  void SetHitDrawOpt(G4String val){fHitDrawOpt=val;}
  void SetStorePrimaries(G4bool val) { fStorePrimaries = val; }
  void SetOutFileName(TString name){fOutFileName=name;}
  G4int PrepareOutput();
  void CloseOutput();
 private:
   A2RunAction*  frunAct;
   A2PrimaryGeneratorAction* fPGA;
  A2CBOutput* fCBOut;

  G4int fIsInteractive;        
   G4String  fdrawFlag;
   G4int     fprintModulo;
   G4double fEventRate;
   G4int fReqEvents;
  TStopwatch* fTimer;
   //G4int     fDrawMode;
  G4String fHitDrawOpt;
  G4bool fOverwriteFile;
  G4bool fStorePrimaries;
  TString fInvokeCmd;
  TString fStartTime;
  TString fDuration;
  TString fDetSetup;

   A2EventActionMessenger*  feventMessenger;

  G4int fCBCollID;

  //ROOT output stuff
  TFile* fOutFile;
  TTree* fOutTree;
  TString fOutFileName;

  static void FormatTimeSec(double seconds, TString& out);
  void ReadDetectorSetup(const char* detSetup);
};

#endif

    
