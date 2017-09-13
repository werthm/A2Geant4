// A2UserTrackInformation
// Author: Dominik Werthmueller, 2017

#ifndef A2UserTrackInformation_h
#define A2UserTrackInformation_h 1

#include "G4VUserTrackInformation.hh"

class G4Track;

class A2UserTrackInformation : public G4VUserTrackInformation
{

private:
    G4int fTrackID;             // Geant4 track id (starting at 1)
    G4int fPartID;              // particle index in generator (starting at 1)

public:
    A2UserTrackInformation();
    A2UserTrackInformation(const G4Track* aTrack);
    A2UserTrackInformation(const A2UserTrackInformation* aTrackInfo);
    virtual ~A2UserTrackInformation() { }

    G4int GetTrackID() const { return fTrackID; }
    G4int GetPartID() const { return fPartID; }

    void SetTrackID(G4int id) { fTrackID = id; }
    void SetPartID(G4int id) { fPartID = id; }

    void Print() const;
};

#endif

