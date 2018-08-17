// event generator reading Pluto-files
// Author: Dominik Werthmueller, 2018

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TTree.h"
#include "TMath.h"

#include "A2FileGeneratorPluto.hh"
#include "MCNtuple.h"

using namespace CLHEP;

//______________________________________________________________________________
A2FileGeneratorPluto::A2FileGeneratorPluto(const char* filename)
    : A2FileGeneratorTree(filename, "data")
{
    // Constructor.

    // init members
}

//______________________________________________________________________________
A2FileGeneratorPluto::~A2FileGeneratorPluto()
{
    // Destructor.

}

//______________________________________________________________________________
G4bool A2FileGeneratorPluto::Init()
{
    // Init the file event reader.

    // call parent method
    A2FileGeneratorTree::Init();

    return true;
}

//______________________________________________________________________________
G4bool A2FileGeneratorPluto::ReadEvent(G4int event)
{
    // Read the event 'event'.

    // read tree entry
    fTree->GetEntry(event);

    return true;
}

