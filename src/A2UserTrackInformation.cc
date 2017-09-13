// A2UserTrackInformation
// Author: Dominik Werthmueller, 2017

#include "G4Track.hh"

#include "A2UserTrackInformation.hh"

//______________________________________________________________________________
A2UserTrackInformation::A2UserTrackInformation()
{
    // Constructor.

    fTrackID = 0;
    fPartID = -1;
}

//______________________________________________________________________________
A2UserTrackInformation::A2UserTrackInformation(const G4Track* aTrack)
{
    // Constructor.

    fTrackID = aTrack->GetTrackID();
    fPartID = -1;
}

//______________________________________________________________________________
A2UserTrackInformation::A2UserTrackInformation(const A2UserTrackInformation* aTrackInfo)
{
    // Constructor.

    fTrackID = aTrackInfo->GetTrackID();
    fPartID = aTrackInfo->GetPartID();
}

//______________________________________________________________________________
void A2UserTrackInformation::Print() const
{
    // Print track information.

    G4cout << "A2UserTrackInformation: "
              "fTrackID: " << fTrackID << "  "
              "fPartID: "  << fPartID  << G4endl;
}

