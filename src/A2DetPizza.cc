// Pizza detector
// Author: Dominik Werthmueller, 2016

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VisAttributes.hh"
#include "G4SDManager.hh"
#include "G4TessellatedSolid.hh"
#include "CLHEP/Units/SystemOfUnits.h"

#include "TString.h"

#include "A2DetPizza.hh"
#include "A2SD.hh"
#include "A2VisSD.hh"
#include "A2Utils.hh"

using namespace CLHEP;

// init static class members
const G4double A2DetPizza::fgDefaultZPos = 162*cm;

//______________________________________________________________________________
A2DetPizza::A2DetPizza(G4double zpos)
{
    // Constructor.

    // init members
    fZPos = zpos;
    fIsCheckOverlap = true;
    fRegionPizza = new G4Region("Pizza");
    fPizzaSD = 0;
    fPizzaVisSD = 0;
}

//______________________________________________________________________________
A2DetPizza::~A2DetPizza()
{
    // Destructor.

    if (fRegionPizza) delete fRegionPizza;
    if (fPizzaSD) delete fPizzaSD;
    if (fPizzaVisSD) delete fPizzaVisSD;
}

//______________________________________________________________________________
G4VPhysicalVolume* A2DetPizza::Construct(G4LogicalVolume* motherLogic)
{
    // Construct the detector.

    // global geometry data
    const G4int nPizza = 24;
    G4double phi0 = 180.*deg;
    G4double dphi = 360.*deg / nPizza;
    const G4double scint_thick = 10*mm;
    const G4double shift_sc_center_airbox = 10*mm;

    // set the mother volume
    fMotherLogic = motherLogic;

    // construct the air box
    G4Box* airBox = new G4Box("pizza_box", 130.0*cm, 130.0*cm, 45*mm);
    fMyLogic = new G4LogicalVolume(airBox, fNistManager->FindOrBuildMaterial("G4_AIR"), "pizza_box");
    fMyLogic->SetVisAttributes(G4VisAttributes::Invisible);
    fMyPhysi = new G4PVPlacement(0, G4ThreeVector(0.0*cm, 0.0*cm, fZPos + 0.5*scint_thick + shift_sc_center_airbox),
                                                  fMyLogic, "pizza_box",
                                                  fMotherLogic, false, 0);
    if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(fMyPhysi, "A2DetPizza::Construct()");

    //
    // build the scintillator (EJ-200)
    //

    // geometry data (anti-clockwise ordering of surface points)
    const G4int scint_n = 4;
    const G4double scint_x[scint_n] = { 37.734*mm, 727.947*mm, 727.947*mm, 37.734*mm };
    const G4double scint_y[scint_n] = { -4.584*mm, -95.601*mm, 95.601*mm, 4.584*mm };

    // create solid and logical volume
    G4TessellatedSolid* scint = A2Utils::BuildPlanarTessSolid(scint_n, scint_x, scint_y,
                                                              scint_thick, "pizza_scint");
    G4LogicalVolume* scint_log = new G4LogicalVolume(scint,
                                                     fNistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE"),
                                                     "pizza_scint");
    scint_log->SetVisAttributes(G4Colour(1, 1, 0));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateZ(phi0 - i*dphi);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(0.0*cm, 0.0*cm, -shift_sc_center_airbox),
                                                 scint_log, TString::Format("pizza_scint_%d", i+1).Data(),
                                                 fMyLogic, false, i+1);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    // set-up sensitive detectors
    G4SDManager* sdMan = G4SDManager::GetSDMpointer();
    if (fIsInteractive)
    {
        if (!fPizzaVisSD) fPizzaVisSD = new A2VisSD("PizzaVisSD", nPizza);
        sdMan->AddNewDetector(fPizzaVisSD);
        scint_log->SetSensitiveDetector(fPizzaVisSD);
        fRegionPizza->AddRootLogicalVolume(scint_log);
    }
    else
    {
        if (!fPizzaSD) fPizzaSD = new A2SD("PizzaSD", nPizza);
        sdMan->AddNewDetector(fPizzaSD);
        scint_log->SetSensitiveDetector(fPizzaSD);
        fRegionPizza->AddRootLogicalVolume(scint_log);
    }

    //
    // build the lightguide
    //

    // geometry data (anti-clockwise ordering of surface points)
    const G4int light_guide_n = 10;
    const G4double light_guide_x[light_guide_n] = { 727.947*mm, 837.948*mm, 848.540*mm, 912.113*mm, 940.146*mm,
                                                    940.146*mm, 912.113*mm, 848.540*mm, 837.948*mm, 727.947*mm };
    const G4double light_guide_y[light_guide_n] = { -95.601*mm, -110.201*mm, -110.201*mm, -21.500*mm, -12.900*mm,
                                                    12.900*mm, 21.500*mm, 110.201*mm, 110.201*mm, 95.601*mm };
    const G4double light_guide_thick = 30*mm;

    // create solid and logical volume
    G4TessellatedSolid* light_guide = A2Utils::BuildPlanarTessSolid(light_guide_n, light_guide_x, light_guide_y,
                                                                    light_guide_thick, "pizza_light_guide");
    G4LogicalVolume* light_guide_log = new G4LogicalVolume(light_guide,
                                                           fNistManager->FindOrBuildMaterial("A2_ACRYLIC"),
                                                           "pizza_light_guide");
    light_guide_log->SetVisAttributes(G4Colour(0, 1, 0));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateZ(phi0 - i*dphi);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(0.0*cm, 0.0*cm, -shift_sc_center_airbox),
                                                 light_guide_log, TString::Format("pizza_light_guide_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the metal shoes
    //

    // geometry data (anti-clockwise ordering of surface points)
    const G4int shoe_n = 8;
    const G4double shoe_x[shoe_n] = { 728.021*mm, 837.948*mm, 848.540*mm, 918.330*mm,
                                      918.330*mm, 848.540*mm, 837.948*mm, 728.021*mm };
    const G4double shoe_y[shoe_n] = { -95.601*mm, -110.201*mm, -110.201*mm, -109.915*mm,
                                      109.915*mm, 110.201*mm, 110.201*mm, 95.601*mm };
    const G4double shoe_thick = 20*mm;
    const G4double shoe_z = 0.5*light_guide_thick + 0.5*shoe_thick;

    // create solid and logical volume
    G4TessellatedSolid* shoe = A2Utils::BuildPlanarTessSolid(shoe_n, shoe_x, shoe_y,
                                                             shoe_thick, "pizza_shoe");
    G4LogicalVolume* shoe_log = new G4LogicalVolume(shoe,
                                                    fNistManager->FindOrBuildMaterial("G4_Al"),
                                                    "pizza_shoe");
    shoe_log->SetVisAttributes(G4Colour(0, 0, 1));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateZ(phi0 - i*dphi);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(0.0*cm, 0.0*cm, shoe_z - shift_sc_center_airbox),
                                                 shoe_log, TString::Format("pizza_shoe_back_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
        v = new G4PVPlacement(rot, G4ThreeVector(0.0*cm, 0.0*cm, -shoe_z - shift_sc_center_airbox),
                              shoe_log, TString::Format("pizza_shoe_%d", i+1).Data(),
                              fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the metal shoes edge pieces
    //

    // geometry data (anti-clockwise ordering of surface points)
    const G4int shoe_e_n = 4;
    const G4double shoe_e_x[shoe_n] = {  848.540*mm,  918.330*mm, 918.330*mm, 912.113*mm };
    const G4double shoe_e_y[shoe_n] = { -110.201*mm, -109.915*mm, -21.500*mm, -21.500*mm };
    const G4double shoe_e_thick = 30*mm;
    const G4double shoe_e_z = 0;

    // create solid and logical volume
    G4TessellatedSolid* shoe_e = A2Utils::BuildPlanarTessSolid(shoe_e_n, shoe_e_x, shoe_e_y,
                                                               shoe_e_thick, "pizza_shoe_edge");
    G4LogicalVolume* shoe_e_log = new G4LogicalVolume(shoe_e,
                                                      fNistManager->FindOrBuildMaterial("G4_Al"),
                                                      "pizza_shoe_edge");
    shoe_e_log->SetVisAttributes(G4Colour(0.3, 0, 0.7));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot_1 = new G4RotationMatrix();
        rot_1->rotateZ(phi0 - i*dphi);
        G4VPhysicalVolume* v = new G4PVPlacement(rot_1, G4ThreeVector(0.0*cm, 0.0*cm, shoe_e_z - shift_sc_center_airbox),
                                                 shoe_e_log, TString::Format("pizza_shoe_edge_1_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
        G4RotationMatrix* rot_2 = new G4RotationMatrix();
        rot_2->rotateZ(phi0 - i*dphi);
        rot_2->rotateX(180*deg);
        v = new G4PVPlacement(rot_2, G4ThreeVector(0.0*cm, 0.0*cm, shoe_e_z - shift_sc_center_airbox),
                              shoe_e_log, TString::Format("pizza_shoe_edge_2_%d", i+1).Data(),
                              fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the PMs
    //

    // geometry data
    G4double innerRadius = 15.5*mm; // ?
    G4double outerRadius = 17.5*mm;
    G4double hz = 0.5*187*mm;
    G4double startAngle = 0*deg;
    G4double spanningAngle = 360*deg;
    G4double light_guide_pos = 940.146*mm;
    G4double center_shift = light_guide_pos+hz;

    // create solid and logical volume
    G4Tubs* pm = new G4Tubs("pizza_pm", innerRadius, outerRadius, hz, startAngle, spanningAngle);
    G4LogicalVolume* pm_log = new G4LogicalVolume(pm,
                                  fNistManager->FindOrBuildMaterial("A2_MUMETAL"),
                                  "pizza_pm");
    pm_log->SetVisAttributes(G4Colour(0, 1, 1));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        G4double rot_z = phi0 - i*dphi;
        rot->rotateZ(rot_z);
        rot->rotateY(90*deg);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(center_shift*cos(rot_z), -center_shift*sin(rot_z), -shift_sc_center_airbox),
                                                 pm_log, TString::Format("pizza_pm_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the PM protection
    //

    // geometry data
    innerRadius = 23*mm; // ?
    outerRadius = 24*mm;
    hz = 0.5*298*mm;
    startAngle = 0*deg;
    spanningAngle = 360*deg;
    const G4double shoe_pos = 918.330*mm + 6.8*mm; // prevents overlap
    center_shift = shoe_pos+hz;

    // create solid and logical volume
    G4Tubs* pm_prot = new G4Tubs("pizza_pm_prot", innerRadius, outerRadius, hz, startAngle, spanningAngle);
    G4LogicalVolume* pm_prot_log = new G4LogicalVolume(pm_prot,
                                                       fNistManager->FindOrBuildMaterial("A2_MUMETAL"),
                                                       "pizza_pm_prot");
    pm_prot_log->SetVisAttributes(G4Colour(1, 0, 1));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        G4double rot_z = phi0 - i*dphi;
        rot->rotateZ(rot_z);
        rot->rotateY(90*deg);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(center_shift*cos(rot_z), -center_shift*sin(rot_z), -shift_sc_center_airbox),
                                                 pm_prot_log, TString::Format("pizza_pm_prot_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the PM protection flanges
    //

    // geometry data
    innerRadius = 24*mm;
    outerRadius = 32*mm;
    hz = 6*mm;
    startAngle = 0*deg;
    spanningAngle = 360*deg;
    center_shift = 918.330*mm + hz + 2.7*mm; // prevents overlap

    // create solid and logical volume
    G4Tubs* pm_prot_fl = new G4Tubs("pizza_pm_prot_fl", innerRadius, outerRadius, hz, startAngle, spanningAngle);
    G4LogicalVolume* pm_prot_fl_log = new G4LogicalVolume(pm_prot_fl,
                                                         fNistManager->FindOrBuildMaterial("A2_BRASS"),
                                                         "pizza_pm_prot_fl");
    pm_prot_fl_log->SetVisAttributes(G4Colour(0.5, 0.5, 0));

    // create the physical volumes
    for (G4int i = 0; i < nPizza; i++)
    {
        G4RotationMatrix* rot = new G4RotationMatrix();
        G4double rot_z = phi0 - i*dphi;
        rot->rotateZ(rot_z);
        rot->rotateY(90*deg);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(center_shift*cos(rot_z), -center_shift*sin(rot_z), -shift_sc_center_airbox),
                                                 pm_prot_fl_log, TString::Format("pizza_pm_prot_fl_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    //
    // build the stabilizing steel plates
    //

    // geometry data
    const G4double st_pl_long = 50*cm;
    const G4double st_pl_short = 42*cm;
    const G4double st_pl_height = 15*cm;
    const G4double st_pl_thick = 1*cm;
    const G4double st_pl_z_1 = 0.5*light_guide_thick + shoe_thick + 0.5*st_pl_thick - shift_sc_center_airbox;
    const G4double st_pl_z_2 = 0.5*light_guide_thick + shoe_thick + 1.5*st_pl_thick - shift_sc_center_airbox;
    center_shift = 85.9*cm;

    // create solid and logical volume
    G4Trd* st_pl = new G4Trd("pizza_st_pl", 0.5*st_pl_long, 0.5*st_pl_short,
                             0.5*st_pl_thick, 0.5*st_pl_thick, 0.5*st_pl_height);
    G4LogicalVolume* st_pl_log = new G4LogicalVolume(st_pl,
                                                     fNistManager->FindOrBuildMaterial("A2_SS"),
                                                     "pizza_st_pl");
    st_pl_log->SetVisAttributes(G4Colour(1, 0, 0));

    // create the physical volumes
    for (G4int i = 0; i < 12; i++)
    {
        // first layer
        G4RotationMatrix* rot = new G4RotationMatrix();
        G4double rot_z = 187*deg - i*dphi*2;
        rot->rotateZ(rot_z + 270*deg);
        rot->rotateX(90*deg);
        G4VPhysicalVolume* v = new G4PVPlacement(rot, G4ThreeVector(center_shift*cos(rot_z), -center_shift*sin(rot_z), st_pl_z_1),
                                                 st_pl_log, TString::Format("pizza_st_pl_1_%d", i+1).Data(),
                                                 fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");

        // second layer
        rot = new G4RotationMatrix();
        rot_z = 175*deg - i*dphi*2;
        rot->rotateZ(rot_z + 270*deg);
        rot->rotateX(90*deg);
        v = new G4PVPlacement(rot, G4ThreeVector(center_shift*cos(rot_z), -center_shift*sin(rot_z), st_pl_z_2),
                              st_pl_log, TString::Format("pizza_st_pl_2_%d", i+1).Data(),
                              fMyLogic, false, i);
        if (fIsCheckOverlap) A2Utils::CheckOverlapAndAbort(v, "A2DetPizza::Construct()");
    }

    return fMyPhysi;
}

