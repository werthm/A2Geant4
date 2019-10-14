// event generator reading GiBUU ROOT files
// Author: Dominik Werthmueller, 2019

#ifndef A2FileGeneratorGiBUU_h
#define A2FileGeneratorGiBUU_h 1

#include "A2FileGeneratorTree.hh"

class TTreeReader;

class A2FileGeneratorGiBUU : public A2FileGeneratorTree
{

protected:
    G4double fReaderWeight;             // event weight
    std::vector<G4int>* fReaderCode;    // particle code
    std::vector<G4double>* fReaderPx;   // particle momentum
    std::vector<G4double>* fReaderPy;   // particle momentum
    std::vector<G4double>* fReaderPz;   // particle momentum
    std::vector<G4double>* fReaderE;    // particle energy
    std::vector<G4double>* fReaderX;    // particle position
    std::vector<G4double>* fReaderY;    // particle position
    std::vector<G4double>* fReaderZ;    // particle position

    static const G4int fgMaxParticles;

public:
    A2FileGeneratorGiBUU(const char* filename);
    virtual ~A2FileGeneratorGiBUU();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
    virtual G4int GetMaxParticles();
};

#endif

