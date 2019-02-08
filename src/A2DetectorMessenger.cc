
#include "A2DetectorMessenger.hh"

#include "A2DetectorConstruction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4ThreeVector.hh"
#include "G4Version.hh"

#if G4VERSION_NUMBER >= 1030
G4ApplicationState cmdState = G4State_Init;
#else
G4ApplicationState cmdState = G4State_PreInit;
#endif


A2DetectorMessenger::A2DetectorMessenger(
                                           A2DetectorConstruction* A2Det)
:fA2Detector(A2Det)
{ 
  fA2Dir = new G4UIdirectory("/A2/");
  fA2Dir->SetGuidance("UI commands of this example");
  
  fdetDir = new G4UIdirectory("/A2/det/");
  fdetDir->SetGuidance("detector control");

  fUseTAPSCmd = new G4UIcmdWithAnInteger("/A2/det/useTAPS",this);
  fUseTAPSCmd->SetGuidance("Construct TAPS");
  fUseTAPSCmd->SetParameterName("UseTAPS",false);
  //fUseTAPSCmd->SetRange("UseTAPS=0 don't build TAPS or UseTAPS!=0 build TAPS");
  fUseTAPSCmd->AvailableForStates(cmdState,G4State_Idle);


  fUseCBCmd = new G4UIcmdWithAnInteger("/A2/det/useCB",this);
  fUseCBCmd->SetGuidance("Construct CB");
  fUseCBCmd->SetParameterName("UseCB",false);
  //fUseCBCmd->SetRange("UseCB=0 don't build CB or UseCB!=0 build CB");
  fUseCBCmd->AvailableForStates(cmdState,G4State_Idle);


  fUsePIDCmd = new G4UIcmdWithAnInteger("/A2/det/usePID",this);
  fUsePIDCmd->SetGuidance("Construct PID");
  fUsePIDCmd->SetParameterName("UsePID",false);
  //fUsePIDCmd->SetRange("UsePID=0 don't build PID or UsePID!=0 build PID");
  fUsePIDCmd->AvailableForStates(cmdState,G4State_Idle);

  fUseMWPCCmd = new G4UIcmdWithAnInteger("/A2/det/useMWPC",this);
  fUseMWPCCmd->SetGuidance("Construct MWPC");
  fUseMWPCCmd->SetParameterName("UseMWPC",false);
  fUseMWPCCmd->AvailableForStates(cmdState,G4State_Idle);

  fUseTargetCmd=new G4UIcmdWithAString("/A2/det/useTarget",this);
  fUseTargetCmd->SetGuidance("Select the type of target");
  fUseTargetCmd->SetGuidance("Either Cryo or Solid");
  fUseTargetCmd->SetParameterName("UseTarget",false);
  fUseTargetCmd->AvailableForStates(cmdState,G4State_Idle);

  fTargetMatCmd=new G4UIcmdWithAString("/A2/det/targetMaterial",this);
  fTargetMatCmd->SetGuidance("Select the target material");
  fTargetMatCmd->SetParameterName("TargetMaterial",false);
  fTargetMatCmd->AvailableForStates(cmdState,G4State_Idle);

  fTargetLengthCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTargetLength",this);
  fTargetLengthCmd->SetGuidance("Set target cell length");
  fTargetLengthCmd->SetParameterName("TargetLength",false);
  fTargetLengthCmd->SetUnitCategory("Length");
  fTargetLengthCmd->AvailableForStates(cmdState,G4State_Idle);

  fTargetRadiusCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTargetRadius",this);
  fTargetRadiusCmd->SetGuidance("Set target radius");
  fTargetRadiusCmd->SetParameterName("TargetRadius",false);
  fTargetRadiusCmd->SetUnitCategory("Length");
  fTargetRadiusCmd->AvailableForStates(cmdState,G4State_Idle);

  fTargetZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTargetZ",this);
  fTargetZCmd->SetGuidance("Set distance of target center from center of ball");
  fTargetZCmd->SetParameterName("TargetZ",false);
  fTargetZCmd->AvailableForStates(cmdState,G4State_Idle);
  fTargetZCmd->SetUnitCategory("Length");
  
  // Target magnetic coils type
  fTargetMagneticCoilsCmd = new G4UIcmdWithAString("/A2/det/targetMagneticCoils",this);
  fTargetMagneticCoilsCmd->SetGuidance("Select the target magnetic coils type");
  fTargetMagneticCoilsCmd->SetParameterName("TargetMagneticCoils",false);
  fTargetMagneticCoilsCmd->AvailableForStates(cmdState,G4State_Idle);

  // Target magnetic field map
  fTargetMagneticFieldCmd = new G4UIcmdWithAString("/A2/det/setTargetMagneticFieldMap",this);
  fTargetMagneticFieldCmd->SetGuidance("Set path/name of the target magnetic field map file");
  fTargetMagneticFieldCmd->SetParameterName("TargetMagneticField",false);
  fTargetMagneticFieldCmd->AvailableForStates(cmdState,G4State_Idle);

  fHemiGapCmd = new G4UIcmdWith3VectorAndUnit("/A2/det/setHemiGap",this);
  fHemiGapCmd->SetGuidance("Set air gap between each hemisphere and equator");
  fHemiGapCmd->SetParameterName("HemiGapUp","HemiGapDown","HemiGapNA",false);
  fHemiGapCmd->SetUnitCategory("Length");
  fHemiGapCmd->AvailableForStates(cmdState,G4State_Idle);

  fCBCrystGeoCmd = new G4UIcmdWithAString("/A2/det/setCBCrystGeo",this);
  fCBCrystGeoCmd->SetGuidance("Set the geometry implementation of the CB crystals.");
  fCBCrystGeoCmd->SetParameterName("GeometryImpl",false);
  fCBCrystGeoCmd->AvailableForStates(cmdState,G4State_Idle);

  fTAPSFileCmd = new G4UIcmdWithAString("/A2/det/setTAPSFile",this);
  fTAPSFileCmd->SetGuidance("Set the taps configuration file.");
  fTAPSFileCmd->SetParameterName("tapsfile",false);
  fTAPSFileCmd->AvailableForStates(cmdState,G4State_Idle);

  fTAPSZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setTAPSZ",this);
  fTAPSZCmd->SetGuidance("Set distance of TAPS from centre of ball");
  fTAPSZCmd->SetParameterName("TAPSZ",false);
  fTAPSZCmd->SetUnitCategory("Length");
  fTAPSZCmd->AvailableForStates(cmdState,G4State_Idle);

  fTAPSNCmd = new G4UIcmdWithAnInteger("/A2/det/setTAPSN",this);
  fTAPSNCmd->SetGuidance("Set number of active taps crystals");
  fTAPSNCmd->SetParameterName("NTAPS",false);
  fTAPSNCmd->SetRange("NTAPS>0 && NTAPS<=552");
  fTAPSNCmd->AvailableForStates(cmdState,G4State_Idle);

  fTAPSPbCmd = new G4UIcmdWithAnInteger("/A2/det/setTAPSPbWO4Rings",this);
  fTAPSPbCmd->SetGuidance("Set number of active BaF2 rings");
  fTAPSPbCmd->SetParameterName("NPbWO4",false);
  fTAPSPbCmd->SetRange("NPbWO4>=0 && NPbWO4<=15");
  fTAPSPbCmd->AvailableForStates(cmdState,G4State_Idle);

  fPIDZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setPIDZ",this);
  fPIDZCmd->SetGuidance("Set distance of PID from centre of ball");
  fPIDZCmd->SetParameterName("PIDZ",false);
  fPIDZCmd->SetUnitCategory("Length");
  fPIDZCmd->AvailableForStates(cmdState,G4State_Idle);

  fPIDRotCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setPIDRotation",this);
  fPIDRotCmd->SetGuidance("Set angle of rotation around z-axis of PID");
  fPIDRotCmd->SetParameterName("PIDRotation",false);
  fPIDRotCmd->SetUnitCategory("Angle");
  fPIDRotCmd->AvailableForStates(cmdState,G4State_Idle);

  fUpdateCmd = new G4UIcmdWithoutParameter("/A2/det/update",this);
  fUpdateCmd->SetGuidance("Update calorimeter geometry.");
  fUpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  fUpdateCmd->SetGuidance("if you changed geometrical value(s).");
  fUpdateCmd->AvailableForStates(G4State_Idle);

  fUseTOFCmd = new G4UIcmdWithAnInteger("/A2/det/useTOF",this);
  fUseTOFCmd->SetGuidance("Construct TOF");
  fUseTOFCmd->SetParameterName("UseTOF",false);
  fUseTOFCmd->AvailableForStates(cmdState,G4State_Idle);

  fTOFFileCmd = new G4UIcmdWithAString("/A2/det/setTOFFile",this);
  fTOFFileCmd->SetGuidance("Set the tof configuration file.");
  fTOFFileCmd->SetParameterName("toffile",false);
  fTOFFileCmd->AvailableForStates(cmdState,G4State_Idle);

  fUseCherenkovCmd = new G4UIcmdWithAnInteger("/A2/det/useCherenkov",this);
  fUseCherenkovCmd->SetGuidance("Construct Cherenkov");
  fUseCherenkovCmd->SetParameterName("UseCherenkov",false);
  //fUseCherenkovCmd->SetRange("UseCherenkov=0 don't build Cherenkov or UseCherenkov!=0 build Cherenkov");
  fUseCherenkovCmd->AvailableForStates(cmdState,G4State_Idle);

  fUsePizzaCmd = new G4UIcmdWithAnInteger("/A2/det/usePizza",this);
  fUsePizzaCmd->SetGuidance("Construct Pizza detector");
  fUsePizzaCmd->SetParameterName("UsePizza",false);
  fUseCherenkovCmd->AvailableForStates(cmdState,G4State_Idle);

  fPizzaZCmd = new G4UIcmdWithADoubleAndUnit("/A2/det/setPizzaZ",this);
  fPizzaZCmd->SetGuidance("Set distance of Pizza detector from centre of ball");
  fPizzaZCmd->SetParameterName("PizzaZ",false);
  fPizzaZCmd->SetUnitCategory("Length");
  fPizzaZCmd->AvailableForStates(cmdState,G4State_Idle);
}



A2DetectorMessenger::~A2DetectorMessenger()
{
  delete fUseTAPSCmd;
  delete fUseCBCmd;
  delete fUsePIDCmd;
  delete fUseTargetCmd;
  delete fTargetMatCmd;
  delete fTargetMagneticCoilsCmd;
  delete fUpdateCmd;
  delete fTAPSFileCmd;
  delete fTAPSZCmd;
  delete fTAPSNCmd;
  delete fTAPSPbCmd;
  delete fPIDZCmd;
  delete fPIDRotCmd;
  delete fUseMWPCCmd;
  delete fTOFFileCmd;
  delete fUseTOFCmd;
  delete fUseCherenkovCmd;
  delete fUsePizzaCmd;
  delete fPizzaZCmd;
  delete fTargetLengthCmd;
  delete fTargetRadiusCmd;
  delete fTargetZCmd;
  delete fTargetMagneticFieldCmd;
  delete fHemiGapCmd;
  delete fCBCrystGeoCmd;
 }



void A2DetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 


  if( command == fUseTAPSCmd )
    { fA2Detector->SetUseTAPS(fUseTAPSCmd->GetNewIntValue(newValue));}

  if( command == fUseCBCmd )
    { fA2Detector->SetUseCB(fUseCBCmd->GetNewIntValue(newValue));}

  if( command == fUsePIDCmd )
    { fA2Detector->SetUsePID(fUsePIDCmd->GetNewIntValue(newValue));}


  if( command == fUseMWPCCmd )
    { fA2Detector->SetUseMWPC(fUseMWPCCmd->GetNewIntValue(newValue));}

  if( command == fUseCherenkovCmd )
    { fA2Detector->SetUseCherenkov(fUseCherenkovCmd->GetNewIntValue(newValue));}

  if( command == fUsePizzaCmd )
    { fA2Detector->SetUsePizza(fUsePizzaCmd->GetNewIntValue(newValue));}

  if( command == fPizzaZCmd )
    { fA2Detector->SetPizzaZ(fPizzaZCmd->GetNewDoubleValue(newValue));}

  if( command == fUpdateCmd )
    { fA2Detector->UpdateGeometry(); }
  
  if( command == fUseTargetCmd )
    { fA2Detector->SetUseTarget(newValue);}
  
  if( command == fTargetMatCmd )
    { fA2Detector->SetTargetMaterial(newValue);}
    
  if ( command == fTargetMagneticCoilsCmd )
    { fA2Detector->SetTargetMagneticCoils(newValue); }
  
  if( command == fTargetLengthCmd )
    { fA2Detector->SetTargetLength(fTargetLengthCmd->GetNewDoubleValue(newValue));}

  if( command == fTargetRadiusCmd )
    { fA2Detector->SetTargetRadius(fTargetRadiusCmd->GetNewDoubleValue(newValue));}

  if( command == fTargetZCmd )
    { fA2Detector->SetTargetZ(fTargetZCmd->GetNewDoubleValue(newValue));}

  // Target magnetic field map
  if( command == fTargetMagneticFieldCmd )
    { fA2Detector->SetTargetMagneticFieldMap(newValue); }

   if( command == fHemiGapCmd )
    { fA2Detector->SetHemiGap(fHemiGapCmd->GetNew3VectorValue(newValue));}

   if( command == fCBCrystGeoCmd )
    { fA2Detector->SetCBCrystGeometry(newValue);}
 
  if( command == fTAPSFileCmd )
    { fA2Detector->SetTAPSFile(newValue);}
  
  if( command == fTAPSZCmd )
    { fA2Detector->SetTAPSZ(fTAPSZCmd->GetNewDoubleValue(newValue));}
  
  
  if( command == fTAPSNCmd )
    {fA2Detector->SetTAPSN(fTAPSNCmd->GetNewIntValue(newValue));}
  
  if( command == fTAPSPbCmd )
    {fA2Detector->SetTAPSPbWO4Rings(fTAPSPbCmd->GetNewIntValue(newValue));}
  
  if( command == fPIDZCmd )
    { fA2Detector->SetPIDZ(fPIDZCmd->GetNewDoubleValue(newValue));}

  if( command == fPIDRotCmd )
    { fA2Detector->SetPIDRotation(fPIDRotCmd->GetNewDoubleValue(newValue));}
  
  if( command == fTOFFileCmd )
    { fA2Detector->SetTOFFile(newValue);}
  
  if( command == fUseTOFCmd )
    { fA2Detector->SetUseTOF(fUseTOFCmd->GetNewIntValue(newValue));}
  
 }


