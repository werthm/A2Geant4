// event generator reading GiBUU ROOT files
// Author: Dominik Werthmueller, 2019

#ifndef A2FileGeneratorGiBUU_h
#define A2FileGeneratorGiBUU_h 1

#include "TTreeReaderValue.h"

#include "A2FileGeneratorTree.hh"

class TTreeReader;

class A2FileGeneratorGiBUU : public A2FileGeneratorTree
{

protected:
    TTreeReader* fReader;                                   // tree reader
    TTreeReaderValue<G4double>* fReaderWeight;              // event weight
    TTreeReaderValue<std::vector<G4int>>* fReaderCode;      // particle code
    TTreeReaderValue<std::vector<G4double>>* fReaderPx;     // particle momentum
    TTreeReaderValue<std::vector<G4double>>* fReaderPy;     // particle momentum
    TTreeReaderValue<std::vector<G4double>>* fReaderPz;     // particle momentum
    TTreeReaderValue<std::vector<G4double>>* fReaderE;      // particle energy
    TTreeReaderValue<std::vector<G4double>>* fReaderX;      // particle position
    TTreeReaderValue<std::vector<G4double>>* fReaderY;      // particle position
    TTreeReaderValue<std::vector<G4double>>* fReaderZ;      // particle position

    static const G4int fgMaxParticles;

public:
    A2FileGeneratorGiBUU(const char* filename);
    virtual ~A2FileGeneratorGiBUU();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
    virtual G4int GetMaxParticles();
};

#endif

