// Utility methods
// Author: Dominik Werthmueller, 2018

#ifndef A2Utils_h
#define A2Utils_h 1

#include "G4String.hh"

class G4VPhysicalVolume;

namespace A2Utils
{
    void CheckOverlapAndAbort(G4VPhysicalVolume* vol, const G4String& location);
}

#endif

