
#ifndef A2PrimaryGeneratorAction_h
#define A2PrimaryGeneratorAction_h 1

#include "TString.h"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Types.hh"

class TLorentzVector;
class G4ParticleGun;
class G4Event;
class A2PrimaryGeneratorMessenger;
class A2DetectorConstruction;
class A2FileGenerator;

//Event generator mode
enum { EPGA_g4, EPGA_phase_space, EPGA_FILE, EPGA_Overlap};

class A2PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  A2PrimaryGeneratorAction();
  ~A2PrimaryGeneratorAction();

  public:
  void GeneratePrimaries(G4Event*);

  void SetUpFileInput();
  void SetInputFile(TString filename){fInFileName=filename;};
  void SetNParticlesToBeTracked(Int_t n){
    fNToBeTracked=n;
    fTrackThis=new Int_t[n];
    for (G4int i = 0; i < fNToBeTracked; i++)
      fTrackThis[i] = -99;
  }
  void SetParticlesToBeTracked(Int_t ipart){if(fTrackThis) fTrackThis[fNToBeTcount++]=ipart;}
  Int_t *GetParticlesToBeTracked(){return fTrackThis;}
  Int_t GetNParticlesToBeTracked(){return fNToBeTracked;}
  G4bool IsTracked(Int_t part){
    for (G4int i = 0; i < fNToBeTracked; i++)
      if (part == fTrackThis[i])
        return true;
    return false;
  }
  G4int GetNEvents();

  Int_t GetNGenParticles(){return fNGenParticles;}
  Int_t GetNGenMaxParticles(){return fNGenMaxParticles;}
  TLorentzVector* GetGenLorentzVec(Int_t i){return fGenLorentzVec[i];}
  TLorentzVector** GetGenLorentzVecs(){return fGenLorentzVec;}
  TLorentzVector* GetBeamLorentzVec(){return fBeamLorentzVec;}
  Float_t* GetVertex(){return fGenPosition;}
  Int_t* GetGenPartType(){return fGenPartType;}

  void SetDetCon(A2DetectorConstruction* det){fDetCon=det;}

private:
  G4ParticleGun*  fParticleGun;    //pointer to particle gun
  A2PrimaryGeneratorMessenger* fGunMessenger; //messenger of this class
  A2DetectorConstruction* fDetCon;   //pointer to the detector volumes
  TString fInFileName;  //Name of input file
  Int_t fNGenParticles;     //Number of particles in ntuple
  Int_t fNGenMaxParticles;     //Maximum number of particles in ntuple
  Float_t fGenPosition[3]; //vertex position from ntuple, can't be double!
  TLorentzVector ** fGenLorentzVec;    //4 vector components from the ntuple branches converted into a ROOT lorentz vector
  TLorentzVector* fBeamLorentzVec; //For the beam or nonntuple input
  A2FileGenerator* fFileGen;    // pointer to input file generator

  Int_t *fGenPartType;        //Array of G3 particle types
  Int_t *fTrackThis;         //Array carrying the index of particles to be tracked
  Int_t fNToBeTracked;    //Number of particles in input file to be tracked
  Int_t fNToBeTcount;     //counter for setting fTrackThis array
  Int_t fNevent;          //event number for the ROOT tree

  G4int fMode;    //select events via standard, phase space or ROOT input
public:
  void SetMode(G4int mode);
  G4int GetMode(){return fMode;}
  A2FileGenerator* GetFileGen() const { return fFileGen; }
  //for phase space generator
private:
  void PhaseSpaceGenerator(G4Event* anEvent);
  void OverlapGenerator(G4Event* anEvent);
  G4float fTmin;       //Min phase spce kinetic energy
  G4float fTmax;       //Max phase space kinetic energy
  G4float fThetamin;       //Min phase spce angle
  G4float fThetamax;       //Max phase space angle
  G4float fBeamEnergy;    //beam energy
  G4float fBeamXSigma;    //beam X width
  G4float fBeamYSigma;    //beam X width
  G4float fBeamDiameter;    //beam diameter on target
  G4float fTargetZ0;      //Target Z position
  G4float fTargetThick;   //Target thickness
  G4float fTargetRadius;   //Target radius (in x-y plane)
  //G4bool fdoPhaseSpace;
  G4float fSplitTheta;  //angle between particles for overlap generator

public:
  void SetTmin(G4float min){fTmin=min;}
  void SetTmax(G4float max){fTmax=max;}
  void SetThetamin(G4float min){fThetamin=min;}
  void SetThetamax(G4float max){fThetamax=max;}
  void SetBeamEnergy(G4float energy){fBeamEnergy=energy;}
  void SetBeamXSigma(G4float sigma){fBeamXSigma=sigma;}
  void SetBeamYSigma(G4float sigma){fBeamYSigma=sigma;}
  void SetBeamDiameter(G4float d){fBeamDiameter=d;}
  void SetTargetZ0(G4float z){fTargetZ0=z;}
  void SetTargetThick(G4float z){fTargetThick=z;}
  void SetTargetRadius(G4float z){fTargetRadius=z;}
 
  void SetSplitTheta(G4float min){fSplitTheta=min;}
 
};


#endif


