// Generic solid target supporting different types used
// in past experiments.
// Author: Dominik Werthmueller, 2018

#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"

#include "A2SolidTargetGeneric.hh"
#include "A2Utils.hh"

using namespace CLHEP;

//______________________________________________________________________________
A2SolidTargetGeneric::A2SolidTargetGeneric(ESolidTargetType type)
    : A2Target()
{
    // Constructor.

    // init members
    fLength = 1*cm;
    fRadius = 2*cm;
    fType = type;
}

//______________________________________________________________________________
G4VPhysicalVolume* A2SolidTargetGeneric::Construct(G4LogicalVolume* motherLogic, G4double z0)
{
    // Construct the target.

    // some variables
    G4VPhysicalVolume* v;

    // global geometry data
    G4double airTube_z0 = z0;
    G4double airTube_radius = fRadius + 10*um;
    G4double airTube_length = fLength + 10*um;

    // prepare specific targets
    switch (fType)
    {
        case kStandard:
            G4cout << "A2SolidTargetGeneric::Construct(): Constructing generic solid target" << G4endl;
            break;
        case kOct_18:
            G4cout << "A2SolidTargetGeneric::Construct(): Constructing solid target for October 2018 experiment" << G4endl;
            airTube_z0 = -25*cm;
            airTube_radius = 25*mm + 10*um;
            airTube_length = 100*cm + 10*um;
            break;
    }

    // user info
    G4cout << "A2SolidTargetGeneric::Construct(): Target length   : " << fLength << " mm" << G4endl;
    G4cout << "A2SolidTargetGeneric::Construct(): Target radius   : " << fRadius << " mm" << G4endl;
    G4cout << "A2SolidTargetGeneric::Construct(): Target material : " << fMaterial->GetName() << G4endl;

    // set the mother volume
    fMotherLogic = motherLogic;

    // check target material
    if (!fMaterial)
    {
        G4cerr << "A2SolidTargetGeneric::Construct(): Target material not defined!" << G4endl;
        exit(1);
    }

    //
    // generic volumes
    //

    // construct the air tube
    G4Tubs* airTube = new G4Tubs("TRGT", 0, airTube_radius, airTube_length / 2., 0*deg, 360*deg);
    fMyLogic = new G4LogicalVolume(airTube, fNistManager->FindOrBuildMaterial("G4_AIR"), "TRGT");
    fMyLogic->SetVisAttributes(G4VisAttributes::Invisible);
    fMyPhysi = new G4PVPlacement(0, G4ThreeVector(0.0*cm, 0.0*cm, airTube_z0), fMyLogic, "TRGT",
                                 fMotherLogic, false, 0);
    A2Utils::CheckOverlapAndAbort(fMyPhysi, "A2SolidTargetGeneric::Construct()");

    // target 'cell'
    G4Tubs* cell = new G4Tubs("CELL", 0, fRadius, fLength/2., 0*deg, 360*deg);
    G4LogicalVolume* cell_log = new G4LogicalVolume(cell, fMaterial, "CELL");
    if (fType == kOct_18)
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, -airTube_z0), cell_log, "CELL", fMyLogic, false, 0);
    else
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), cell_log, "CELL", fMyLogic, false, 0);
    G4VisAttributes* vis_att = new G4VisAttributes(G4Colour(1, 0, 0));
    cell_log->SetVisAttributes(vis_att);
    A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

    //
    // specific volumes
    //

    if (fType == kOct_18)
    {
        // construct the carbon tube
        G4Tubs* vaccTube = new G4Tubs("VACC_TUBE", 24*mm, 25*mm, 50*cm, 0*deg, 360*deg);
        G4LogicalVolume* vaccTube_log = new G4LogicalVolume(vaccTube, fNistManager->FindOrBuildMaterial("G4_C"), "VACC_TUBE");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), vaccTube_log, "VACC_TUBE", fMyLogic, false, 0);
        vis_att = new G4VisAttributes(G4Colour(0.2, 0.2, 0.2));
        vaccTube_log->SetVisAttributes(vis_att);
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");
    }

    return fMyPhysi;
}

