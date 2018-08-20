// event generator reading Pluto-files
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGeneratorPluto_h
#define A2FileGeneratorPluto_h 1

#include "Rtypes.h"

#include "A2FileGeneratorTree.hh"

class A2FileGeneratorPluto : public A2FileGeneratorTree
{

protected:
    Int_t fNPart;           // number of particles
    Int_t* fPartTop;        //[fNPart] particle top branch
    Double_t* fPartPx;      //[fNPart] particle x-momentum
    Double_t* fPartPy;      //[fNPart] particle y-momentum
    Double_t* fPartPz;      //[fNPart] particle z-momentum
    Double_t* fPartE;       //[fNPart] particle energy
    Double_t* fPartX;       //[fNPart] particle x-vertex
    Double_t* fPartY;       //[fNPart] particle y-vertex
    Double_t* fPartZ;       //[fNPart] particle z-vertex
    Double_t* fPartT;       //[fNPart] particle time
    Int_t* fPartID;         //[fNPart] particle ID
    Int_t* fPartDtrIdx;     //[fNPart] particle daughter index

    G4ParticleDefinition* PlutoToG4(Int_t id);

    static const G4int fgMaxParticles;
    static const G4int fgPlutoG4Conversion[70];

public:
    A2FileGeneratorPluto(const char* filename);
    virtual ~A2FileGeneratorPluto();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
    virtual G4int GetMaxParticles();
};

#endif

