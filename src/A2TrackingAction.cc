// A2TrackingAction
// Author: Dominik Werthmueller, 2017

#include "G4Track.hh"
#include "G4TrackingManager.hh"
#include "G4RunManager.hh"

#include "A2TrackingAction.hh"
#include "A2UserTrackInformation.hh"
#include "A2PrimaryGeneratorAction.hh"

//______________________________________________________________________________
A2TrackingAction::A2TrackingAction()
{
    // Constructor.

    fPGA = (A2PrimaryGeneratorAction*)
            G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction();
}

//______________________________________________________________________________
A2TrackingAction::~A2TrackingAction()
{
    // Destructor.

}

//______________________________________________________________________________
void A2TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    // Overwrite PreUserTrackingAction().

    // check for secondary track
    if (aTrack->GetParentID() == 0 && aTrack->GetUserInformation() == 0)
    {
        // create and set track information object
        A2UserTrackInformation* anInfo = new A2UserTrackInformation(aTrack);
        if (fPGA->GetNParticlesToBeTracked())
            anInfo->SetPartID(fPGA->GetParticlesToBeTracked()[anInfo->GetTrackID()-1]);
        else
            anInfo->SetPartID(1);   // set dummy ID
        G4Track* theTrack = (G4Track*)aTrack;
        theTrack->SetUserInformation(anInfo);
    }
    else if (aTrack->GetUserInformation() == 0)
    {
        G4cerr << "A2TrackingAction::PreUserTrackingAction: No tracking "
                  "information found for secondary track!"
               << G4endl;
        exit(-1);
    }
}

//______________________________________________________________________________
void A2TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
    // Overwrite PostUserTrackingAction().

    // get list of secondaries
    G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();

    // secondaries found
    if (secondaries)
    {
        // create new track information object
        A2UserTrackInformation* info = (A2UserTrackInformation*)
                                        aTrack->GetUserInformation();

        // loop over list of secondaries
        size_t nSeco = secondaries->size();
        for (size_t i = 0; i < nSeco; i++)
        {
            // set track information object if no object is present
            if ((*secondaries)[i]->GetUserInformation() == 0)
            {
                A2UserTrackInformation* infoNew = new A2UserTrackInformation(info);
                (*secondaries)[i]->SetUserInformation(infoNew);
            }
        }
    }
}

