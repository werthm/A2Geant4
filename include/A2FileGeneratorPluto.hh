// event generator reading Pluto-files
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGeneratorPluto_h
#define A2FileGeneratorPluto_h 1

#include "Rtypes.h"

#include "A2FileGeneratorTree.hh"

class A2FileGeneratorPluto : public A2FileGeneratorTree
{

protected:

public:
    A2FileGeneratorPluto(const char* filename);
    virtual ~A2FileGeneratorPluto();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
};

#endif

