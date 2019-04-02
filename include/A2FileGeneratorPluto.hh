// event generator reading Pluto-files
// Author: Dominik Werthmueller, 2018-2019

#ifdef WITH_PLUTO

#ifndef A2FileGeneratorPluto_h
#define A2FileGeneratorPluto_h 1

#include "Rtypes.h"
#include "TTreeReaderArray.h"

#include "PParticle.h"

#include "A2FileGeneratorTree.hh"

class TTreeReader;

class A2FileGeneratorPluto : public A2FileGeneratorTree
{

protected:
    TTreeReader* fReader;                       // tree reader
    TTreeReaderArray<PParticle>* fReaderPart;   // particles

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
#endif

