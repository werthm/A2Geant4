// event generator reading a ROOT TTree
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGeneratorTree_h
#define A2FileGeneratorTree_h 1

#include "A2FileGenerator.hh"

class TFile;
class TTree;

class A2FileGeneratorTree : public A2FileGenerator
{

protected:
    TFile* fFile;           // ROOT input file
    TTree* fTree;           // input tree

    G4bool LinkBranch(const char* bname, void* addr, G4bool verbose = true);

public:
    A2FileGeneratorTree(const char* filename, EFileGenType type,
                        const char* treename);
    virtual ~A2FileGeneratorTree();

    virtual G4bool Init();
    virtual G4bool ReadEvent(G4int event);
};

#endif

