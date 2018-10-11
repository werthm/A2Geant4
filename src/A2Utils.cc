// Utility methods
// Author: Dominik Werthmueller, 2018

#include "G4VPhysicalVolume.hh"
#include "G4TriangularFacet.hh"
#include "G4QuadrangularFacet.hh"
#include "G4TessellatedSolid.hh"

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

//______________________________________________________________________________
G4TriangularFacet** A2Utils::BuildTriSurface(G4int n, const G4double* x,
                                             const G4double* y, G4double z,
                                             G4bool reverse)
{
    // Build a surface defined by the returned array of 'n' triangular facets
    // using 'n' points with coordinates 'x', 'y' and 'z'.
    // If 'reverse' is true, reverse the order of the points inverting the
    // direction of the normal vector of the surface.

    // check number of points
    if (n < 3)
    {
        G4cout << "A2Utils::BuildTriSurface(): At least 3 points needed!" << G4endl;
        return 0;
    }

    // calculate middle point
    G4double mid_x = 0;
    G4double mid_y = 0;
    for (G4int i = 0; i < n; i++)
    {
        mid_x += x[i];
        mid_y += y[i];
    }
    mid_x /= (G4double)n;
    mid_y /= (G4double)n;

    // create triangular facets
    G4TriangularFacet** out = new G4TriangularFacet*[n];
    for (G4int i = 0; i < n; i++)
    {
        // set points
        G4int p1 = i;
        G4int p2 = i == n-1 ? 0 : i+1;

        // create facet
        if (reverse)
        {
            out[i] = new G4TriangularFacet(G4ThreeVector(mid_x, mid_y, z),
                                           G4ThreeVector(x[p2], y[p2], z),
                                           G4ThreeVector(x[p1], y[p1], z),
                                           ABSOLUTE);
        }
        else
        {
            out[i] = new G4TriangularFacet(G4ThreeVector(mid_x, mid_y, z),
                                           G4ThreeVector(x[p1], y[p1], z),
                                           G4ThreeVector(x[p2], y[p2], z),
                                           ABSOLUTE);
        }
    }

    return out;
}

//______________________________________________________________________________
G4QuadrangularFacet** A2Utils::BuildDepthFacets(G4int n, const G4double* x,
                                                const G4double* y, G4double thickness)
{
    // Build an array of of 'n' quadrangular facets using 'n' points with coordinates
    // 'x' and 'y' and use 'thickness' for calculating the z-coordinates.

    // check number of points
    if (n < 2)
    {
        G4cout << "A2Utils::BuildDepthFacets(): At least 2 points needed!" << G4endl;
        return 0;
    }

    // create quadrangular facets
    G4QuadrangularFacet** out = new G4QuadrangularFacet*[n];
    for (G4int i = 0; i < n; i++)
    {
        // set points
        G4int p1 = i == n-1 ? 0 : i+1;
        G4int p2 = i;

        // create facet
        out[i] = new G4QuadrangularFacet(G4ThreeVector(x[p1], y[p1], 0.5*thickness),
                                         G4ThreeVector(x[p2], y[p2], 0.5*thickness),
                                         G4ThreeVector(x[p2], y[p2], -0.5*thickness),
                                         G4ThreeVector(x[p1], y[p1], -0.5*thickness),
                                         ABSOLUTE);
    }

    return out;
}

//______________________________________________________________________________
G4TessellatedSolid* A2Utils::BuildPlanarTessSolid(G4int n, const G4double* x,
                                                  const G4double* y, G4double thickness,
                                                  const G4String& name)
{
    // Build a planar solid named 'name' that is defined by 'n' points with
    // coordinates 'x' and 'y' and thickness 'thickness'.

    // build the front and back surfaces
    G4TriangularFacet** front_facets = BuildTriSurface(n, x, y, 0.5*thickness);
    G4TriangularFacet** back_facets = BuildTriSurface(n, x, y, -0.5*thickness, true);

    // build the depth facets
    G4QuadrangularFacet** depth_facets = BuildDepthFacets(n, x, y, thickness);

    // create the solid
    G4TessellatedSolid* out = new G4TessellatedSolid(name);

    // add all facets
    for (G4int i = 0; i < n; i++)
    {
        out->AddFacet((G4VFacet*) front_facets[i]);
        out->AddFacet((G4VFacet*) depth_facets[i]);
        out->AddFacet((G4VFacet*) back_facets[i]);
    }

    // mark solid as closed
    out->SetSolidClosed(true);

    return out;
}

