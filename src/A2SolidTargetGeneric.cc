// Generic solid target supporting different types used
// in past experiments.
// Author: Dominik Werthmueller, 2018-2019

#include "G4Tubs.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4GenericTrap.hh"

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
    G4double z_diff = 0;
    G4double kapton_thick = 0;

    // set target center
    fCenter.set(0, 0, z0);

    // prepare specific targets
    switch (fType)
    {
        case kStandard:
            G4cout << "A2SolidTargetGeneric::Construct(): Constructing generic solid target" << G4endl;
            break;
        case kOct_18:
            G4cout << "A2SolidTargetGeneric::Construct(): Constructing solid target for October 2018 experiment" << G4endl;
            kapton_thick = 122.5*um;   // Sascha measurement: 120-125 um
            G4double target_window_z = 410.5*mm + 3*mm + kapton_thick; // end of pipe + window ring thickness + foil
            airTube_length = 200*cm;
            z_diff = airTube_length/2. - target_window_z;
            airTube_z0 = -z_diff;
            airTube_radius = 28*mm;
            break;
    }

    // user info
    G4cout << "A2SolidTargetGeneric::Construct(): Target length   : " << fLength << " mm" << G4endl;
    G4cout << "A2SolidTargetGeneric::Construct(): Target radius   : " << fRadius << " mm" << G4endl;
    G4cout << "A2SolidTargetGeneric::Construct(): Target center   : " << fCenter.z() << " mm" << G4endl;
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
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - airTube_z0), cell_log, "CELL", fMyLogic, false, 0);
    else
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), cell_log, "CELL", fMyLogic, false, 0);
    cell_log->SetVisAttributes(G4Colour(1, 0, 0));
    A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

    //
    // specific volumes
    //

    if (fType == kOct_18)
    {
        // construct the carbon tube
        G4Tubs* vacTube = new G4Tubs("VAC_TUBE", 24*mm+0.1*mm, 25*mm+0.1*mm, airTube_length/2. - 1.5*mm - kapton_thick/2., 0*deg, 360*deg);
        G4LogicalVolume* vacTube_log = new G4LogicalVolume(vacTube, fNistManager->FindOrBuildMaterial("A2_CFRP"), "VAC_TUBE");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, -1.5*mm - kapton_thick), vacTube_log, "VAC_TUBE", fMyLogic, false, 0);
        vacTube_log->SetVisAttributes(G4Colour(0.2, 0.2, 0.2, 0.3));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        //
        // construct the target window
        //

        G4Tubs* winTube1 = new G4Tubs("WIN_TUBE_1", 22*mm, 27*mm, 1.5*mm, 0*deg, 360*deg);
        G4LogicalVolume* winTube1_log = new G4LogicalVolume(winTube1, fNistManager->FindOrBuildMaterial("G4_Al"), "WIN_TUBE_1");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, airTube_length/2. - 1.5*mm - kapton_thick), winTube1_log, "WIN_TUBE_1", fMyLogic, false, 0);
        winTube1_log->SetVisAttributes(G4Colour(0.94, 0.94, 0.94));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        G4Tubs* winTube2 = new G4Tubs("WIN_TUBE_2", 22*mm, 24*mm, 3.5*mm, 0*deg, 360*deg);
        G4LogicalVolume* winTube2_log = new G4LogicalVolume(winTube2, fNistManager->FindOrBuildMaterial("G4_Al"), "WIN_TUBE_2");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, airTube_length/2. - 6.5*mm - kapton_thick), winTube2_log, "WIN_TUBE_2", fMyLogic, false, 0);
        winTube2_log->SetVisAttributes(G4Colour(0.94, 0.94, 0.94));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        G4Tubs* winFoil = new G4Tubs("WIN_FOIL", 0, 28*mm, kapton_thick/2., 0*deg, 360*deg);
        G4LogicalVolume* winFoil_log = new G4LogicalVolume(winFoil, fNistManager->FindOrBuildMaterial("G4_KAPTON"), "WIN_FOIL");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, airTube_length/2. - kapton_thick/2), winFoil_log, "WIN_FOIL", fMyLogic, false, 0);
        winFoil_log->SetVisAttributes(G4Colour(1, 0.356, 0.023, 0.3));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        //
        // construct the target holder
        //

        // create main solid (G4Tubs)
        //G4Tubs* t_holder = new G4Tubs("TRGT_HOLDER_BASE", 0, 24*mm, 10*mm, 0*deg, 360*deg);

        //// create main solid (G4TessellatedSolid)
        //const G4int t_holder_n = 6;
        //const G4double t_holder_x[t_holder_n] = {  3.56*mm,  22.42*mm,  18.86*mm, -18.68*mm, -22.42*mm, -3.56*mm};
        //const G4double t_holder_y[t_holder_n] = { 23.84*mm,  -8.84*mm, -15.00*mm, -15.00*mm,  -8.84*mm, 23.84*mm};
        //const G4double t_holder_thick = 20*mm;
        //G4TessellatedSolid* t_holder = A2Utils::BuildPlanarTessSolid(t_holder_n, t_holder_x, t_holder_y,
        //                                                             t_holder_thick, "TRGT_HOLDER_BASE");

        // create main solid (G4GenericTrap)
        std::vector<G4TwoVector> vertices;
        vertices.push_back(G4TwoVector(-20.784610, -12.000000));
        vertices.push_back(G4TwoVector(0.000000, 24.000000));
        vertices.push_back(G4TwoVector(20.784610, -12.000000));
        vertices.push_back(G4TwoVector(20.784610, -12.000000));
        vertices.push_back(G4TwoVector(-20.784610, -12.000000));
        vertices.push_back(G4TwoVector(0.000000, 24.000000));
        vertices.push_back(G4TwoVector(20.784610, -12.000000));
        vertices.push_back(G4TwoVector(20.784610, -12.000000));
        const G4double t_holder_thick = 20*mm;
        G4GenericTrap* t_holder = new G4GenericTrap("TRGT_HOLDER_BASE", t_holder_thick/2., vertices);

        // backward hole
        G4Tubs* subtr_hole_back = new G4Tubs("SUBTR_HOLE_BACK", 0, 2.5*mm, 7.5*mm/2., 0*deg, 360*deg);

        // forward hole
        G4Tubs* subtr_hole_forward = new G4Tubs("SUBTR_HOLE_FORWARD", 0, 5.2*mm, 12.5*mm/2., 0*deg, 360*deg);

        // create total holder solid
        G4VSolid* subtract_1 = new G4SubtractionSolid("TRGT_HOLDER_SUBTRACT_1", t_holder, subtr_hole_back,
                                                      0, G4ThreeVector(0, 0, -6.25*mm));
        G4VSolid* subtract_2 = new G4SubtractionSolid("TRGT_HOLDER_SUBTRACT_2", subtract_1, subtr_hole_forward,
                                                      0, G4ThreeVector(0, 0, 3.75*mm));
        G4LogicalVolume* t_holder_log = new G4LogicalVolume(subtract_2,
                                                            fNistManager->FindOrBuildMaterial("A2_XPS"),
                                                            "TRGT_HOLDER");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - airTube_z0), t_holder_log, "TRGT_HOLDER", fMyLogic, false, 0);
        t_holder_log->SetVisAttributes(G4Colour(1, 1, 0));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        //
        // construct the target retainer
        //

        G4Tubs* retainTube1 = new G4Tubs("RETAINER_TUBE_1", 4.1*mm, 7.2*mm, 0.85*mm, 0*deg, 360*deg);
        G4LogicalVolume* retainTube1_log = new G4LogicalVolume(retainTube1, fNistManager->FindOrBuildMaterial("A2_POLYPROPYLEN"), "RETAINER_TUBE_1");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - airTube_z0 + 1*cm + 0.85*mm), retainTube1_log, "RETAINER_TUBE_1", fMyLogic, false, 0);
        retainTube1_log->SetVisAttributes(G4Colour(0, 1, 0));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");

        G4Tubs* retainTube2 = new G4Tubs("RETAINER_TUBE_2", 4.1*mm, 5.2*mm, 3.5*mm, 0*deg, 360*deg);
        G4LogicalVolume* retainTube2_log = new G4LogicalVolume(retainTube2, fNistManager->FindOrBuildMaterial("A2_POLYPROPYLEN"), "RETAINER_TUBE_2");
        v = new G4PVPlacement(0, G4ThreeVector(0, 0, z0 - airTube_z0 + 1*cm - 3.5*mm), retainTube2_log, "RETAINER_TUBE_2", fMyLogic, false, 0);
        retainTube2_log->SetVisAttributes(G4Colour(0, 1, 0));
        A2Utils::CheckOverlapAndAbort(v, "A2SolidTargetGeneric::Construct()");
    }

    return fMyPhysi;
}

