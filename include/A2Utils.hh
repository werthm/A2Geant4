// Utility methods
// Author: Dominik Werthmueller, 2018

#ifndef A2Utils_h
#define A2Utils_h 1

#include "G4String.hh"

class G4VPhysicalVolume;
class G4TriangularFacet;
class G4QuadrangularFacet;
class G4TessellatedSolid;

namespace A2Utils
{
    // geometry
    void CheckOverlapAndAbort(G4VPhysicalVolume* vol, const G4String& location);
    G4TriangularFacet** BuildTriSurface(G4int n, const G4double* x,
                                        const G4double* y, G4double z,
                                        G4bool reverse = false);
    G4QuadrangularFacet** BuildDepthFacets(G4int n, const G4double* x,
                                           const G4double* y, G4double thickness);
    G4TessellatedSolid* BuildPlanarTessSolid(G4int n, const G4double* x,
                                             const G4double* y, G4double thickness,
                                             const G4String& name);
}

#endif

