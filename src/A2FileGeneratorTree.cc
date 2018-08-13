// event generator reading a ROOT TTree
// Author: Dominik Werthmueller, 2018

#include "TFile.h"
#include "TTree.h"

#include "A2FileGeneratorTree.hh"

//______________________________________________________________________________
A2FileGeneratorTree::A2FileGeneratorTree(const char* filename, const char* treename)
    : A2FileGenerator(filename)
{
    // Constructor.

    // init members
    fFile = 0;
    fTree = 0;

    // try to open input file
    fFile = new TFile(fFileName.c_str());
    if (!fFile || fFile->IsZombie())
    {
        G4cout << "A2FileGeneratorTree::A2FileGeneratorTree(): Could not open the ROOT input file "
               << fFileName << "!" << G4endl;
        return;
    }

    // try to open the tree
    fTree = (TTree*)fFile->Get(treename);
    if (!fTree)
    {
        G4cout << "A2FileGeneratorTree::A2FileGeneratorTree(): Could not open the ROOT tree "
               << treename << " in the file " << fFileName << "!" << G4endl;
        return;
    }
}

//______________________________________________________________________________
A2FileGeneratorTree::~A2FileGeneratorTree()
{
    // Destructor.

    if (fTree)
        delete fTree;
    if (fFile)
        delete fFile;
}

//______________________________________________________________________________
Bool_t A2FileGeneratorTree::LinkBranch(const char* bname, void* addr, Bool_t verbose)
{
    // Try to link the branch with name 'bname' to the pointer 'addr'.
    // Return kTRUE on success, otherwise kFALSE.

    // look for branch
    TBranch* b = fTree->GetBranch(bname);

    // check branch
    if (b)
    {
        b->SetAddress(addr);
        return kTRUE;
    }
    else
    {
        if (verbose)
            G4cout << "A2FileGeneratorTree::LinkBranch(): Branch " << bname << " not found in tree "
                   << fTree->GetName() << "!" << G4endl;
        return kFALSE;
    }
}

