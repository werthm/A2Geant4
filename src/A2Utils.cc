// Utility methods
// Author: Dominik Werthmueller, 2018

#include "G4VPhysicalVolume.hh"

#include "A2Utils.hh"

//______________________________________________________________________________
void A2Utils::CheckOverlapAndAbort(G4VPhysicalVolume* vol, const G4String& location)
{
    // Check if the volume 'vol' has overlaps and abort if that is the case.
    // Use 'location' in the error message to indicate the origin of the
    // problem.

    // check for overlaps
    if (vol->CheckOverlaps())
    {
        G4cerr << location << ": Overlap in volume " << vol->GetName() << " detected!" << G4endl;
        exit(1);
    }
}

