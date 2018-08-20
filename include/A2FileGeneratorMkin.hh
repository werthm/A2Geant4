// event generator reading mkin-files
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGeneratorMkin_h
#define A2FileGeneratorMkin_h 1

#include "Rtypes.h"

#include "A2FileGeneratorTree.hh"

class A2FileGeneratorMkin : public A2FileGeneratorTree
{

protected:
    Float_t fVertexBr[3];   // vertex
    Float_t fBeamBr[5];     // beam 4-momentum
    G4int fNPart;           // number of particles
    Float_t** fPartPBr;     //[fNPart] particle 4-momenta
    Float_t** fPartVBr;     //[fNPart] particle vertices

    static const G4int fgMaxParticles;

public:
    A2FileGeneratorMkin(const char* filename);
    virtual ~A2FileGeneratorMkin();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
    virtual G4int GetMaxParticles();
};

#endif

