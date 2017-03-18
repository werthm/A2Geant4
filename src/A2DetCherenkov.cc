#include "A2DetCherenkov.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Trap.hh"
#include "G4IntersectionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"
#include "G4SubtractionSolid.hh"
#include "G4GenericTrap.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"
#include "G4EllipticalTube.hh"
#include "G4Tubs.hh"
#include "TMath.h"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace CLHEP;


A2DetCherenkov::A2DetCherenkov()
{
    fIsCheckOverlap=kFALSE;
}
A2DetCherenkov::~A2DetCherenkov()
{

}


//______________________________________________________________________________
void A2DetCherenkov::CheckOverlapAndAbort(G4VPhysicalVolume* vol, const G4String& location)
{
    // Check if the volume 'vol' has overlaps and abort if that is the case.
    // Use 'location' in the error message to indicate the origin of the
    // problem.

    // check for overlaps
    if (vol->CheckOverlaps())
    {
        G4cout << location << ": Overlap in volume " << vol->GetName() << " detected!" << G4endl;
        exit(1);
    }
}


G4VPhysicalVolume* A2DetCherenkov::Construct(G4LogicalVolume* MotherLogical){
    fMotherLogic=MotherLogical;

    //make the Cherenkov detector
    MakeCherenkov();

    //check for overlaps
    if (fIsCheckOverlap){
        CheckOverlapAndAbort(fAirBoxPhysi, "A2DetCherenkov::Construct()");
        CheckOverlapAndAbort(fnoseupPhysi, "A2DetCherenkov::Construct()");
        CheckOverlapAndAbort(fnosedownPhysi, "A2DetCherenkov::Construct()");
    }

    return fMyPhysi;
}

void A2DetCherenkov::MakeCherenkov(){
    //Colours with thier corresponding materials used in the visualization:
    G4VisAttributes* GrayVisAtt= new G4VisAttributes(G4Colour(0.8,0.8,0.8)); //frame for nose(gry)
    G4VisAttributes* BrownVisAtt= new G4VisAttributes(G4Colour(0.8,0.6,0.2)); //(brown)
    G4VisAttributes* CyanVisAtt= new G4VisAttributes(G4Colour(0.0, 1.0, 1.0)); // Mylar foil window
    G4VisAttributes* MagentaVisAtt= new G4VisAttributes(G4Colour(1.0,0.0,1.0)); //Tedlar foil
    G4VisAttributes* BlueVisAtt= new G4VisAttributes(G4Colour(0.0,0.0,1.0)); //PVC nose
    G4VisAttributes* GreenVisAtt= new G4VisAttributes(G4Colour(0.0,1.0,0.0)); //Cherenkov gas
    G4VisAttributes* RedVisAtt= new G4VisAttributes(G4Colour(1.0, 0.0, 0.0)); //Al box
    G4VisAttributes* WhiteVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0)); //

// Al frame of Cherenkov detector, consisting of large box, triangular shaped trapezoid at top right corner in xz-plane
    G4Box* airbox=new G4Box("airbox",72.5*cm,60.0*cm,36.0*cm);// the main box part of Cherenkov will be placed this box of air

    fAirBoxLogic=new G4LogicalVolume(airbox,fNistManager->FindOrBuildMaterial("G4_AIR"),"airbox");
    fAirBoxPhysi=new G4PVPlacement(0, G4ThreeVector(-3.0*cm,0.0*cm,133.*cm), fAirBoxLogic, "airbox", fMotherLogic, false, 0);
//    fAirBoxLogic->SetVisAttributes(RedVisAtt);
    fAirBoxLogic->SetVisAttributes(G4VisAttributes::Invisible);


    G4Box* centralbox=new G4Box("centralbox",60.0*cm,60.0*cm,35.9*cm);//box does not contain the Al backplate of 2mm thickness

    fAlBoxLogic=new G4LogicalVolume(centralbox,fNistManager->FindOrBuildMaterial("G4_Al"),"centralbox");
    fAlBoxPhysi=new G4PVPlacement(0, G4ThreeVector(-12.5*cm,0.0*cm,-0.1*cm), fAlBoxLogic, "centralbox", fAirBoxLogic, false, 0);
    fAlBoxLogic->SetVisAttributes(RedVisAtt);
    //fAlBoxLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //the back plate made of Al
    G4Box* backAlbox=new G4Box("backAlbox",72.5*cm,60.0*cm,0.1*cm);//Al backplate of 2mm thickness in z direction

    fbackAlBoxLogic=new G4LogicalVolume(backAlbox,fNistManager->FindOrBuildMaterial("G4_Al"),"backAlbox");
    fbackAlBoxPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,35.9*cm), fbackAlBoxLogic, "backAlbox", fAirBoxLogic, false, 0);
    fbackAlBoxLogic->SetVisAttributes(RedVisAtt);
    //fbackAlBoxLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //the inner volume of the box which is filled with Cherenkov gas
    G4Box* centralinnerbox=new G4Box("centralinnerbox",59.8*cm,59.8*cm,35.6*cm);//thickness of Al frame is 6mm (front plate) or 2mm (rest)

    finnerboxvolumeLogic=new G4LogicalVolume(centralinnerbox,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"centralinnerbox");
    finnerboxvolumePhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.3*cm), finnerboxvolumeLogic, "centralinnerbox", fAlBoxLogic, false, 0);
    finnerboxvolumeLogic->SetVisAttributes(GreenVisAtt);
    //finnerboxvolumeLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //need a small box to cut the Al frame on the left side (view from target) where the triangular shape is located
    G4double zhalflength_innertri= (45.*cm - 0.2*cm - 0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.;
    G4Box* smallbox=new G4Box("smallbox",0.1*cm,59.8*cm,zhalflength_innertri);

    fsmallboxLogic=new G4LogicalVolume(smallbox,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"smallbox");
    fsmallboxPhysi=new G4PVPlacement(0, G4ThreeVector(59.9*cm,0.0*cm,13.6*cm), fsmallboxLogic, "smallbox", fAlBoxLogic, false, 0);
    fsmallboxLogic->SetVisAttributes(GreenVisAtt);
    //fsmallboxLogic->SetVisAttributes(G4VisAttributes::Invisible);

//the steel frame

    G4Box* steelframebox1full=new G4Box("steelframebox1full",59.8*cm,59.8*cm,0.3*cm);
    G4Box* steelframebox1tocut=new G4Box("steelframebox1tocut",53.8*cm,53.8*cm,0.31*cm);
    G4SubtractionSolid* steelframebox1 = new G4SubtractionSolid("steelframebox1", steelframebox1full, steelframebox1tocut, 0, G4ThreeVector(0.*cm,0.*cm,0.*cm));

    steelframebox1Logic=new G4LogicalVolume(steelframebox1,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox1");
    steelframebox1Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-35.3*cm), steelframebox1Logic, "steelframebox1", finnerboxvolumeLogic, false, 0);
    steelframebox1Logic->SetVisAttributes(GrayVisAtt);

    steelframebox1copyLogic=new G4LogicalVolume(steelframebox1,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox1");
    steelframebox1copyPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-14.3*cm), steelframebox1copyLogic, "steelframebox1", finnerboxvolumeLogic, false, 0);
    steelframebox1copyLogic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox2full=new G4Box("steelframebox2full",59.8*cm,59.8*cm,(3-0.3)*cm);
    G4Box* steelframebox2tocut=new G4Box("steelframebox2tocut",59.2*cm,59.2*cm,(3.01-0.3)*cm);
    G4SubtractionSolid* steelframebox2 = new G4SubtractionSolid("steelframebox2", steelframebox2full, steelframebox2tocut, 0, G4ThreeVector(0.*cm,0.*cm,0.*cm));

    steelframebox2Logic=new G4LogicalVolume(steelframebox2,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox2");
    steelframebox2Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-32.3*cm), steelframebox2Logic, "steelframebox2", finnerboxvolumeLogic, false, 0);
    steelframebox2Logic->SetVisAttributes(GrayVisAtt);

    steelframebox2copyLogic=new G4LogicalVolume(steelframebox2,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox2");
    steelframebox2copyPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-11.3*cm), steelframebox2copyLogic, "steelframebox2", finnerboxvolumeLogic, false, 0);
    steelframebox2copyLogic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox3full=new G4Box("steelframebox3full",59.8*cm,59.8*cm,(3-0.3)*cm);
    G4Box* steelframebox3tocut=new G4Box("steelframebox3tocut",59.5*cm,59.2*cm,(3.01-0.3)*cm);
    G4SubtractionSolid* steelframebox3 = new G4SubtractionSolid("steelframebox3", steelframebox3full, steelframebox3tocut, 0, G4ThreeVector(0.3*cm,0.*cm,0.*cm));

    steelframebox3Logic=new G4LogicalVolume(steelframebox3,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox3");
    steelframebox3Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,32.3*cm), steelframebox3Logic, "steelframebox3", finnerboxvolumeLogic, false, 0);
    steelframebox3Logic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox4full=new G4Box("steelframebox4full",59.8*cm,59.8*cm,0.3*cm);
    G4Box* steelframebox4tocut=new G4Box("steelframebox4tocut",56.8*cm,53.8*cm,0.31*cm);
    G4SubtractionSolid* steelframebox4 = new G4SubtractionSolid("steelframebox4", steelframebox4full, steelframebox4tocut, 0, G4ThreeVector(3.*cm,0.*cm,0.*cm));

    steelframebox4Logic=new G4LogicalVolume(steelframebox4,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox4");
    steelframebox4Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,35.3*cm), steelframebox4Logic, "steelframebox4", finnerboxvolumeLogic, false, 0);
    steelframebox4Logic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox5=new G4Box("steelframebox5",3.0*cm,0.3*cm,7.5*cm);

    steelframebox5Logic=new G4LogicalVolume(steelframebox5,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox5");
    steelframebox5Physi=new G4PVPlacement(0, G4ThreeVector(-56.5*cm,59.5*cm,-22.1*cm), steelframebox5Logic, "steelframebox5", finnerboxvolumeLogic, false, 0);
    steelframebox5Logic->SetVisAttributes(GrayVisAtt);

    steelframebox5copy1Logic=new G4LogicalVolume(steelframebox5,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox5");
    steelframebox5copy1Physi=new G4PVPlacement(0, G4ThreeVector(56.5*cm,59.5*cm,-22.1*cm), steelframebox5copy1Logic, "steelframebox5", finnerboxvolumeLogic, false, 0);
    steelframebox5copy1Logic->SetVisAttributes(GrayVisAtt);

    steelframebox5copy2Logic=new G4LogicalVolume(steelframebox5,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox5");
    steelframebox5copy2Physi=new G4PVPlacement(0, G4ThreeVector(-56.5*cm,-59.5*cm,-22.1*cm), steelframebox5copy2Logic, "steelframebox5", finnerboxvolumeLogic, false, 0);
    steelframebox5copy2Logic->SetVisAttributes(GrayVisAtt);

    steelframebox5copy3Logic=new G4LogicalVolume(steelframebox5,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox5");
    steelframebox5copy3Physi=new G4PVPlacement(0, G4ThreeVector(56.5*cm,-59.5*cm,-22.1*cm), steelframebox5copy3Logic, "steelframebox5", finnerboxvolumeLogic, false, 0);
    steelframebox5copy3Logic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox6=new G4Box("steelframebox6",0.3*cm,2.7*cm,7.5*cm);

    steelframebox6Logic=new G4LogicalVolume(steelframebox6,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox6");
    steelframebox6Physi=new G4PVPlacement(0, G4ThreeVector(-59.5*cm,56.5*cm,-22.1*cm), steelframebox6Logic, "steelframebox6", finnerboxvolumeLogic, false, 0);
    steelframebox6Logic->SetVisAttributes(GrayVisAtt);

    steelframebox6copy1Logic=new G4LogicalVolume(steelframebox6,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox6");
    steelframebox6copy1Physi=new G4PVPlacement(0, G4ThreeVector(59.5*cm,56.5*cm,-22.1*cm), steelframebox6copy1Logic, "steelframebox6", finnerboxvolumeLogic, false, 0);
    steelframebox6copy1Logic->SetVisAttributes(GrayVisAtt);

    steelframebox6copy2Logic=new G4LogicalVolume(steelframebox6,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox6");
    steelframebox6copy2Physi=new G4PVPlacement(0, G4ThreeVector(-59.5*cm,-56.5*cm,-22.1*cm), steelframebox6copy2Logic, "steelframebox6", finnerboxvolumeLogic, false, 0);
    steelframebox6copy2Logic->SetVisAttributes(GrayVisAtt);

    steelframebox6copy3Logic=new G4LogicalVolume(steelframebox6,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox6");
    steelframebox6copy3Physi=new G4PVPlacement(0, G4ThreeVector(59.5*cm,-56.5*cm,-22.1*cm), steelframebox6copy3Logic, "steelframebox6", finnerboxvolumeLogic, false, 0);
    steelframebox6copy3Logic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox7=new G4Box("steelframebox7",3.0*cm,0.3*cm,19.1*cm);

    steelframebox7Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7Physi=new G4PVPlacement(0, G4ThreeVector(-56.5*cm,59.5*cm,10.5*cm), steelframebox7Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7Logic->SetVisAttributes(GrayVisAtt);

    steelframebox7copy1Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7copy1Physi=new G4PVPlacement(0, G4ThreeVector(56.5*cm,59.5*cm,10.5*cm), steelframebox7copy1Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7copy1Logic->SetVisAttributes(GrayVisAtt);

    steelframebox7copy2Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7copy2Physi=new G4PVPlacement(0, G4ThreeVector(-56.5*cm,-59.5*cm,10.5*cm), steelframebox7copy2Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7copy2Logic->SetVisAttributes(GrayVisAtt);

    steelframebox7copy3Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7copy3Physi=new G4PVPlacement(0, G4ThreeVector(56.5*cm,-59.5*cm,10.5*cm), steelframebox7copy3Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7copy3Logic->SetVisAttributes(GrayVisAtt);

    steelframebox7copy4Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7copy4Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,59.5*cm,10.5*cm), steelframebox7copy4Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7copy4Logic->SetVisAttributes(GrayVisAtt);

    steelframebox7copy5Logic=new G4LogicalVolume(steelframebox7,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox7");
    steelframebox7copy5Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,-59.5*cm,10.5*cm), steelframebox7copy5Logic, "steelframebox7", finnerboxvolumeLogic, false, 0);
    steelframebox7copy5Logic->SetVisAttributes(GrayVisAtt);

    G4Box* steelframebox8=new G4Box("steelframebox8",0.3*cm,2.7*cm,19.1*cm);

    steelframebox8Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8Physi=new G4PVPlacement(0, G4ThreeVector(-59.5*cm,56.5*cm,10.5*cm), steelframebox8Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8Logic->SetVisAttributes(GrayVisAtt);

    steelframebox8copy1Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8copy1Physi=new G4PVPlacement(0, G4ThreeVector(59.5*cm,56.5*cm,10.5*cm), steelframebox8copy1Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8copy1Logic->SetVisAttributes(GrayVisAtt);

    steelframebox8copy2Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8copy2Physi=new G4PVPlacement(0, G4ThreeVector(-59.5*cm,-56.5*cm,10.5*cm), steelframebox8copy2Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8copy2Logic->SetVisAttributes(GrayVisAtt);

    steelframebox8copy3Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8copy3Physi=new G4PVPlacement(0, G4ThreeVector(59.5*cm,-56.5*cm,10.5*cm), steelframebox8copy3Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8copy3Logic->SetVisAttributes(GrayVisAtt);

    steelframebox8copy4Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8copy4Physi=new G4PVPlacement(0, G4ThreeVector(-2.7*cm,56.5*cm,10.5*cm), steelframebox8copy4Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8copy4Logic->SetVisAttributes(GrayVisAtt);

    steelframebox8copy5Logic=new G4LogicalVolume(steelframebox8,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox8");
    steelframebox8copy5Physi=new G4PVPlacement(0, G4ThreeVector(-2.7*cm,-56.5*cm,10.5*cm), steelframebox8copy5Logic, "steelframebox8", finnerboxvolumeLogic, false, 0);
    steelframebox8copy5Logic->SetVisAttributes(GrayVisAtt);


//now the triangular shaped trapezoid which is located on the left side
    std::vector<G4TwoVector> vertices_tri;

    vertices_tri.push_back(G4TwoVector(-10.5*cm,-60.0*cm));
    vertices_tri.push_back(G4TwoVector(-10.5*cm,-60.0*cm));
    vertices_tri.push_back(G4TwoVector(-10.5*cm,60.0*cm));
    vertices_tri.push_back(G4TwoVector(-10.5*cm,60.0*cm));
    vertices_tri.push_back(G4TwoVector(10.5*cm,-60.0*cm));
    vertices_tri.push_back(G4TwoVector(-10.5*cm,-60.0*cm));
    vertices_tri.push_back(G4TwoVector(-10.5*cm,60.0*cm));
    vertices_tri.push_back(G4TwoVector(10.5*cm,60.0*cm));

    G4GenericTrap* tri= new G4GenericTrap("tri",22.4*cm,vertices_tri);

    ftriAlLogic=new G4LogicalVolume(tri,fNistManager->FindOrBuildMaterial("G4_Al"),"tri");
    ftriAlPhysi=new G4PVPlacement(0, G4ThreeVector(58.*cm,0.0*cm,13.4*cm), ftriAlLogic, "tri", fAirBoxLogic, false, 0);
    ftriAlLogic->SetVisAttributes(RedVisAtt);
    //ftriAlLogic->SetVisAttributes(G4VisAttributes::Invisible);

    G4RotationMatrix* rm = new G4RotationMatrix();
    rm->rotateZ(0.*deg);

    //subtract the inner volume of the triangle to get the 2mm and 6mm thick Al frame of the triangle
    std::vector<G4TwoVector> vertices_innertri;

    vertices_innertri.push_back(G4TwoVector(-10.5*cm,-59.8*cm));
    vertices_innertri.push_back(G4TwoVector(-10.5*cm,-59.8*cm));
    vertices_innertri.push_back(G4TwoVector(-10.5*cm,59.8*cm));
    vertices_innertri.push_back(G4TwoVector(-10.5*cm,59.8*cm));
    vertices_innertri.push_back(G4TwoVector(10.279326*cm,-59.8*cm));
    vertices_innertri.push_back(G4TwoVector(-10.5*cm,-59.8*cm));
    vertices_innertri.push_back(G4TwoVector(-10.5*cm,59.8*cm));
    vertices_innertri.push_back(G4TwoVector(10.279326*cm,59.8*cm));

    G4GenericTrap* innertri= new G4GenericTrap("innertri",(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.),vertices_innertri);

    finnertriLogic=new G4LogicalVolume(innertri,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"innertri");
    finnertriPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), finnertriLogic, "innertri", ftriAlLogic, false, 0);
    finnertriLogic->SetVisAttributes(GreenVisAtt);
   //finnertriLogic->SetVisAttributes(G4VisAttributes::Invisible);

    G4Box* steelframebox9=new G4Box("steelframebox9",6.9793*cm,2.7*cm,0.3*cm);

    steelframebox9Logic=new G4LogicalVolume(steelframebox9,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox9");
    steelframebox9Physi=new G4PVPlacement(0, G4ThreeVector(-3.0*cm,56.5*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-0.3*cm), steelframebox9Logic, "steelframebox9", finnertriLogic, false, 0);
    steelframebox9Logic->SetVisAttributes(GrayVisAtt);

    steelframebox9copyLogic=new G4LogicalVolume(steelframebox9,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframebox9");
    steelframebox9copyPhysi=new G4PVPlacement(0, G4ThreeVector(-3.0*cm,-56.5*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-0.3*cm), steelframebox9copyLogic, "steelframebox9", finnertriLogic, false, 0);
    steelframebox9copyLogic->SetVisAttributes(GrayVisAtt);

    std::vector<G4TwoVector> vertices_steelframe_trap;

    vertices_steelframe_trap.push_back(G4TwoVector(7.4815*cm,59.8*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(7.4815*cm,59.2*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(-10.279326*cm,59.2*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(-10.279326*cm,59.8*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(10.279326*cm,59.8*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(10.279326*cm,59.2*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(-10.279326*cm,59.2*cm));
    vertices_steelframe_trap.push_back(G4TwoVector(-10.279326*cm,59.8*cm));

    G4GenericTrap* steelframe_trap= new G4GenericTrap("steelframe_trap",3.0*cm,vertices_steelframe_trap);

    fsteelframe_trapLogic=new G4LogicalVolume(steelframe_trap,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframe_trap");
    fsteelframe_trapPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-3.0*cm), fsteelframe_trapLogic, "steelframe_trap", finnertriLogic, false, 0);
    fsteelframe_trapLogic->SetVisAttributes(GrayVisAtt);

    std::vector<G4TwoVector> vertices_steelframe_trap2;

    vertices_steelframe_trap2.push_back(G4TwoVector(7.4815*cm,-59.8*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(-10.279326*cm,-59.8*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(-10.279326*cm,-59.2*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(7.4815*cm,-59.2*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(10.279326*cm,-59.8*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(-10.279326*cm,-59.8*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(-10.279326*cm,-59.2*cm));
    vertices_steelframe_trap2.push_back(G4TwoVector(10.279326*cm,-59.2*cm));

    G4GenericTrap* steelframe_trap2= new G4GenericTrap("steelframe_trap2",3.0*cm,vertices_steelframe_trap2);

    fsteelframe_trap2Logic=new G4LogicalVolume(steelframe_trap2,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframe_trap2");
    fsteelframe_trap2Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-3.0*cm), fsteelframe_trap2Logic, "steelframe_trap2", finnertriLogic, false, 0);
    fsteelframe_trap2Logic->SetVisAttributes(GrayVisAtt);


    std::vector<G4TwoVector> vertices_steelframe_trap3;

    vertices_steelframe_trap3.push_back(G4TwoVector((7.4815-0.6)*cm,59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector(7.4815*cm,59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector(7.4815*cm,-59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector((7.4815-0.6)*cm,-59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector((10.279326-0.6)*cm,59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector(10.279326*cm,59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector(10.279326*cm,-59.2*cm));
    vertices_steelframe_trap3.push_back(G4TwoVector((10.279326-0.6)*cm,-59.2*cm));

    G4GenericTrap* steelframe_trap3= new G4GenericTrap("steelframe_trap3",3.0*cm,vertices_steelframe_trap3);

    fsteelframe_trap3Logic=new G4LogicalVolume(steelframe_trap3,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframe_trap3");
    fsteelframe_trap3Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-3.0*cm), fsteelframe_trap3Logic, "steelframe_trap3", finnertriLogic, false, 0);
    fsteelframe_trap3Logic->SetVisAttributes(GrayVisAtt);


    std::vector<G4TwoVector> vertices_steelframe_trap4;

    vertices_steelframe_trap4.push_back(G4TwoVector((9.9993-6)*cm,59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((9.9993-0.6)*cm,59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((9.9993-0.6)*cm,-59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((9.9993-6)*cm,-59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((10.279326-6)*cm,59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((10.279326-0.6)*cm,59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((10.279326-0.6)*cm,-59.2*cm));
    vertices_steelframe_trap4.push_back(G4TwoVector((10.279326-6)*cm,-59.2*cm));


    G4GenericTrap* steelframe_trap4= new G4GenericTrap("steelframe_trap4",0.3*cm,vertices_steelframe_trap4);

    fsteelframe_trap4Logic=new G4LogicalVolume(steelframe_trap4,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"steelframe_trap4");
    fsteelframe_trap4Physi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,(zhalflength_innertri+(0.2*cm/(TMath::Sin(TMath::ATan(21./45.))))/2.)-0.3*cm), fsteelframe_trap4Logic, "steelframe_trap4", finnertriLogic, false, 0);
    fsteelframe_trap4Logic->SetVisAttributes(GrayVisAtt);


    //now the backside of the Cherenkov detector (septagonal shape is divided into 4 parts in order to use G4GenericTrap):

    G4double P1_x= -70*cm+2*cm/TMath::Sin(TMath::ATan(54/31.5));
    G4double P1_y= 0.0*cm;

    G4double P2_x= -54./31.5*P1_x/(-54./38.5-54./31.5);
    G4double P2_y= -54./38.5*P2_x;

    G4double P3_x= P2_y/54.*33.5;
    G4double P3_y= P2_y;

    G4double P4_x= (P2_y+34./27.5*P3_x)/(19.5/61.+34.5/27.5);
    G4double P4_y= 19.5/61.*P4_x;

    G4double P5_x= P4_x;
    G4double P5_y= -P4_y;

    G4double P6_x= (-P4_y-34/18.5*P5_x)/(-53.5/42.5-34./18.5);
    G4double P6_y= -53.5/42.5*P6_x;

    G4double P7_x= P6_y/53.5*30.5;
    G4double P7_y= P6_y;


    std::vector<G4TwoVector> vertices_backwindowframe_left;

    vertices_backwindowframe_left.push_back(G4TwoVector(-38.5*cm,54.0*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(-30.5*cm,-53.5*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(-70.0*cm,0.0*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(-38.5*cm,54.0*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(-30.5*cm,-53.5*cm));
    vertices_backwindowframe_left.push_back(G4TwoVector(-70.0*cm,0.0*cm));

    G4GenericTrap* backwindowframe_left= new G4GenericTrap("backwindowframe_left",2.0/2.*mm,vertices_backwindowframe_left);

    std::vector<G4TwoVector> vertices_backwindowframe_up;

    vertices_backwindowframe_up.push_back(G4TwoVector(-38.5*cm,54.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(33.5*cm,54.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(-38.5*cm,54.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(33.5*cm,54.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_up.push_back(G4TwoVector(0.0*cm,0.0*cm));

    G4GenericTrap* backwindowframe_up= new G4GenericTrap("backwindowframe_up",2.0/2.*mm,vertices_backwindowframe_up);

    std::vector<G4TwoVector> vertices_backwindowframe_right;

    vertices_backwindowframe_right.push_back(G4TwoVector(33.5*cm,54.0*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(61.0*cm,19.5*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(61.0*cm,-19.5*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(33.5*cm,54.0*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(61.0*cm,19.5*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(61.0*cm,-19.5*cm));
    vertices_backwindowframe_right.push_back(G4TwoVector(0.0*cm,0.0*cm));

    G4GenericTrap* backwindowframe_right= new G4GenericTrap("backwindowframe_right",2.0/2.*mm,vertices_backwindowframe_right);


    std::vector<G4TwoVector> vertices_backwindowframe_down;

    vertices_backwindowframe_down.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(61.0*cm,-19.5*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(42.5*cm,-53.5*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(-30.5*cm,-53.5*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(61.0*cm,-19.5*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(42.5*cm,-53.5*cm));
    vertices_backwindowframe_down.push_back(G4TwoVector(-30.5*cm,-53.5*cm));

    G4GenericTrap* backwindowframe_down= new G4GenericTrap("backwindowframe_down",2.0/2.*mm,vertices_backwindowframe_down);



    std::vector<G4TwoVector> vertices_backwindow_left;

    vertices_backwindow_left.push_back(G4TwoVector(P2_x,P2_y));
    vertices_backwindow_left.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_left.push_back(G4TwoVector(P7_x,P7_y));
    vertices_backwindow_left.push_back(G4TwoVector(P1_x,P1_y));
    vertices_backwindow_left.push_back(G4TwoVector(P2_x,P2_y));
    vertices_backwindow_left.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_left.push_back(G4TwoVector(P7_x,P7_y));
    vertices_backwindow_left.push_back(G4TwoVector(P1_x,P1_y));

    G4GenericTrap* backwindow_left_tocutfromAlframe= new G4GenericTrap("backwindow_left_tocutfromAlframe",1.88/2.*mm,vertices_backwindow_left);
    G4GenericTrap* backwindow_left_mylar= new G4GenericTrap("backwindow_left_mylar",0.07/2.*mm,vertices_backwindow_left);
    G4GenericTrap* backwindow_left_tetlar= new G4GenericTrap("backwindow_left_tetlar",0.05/2.*mm,vertices_backwindow_left);

    std::vector<G4TwoVector> vertices_backwindow_up;

    vertices_backwindow_up.push_back(G4TwoVector(P2_x,P2_y));
    vertices_backwindow_up.push_back(G4TwoVector(P3_x,P3_y));
    vertices_backwindow_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_up.push_back(G4TwoVector(P2_x,P2_y));
    vertices_backwindow_up.push_back(G4TwoVector(P3_x,P3_y));
    vertices_backwindow_up.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_up.push_back(G4TwoVector(0.0*cm,0.0*cm));

    G4GenericTrap* backwindow_up_tocutfromAlframe= new G4GenericTrap("backwindow_up_tocutfromAlframe",1.88/2.*mm,vertices_backwindow_up);
    G4GenericTrap* backwindow_up_mylar= new G4GenericTrap("backwindow_up_mylar",0.07/2.*mm,vertices_backwindow_up);
    G4GenericTrap* backwindow_up_tetlar= new G4GenericTrap("backwindow_up_tetlar",0.05/2.*mm,vertices_backwindow_up);

    std::vector<G4TwoVector> vertices_backwindow_right;

    vertices_backwindow_right.push_back(G4TwoVector(P3_x,P3_y));
    vertices_backwindow_right.push_back(G4TwoVector(P4_x,P4_y));
    vertices_backwindow_right.push_back(G4TwoVector(P5_x,P5_y));
    vertices_backwindow_right.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_right.push_back(G4TwoVector(P3_x,P3_y));
    vertices_backwindow_right.push_back(G4TwoVector(P4_x,P4_y));
    vertices_backwindow_right.push_back(G4TwoVector(P5_x,P5_y));
    vertices_backwindow_right.push_back(G4TwoVector(0.0*cm,0.0*cm));

    G4GenericTrap* backwindow_right_tocutfromAlframe= new G4GenericTrap("backwindow_right_tocutfromAlframe",1.88/2.*mm,vertices_backwindow_right);
    G4GenericTrap* backwindow_right_mylar= new G4GenericTrap("backwindow_right_mylar",0.07/2.*mm,vertices_backwindow_right);
    G4GenericTrap* backwindow_right_tetlar= new G4GenericTrap("backwindow_right_tetlar",0.05/2.*mm,vertices_backwindow_right);

    std::vector<G4TwoVector> vertices_backwindow_down;

    vertices_backwindow_down.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_down.push_back(G4TwoVector(P5_x,P5_y));
    vertices_backwindow_down.push_back(G4TwoVector(P6_x,P6_y));
    vertices_backwindow_down.push_back(G4TwoVector(P7_x,P7_y));
    vertices_backwindow_down.push_back(G4TwoVector(0.0*cm,0.0*cm));
    vertices_backwindow_down.push_back(G4TwoVector(P5_x,P5_y));
    vertices_backwindow_down.push_back(G4TwoVector(P6_x,P6_y));
    vertices_backwindow_down.push_back(G4TwoVector(P7_x,P7_y));

    G4GenericTrap* backwindow_down_tocutfromAlframe= new G4GenericTrap("backwindow_down_tocutfromAlframe",1.88/2.*mm,vertices_backwindow_down);
    G4GenericTrap* backwindow_down_mylar= new G4GenericTrap("backwindow_down_mylar",0.07/2.*mm,vertices_backwindow_down);
    G4GenericTrap* backwindow_down_tetlar= new G4GenericTrap("backwindow_down_tetlar",0.05/2.*mm,vertices_backwindow_down);



    //backwindow frame around the foils which is cut from Al frame, assumed it is 2 cm thick in y-direction and 2 mm thick in z direction
    fbackwindowframeleftLogic=new G4LogicalVolume(backwindowframe_left,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"backwindowframe_left");
    fbackwindowframeleftPhysi=new G4PVPlacement(0, G4ThreeVector(3.0*cm,0.0*cm,0.0*cm), fbackwindowframeleftLogic, "backwindowframe_left", fbackAlBoxLogic, false, 0);
    fbackwindowframeleftLogic->SetVisAttributes(GrayVisAtt);
    //fbackwindowframeleftLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowframeupLogic=new G4LogicalVolume(backwindowframe_up,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"backwindowframe_up");
    fbackwindowframeupPhysi=new G4PVPlacement(0, G4ThreeVector(3.0*cm,0.0*cm,0.0*cm), fbackwindowframeupLogic, "backwindowframe_up", fbackAlBoxLogic, false, 0);
    fbackwindowframeupLogic->SetVisAttributes(GrayVisAtt);
    //fbackwindowframeupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowframerightLogic=new G4LogicalVolume(backwindowframe_right,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"backwindowframe_right");
    fbackwindowframerightPhysi=new G4PVPlacement(0, G4ThreeVector(3.0*cm,0.0*cm,0.0*cm), fbackwindowframerightLogic, "backwindowframe_right", fbackAlBoxLogic, false, 0);
    fbackwindowframerightLogic->SetVisAttributes(GrayVisAtt);
    //fbackwindowframerightLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowframedownLogic=new G4LogicalVolume(backwindowframe_down,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"backwindowframe_down");
    fbackwindowframedownPhysi=new G4PVPlacement(0, G4ThreeVector(3.0*cm,0.0*cm,0.0*cm), fbackwindowframedownLogic, "backwindowframe_down", fbackAlBoxLogic, false, 0);
    fbackwindowframedownLogic->SetVisAttributes(GrayVisAtt);
    //fbackwindowframedownLogic->SetVisAttributes(G4VisAttributes::Invisible);


    //backwindow which is cut from Al frame and is part of the inner volume filled with Cherenkov gas
    fbackwindowleftLogic=new G4LogicalVolume(backwindow_left_tocutfromAlframe,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"backwindow_left_tocutfromAlframe");
    fbackwindowleftPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-0.06*mm), fbackwindowleftLogic, "backwindow_left_tocutfromAlframe", fbackwindowframeleftLogic, false, 0);
    fbackwindowleftLogic->SetVisAttributes(GreenVisAtt);
    //fbackwindowleftLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowupLogic=new G4LogicalVolume(backwindow_up_tocutfromAlframe,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"backwindow_up_tocutfromAlframe");
    fbackwindowupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-0.06*mm), fbackwindowupLogic, "backwindow_up_tocutfromAlframe", fbackwindowframeupLogic, false, 0);
    fbackwindowupLogic->SetVisAttributes(GreenVisAtt);
    //fbackwindowupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowrightLogic=new G4LogicalVolume(backwindow_right_tocutfromAlframe,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"backwindow_right_tocutfromAlframe");
    fbackwindowrightPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-0.06*mm), fbackwindowrightLogic, "backwindow_right_tocutfromAlframe", fbackwindowframerightLogic, false, 0);
    fbackwindowrightLogic->SetVisAttributes(GreenVisAtt);
    //fbackwindowrightLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowdownLogic=new G4LogicalVolume(backwindow_down_tocutfromAlframe,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"backwindow_down_tocutfromAlframe");
    fbackwindowdownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-0.06*mm), fbackwindowdownLogic, "backwindow_down_tocutfromAlframe", fbackwindowframedownLogic, false, 0);
    fbackwindowdownLogic->SetVisAttributes(GreenVisAtt);
    //fbackwindowdownLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //Mylar backwindow
    fbackwindowMylarleftLogic=new G4LogicalVolume(backwindow_left_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"backwindow_left_mylar");
    fbackwindowMylarleftPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.915*mm), fbackwindowMylarleftLogic, "backwindow_left_mylar", fbackwindowframeleftLogic, false, 0);
    fbackwindowMylarleftLogic->SetVisAttributes(CyanVisAtt);
    //fbackwindowMylarleftLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowMylarupLogic=new G4LogicalVolume(backwindow_up_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"backwindow_up_mylar");
    fbackwindowMylarupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.915*mm), fbackwindowMylarupLogic, "backwindow_up_mylar", fbackwindowframeupLogic, false, 0);
    fbackwindowMylarupLogic->SetVisAttributes(CyanVisAtt);
    //fbackwindowMylarupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowMylarrightLogic=new G4LogicalVolume(backwindow_right_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"backwindow_right_mylar");
    fbackwindowMylarrightPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.915*mm), fbackwindowMylarrightLogic, "backwindow_right_mylar", fbackwindowframerightLogic, false, 0);
    fbackwindowMylarrightLogic->SetVisAttributes(CyanVisAtt);
    //fbackwindowMylarrightLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowMylardownLogic=new G4LogicalVolume(backwindow_down_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"backwindow_down_mylar");
    fbackwindowMylardownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.915*mm), fbackwindowMylardownLogic, "backwindow_down_mylar", fbackwindowframedownLogic, false, 0);
    fbackwindowMylardownLogic->SetVisAttributes(CyanVisAtt);
    //fbackwindowMylardownLogic->SetVisAttributes(G4VisAttributes::Invisible);


    //Tedlar backwindow
    fbackwindowTetlarleftLogic=new G4LogicalVolume(backwindow_left_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"backwindow_left_tetlar");
    fbackwindowTetlarleftPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.975*mm), fbackwindowTetlarleftLogic, "backwindow_left_tetlar", fbackwindowframeleftLogic, false, 0);
    fbackwindowTetlarleftLogic->SetVisAttributes(MagentaVisAtt);
    //fbackwindowTetlarleftLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowTetlarupLogic=new G4LogicalVolume(backwindow_up_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"backwindow_up_tetlar");
    fbackwindowTetlarupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.975*mm), fbackwindowTetlarupLogic, "backwindow_up_tetlar", fbackwindowframeupLogic, false, 0);
    fbackwindowTetlarupLogic->SetVisAttributes(MagentaVisAtt);
    //fbackwindowTetlarupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowTetlarrightLogic=new G4LogicalVolume(backwindow_right_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"backwindow_right_tetlar");
    fbackwindowTetlarrightPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.975*mm), fbackwindowTetlarrightLogic, "backwindow_right_tetlar", fbackwindowframerightLogic, false, 0);
    fbackwindowTetlarrightLogic->SetVisAttributes(MagentaVisAtt);
    //fbackwindowTetlarrightLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fbackwindowTetlardownLogic=new G4LogicalVolume(backwindow_down_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"backwindow_down_tetlar");
    fbackwindowTetlardownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.975*mm), fbackwindowTetlardownLogic, "backwindow_down_tetlar", fbackwindowframedownLogic, false, 0);
    fbackwindowTetlardownLogic->SetVisAttributes(MagentaVisAtt);
    //fbackwindowTetlardownLogic->SetVisAttributes(G4VisAttributes::Invisible);


    //nose of the Cherenkov detector: (the x axis goes from right to left in the drawing!!!)

    //First the 2cm frame on which the nose is mounted is made

    std::vector<G4TwoVector> vertices_nose_frame_upperhalf;

    vertices_nose_frame_upperhalf.push_back(G4TwoVector(18.5*cm,32.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(40.5*cm,0.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(-43.0*cm,0.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(-21.5*cm,37.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(18.5*cm,32.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(40.5*cm,0.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(-43.0*cm,0.0*cm));
    vertices_nose_frame_upperhalf.push_back(G4TwoVector(-21.5*cm,37.0*cm));

    G4GenericTrap* nose_frame_upperhalf= new G4GenericTrap("nose_frame_upperhalf",2./2.*cm,vertices_nose_frame_upperhalf);

    std::vector<G4TwoVector> vertices_nose_frame_lowerhalf;

    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(40.5*cm,0.0*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(17.5*cm,-38.5*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(-21.5*cm,-33.0*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(-43.0*cm,0.0*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(40.5*cm,0.0*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(17.5*cm,-38.5*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(-21.5*cm,-33.0*cm));
    vertices_nose_frame_lowerhalf.push_back(G4TwoVector(-43.0*cm,0.0*cm));

    G4GenericTrap* nose_frame_lowerhalf= new G4GenericTrap("nose_frame_lowerhalf",2./2.*cm,vertices_nose_frame_lowerhalf);

   fnoseframeupLogic=new G4LogicalVolume(nose_frame_upperhalf,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"nose_frame_upperhalf");
   fnoseframeupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,96*cm), fnoseframeupLogic, "nose_frame_upperhalf", fMotherLogic, false, 0);
   fnoseframeupLogic->SetVisAttributes(GrayVisAtt);
   //fnoseframeupLogic->SetVisAttributes(G4VisAttributes::Invisible);

   fnoseframedownLogic=new G4LogicalVolume(nose_frame_lowerhalf,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"nose_frame_lowerhalf");
   fnoseframedownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,96*cm), fnoseframedownLogic, "nose_frame_lowerhalf", fMotherLogic, false, 0);
   fnoseframedownLogic->SetVisAttributes(GrayVisAtt);
   //fnoseframedownLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //make the inner volume of the frame which contains the Cherenkov gas

    G4double thickness_nose_PVC= 5*mm;//this is not the correct value!!!!! Needs to be replaced
    G4double thickness_frame=2.0*cm+thickness_nose_PVC;//in xy-frame

    //calculate new Points of the two inner hexagons, using first incorrect coordinate system (x axis going from left to right)
    G4double point1x_newhex = (-40.5*cm + thickness_frame/TMath::Sin(TMath::ATan(38.5/22.0)));
    G4double point1y_newhex = 0.0*cm;

    G4double point4x_newhex = (43.0*cm - thickness_frame/TMath::Sin(TMath::ATan(33./21.5)));
    G4double point4y_newhex = 0.0*cm;

    G4double point2x_newhex = (point1y_newhex-32./22.*point1x_newhex)/(-32./18.5-32/22.);
    G4double point2y_newhex = -32./18.5*point2x_newhex;

    G4double point3x_newhex = (point4y_newhex+37./21.5*point4x_newhex)/(74./21.5);
    G4double point3y_newhex = 37./21.5*point3x_newhex;

    G4double point5x_newhex = (point4y_newhex-33./21.5*point4x_newhex)/(-33./21.5-33./21.5);
    G4double point5y_newhex = -33./21.5*point5x_newhex;

    G4double point6x_newhex = (point1y_newhex+38.5/23.*point1x_newhex)/(38.5/17.5+38.5/23.);
    G4double point6y_newhex = 38.5/17.5*point6x_newhex;

    std::vector<G4TwoVector> vertices_hexframe_upperhalf_inner;

    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point3x_newhex,point3y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point2x_newhex,point2y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point3x_newhex,point3y_newhex));
    vertices_hexframe_upperhalf_inner.push_back(G4TwoVector(-1.*point2x_newhex,point2y_newhex));


    G4GenericTrap* hex_noseframe_upperhalf_inner= new G4GenericTrap("hex_noseframe_upperhalf_inner",(2*cm)/2.,vertices_hexframe_upperhalf_inner);//
    G4GenericTrap* frontwindow_atAlBox_upperhalf= new G4GenericTrap("frontwindow_atAlBox_upperhalf",(6*mm)/2.,vertices_hexframe_upperhalf_inner);//

    std::vector<G4TwoVector> vertices_hexframe_lowerhalf_inner;

    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point6x_newhex,point6y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point5x_newhex,point5y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point6x_newhex,point6y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point5x_newhex,point5y_newhex));
    vertices_hexframe_lowerhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));

    G4GenericTrap* hex_noseframe_lowerhalf_inner= new G4GenericTrap("hex_noseframe_lowerhalf_inner",(2*cm)/2.,vertices_hexframe_lowerhalf_inner);
    G4GenericTrap* frontwindow_atAlBox_lowerhalf= new G4GenericTrap("frontwindow_atAlBox_lowerhalf",(6*mm)/2.,vertices_hexframe_lowerhalf_inner);//

    fnoseframeinnerupLogic=new G4LogicalVolume(hex_noseframe_upperhalf_inner,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"hex_noseframe_upperhalf_inner");
    fnoseframeinnerupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), fnoseframeinnerupLogic, "hex_noseframe_upperhalf_inner", fnoseframeupLogic, false, 0);
    fnoseframeinnerupLogic->SetVisAttributes(GreenVisAtt);
    //fnoseframeinnerupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fnoseframeinnerdownLogic=new G4LogicalVolume(hex_noseframe_lowerhalf_inner,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"hex_noseframe_lowerhalf_inner");
    fnoseframeinnerdownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), fnoseframeinnerdownLogic, "hex_noseframe_lowerhalf_inner", fnoseframedownLogic, false, 0);
    fnoseframeinnerdownLogic->SetVisAttributes(GreenVisAtt);
    //fnoseframeinnerdownLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //this is the front window at the Al box which represents the Cherenkov gas volume
    frontwindowupperLogic=new G4LogicalVolume(frontwindow_atAlBox_upperhalf,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"frontwindow_atAlBox_upperhalf");
    frontwindowupperPhysi=new G4PVPlacement(0, G4ThreeVector(15.5*cm,0.0*cm,-35.6*cm), frontwindowupperLogic, "frontwindow_atAlBox_upperhalf", fAlBoxLogic, false, 0);
    frontwindowupperLogic->SetVisAttributes(GreenVisAtt);
    //frontwindowupperLogic->SetVisAttributes(G4VisAttributes::Invisible);

    frontwindowlowerLogic=new G4LogicalVolume(frontwindow_atAlBox_lowerhalf,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"frontwindow_atAlBox_lowerhalf");
    frontwindowlowerPhysi=new G4PVPlacement(0, G4ThreeVector(15.5*cm,0.0*cm,-35.6*cm), frontwindowlowerLogic, "frontwindow_atAlBox_lowerhalf", fAlBoxLogic, false, 0);
    frontwindowlowerLogic->SetVisAttributes(GreenVisAtt);
    //frontwindowlowerLogic->SetVisAttributes(G4VisAttributes::Invisible);

    //now the nose itself
    G4double distance=2.0*cm;//distance from outer nose frame to the beginning of the nose

    //calculate new Points of the two inner hexagons, using first incorrect coordinate system (x axis going from left to right)
    G4double point1x_nose = (-40.5*cm + distance/TMath::Sin(TMath::ATan(38.5/22.0)));
    G4double point1y_nose = 0.0*cm;

    G4double point4x_nose = (43.0*cm - distance/TMath::Sin(TMath::ATan(33./21.5)));
    G4double point4y_nose = 0.0*cm;

    G4double point2x_nose = (point1y_nose-32./22.*point1x_nose)/(-32./18.5-32/22.);
    G4double point2y_nose = -32./18.5*point2x_nose;

    G4double point3x_nose = (point4y_nose+37./21.5*point4x_nose)/(74./21.5);
    G4double point3y_nose = 37./21.5*point3x_nose;

    G4double point5x_nose = (point4y_nose-33./21.5*point4x_nose)/(-33./21.5-33./21.5);
    G4double point5y_nose = -33./21.5*point5x_nose;

    G4double point6x_nose = (point1y_nose+38.5/23.*point1x_nose)/(38.5/17.5+38.5/23.);
    G4double point6y_nose = 38.5/17.5*point6x_nose;

    std::vector<G4TwoVector> vertices_hex_nose_upperhalf;

    vertices_hex_nose_upperhalf.push_back(G4TwoVector(24.5*cm,0.0*cm));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-28.0*cm,0.0*cm));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-12.5*cm,24.0*cm));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(10.5*cm,20.0*cm));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-1.*point1x_nose,point1y_nose));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-1.*point4x_nose,point4y_nose));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-1.*point3x_nose,point3y_nose));
    vertices_hex_nose_upperhalf.push_back(G4TwoVector(-1.*point2x_nose,point2y_nose));

    G4GenericTrap* hex_nose_upperhalf= new G4GenericTrap("hex_nose_upperhalf",35./2.*cm,vertices_hex_nose_upperhalf);

    std::vector<G4TwoVector> vertices_hex_nose_lowerhalf;

    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(24.5*cm,0.0*cm));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(9.8*cm,-23.5*cm));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-14.0*cm,-19.5*cm));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-28.0*cm,0.0*cm));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-1.*point1x_nose,point1y_nose));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-1.*point6x_nose,point6y_nose));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-1.*point5x_nose,point5y_nose));
    vertices_hex_nose_lowerhalf.push_back(G4TwoVector(-1.*point4x_nose,point4y_nose));

    G4GenericTrap* hex_nose_lowerhalf= new G4GenericTrap("hex_nose_lowerhalf",35./2.*cm,vertices_hex_nose_lowerhalf);

    fnoseupLogic=new G4LogicalVolume(hex_nose_upperhalf,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"hex_nose_upperhalf");
    fnoseupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,77.5*cm), fnoseupLogic, "hex_nose_upperhalf", fMotherLogic, false, 0);
    fnoseupLogic->SetVisAttributes(BlueVisAtt);
    //fnoseupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fnosedownLogic=new G4LogicalVolume(hex_nose_lowerhalf,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"hex_nose_lowerhalf");
    fnosedownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,77.5*cm), fnosedownLogic, "hex_nose_lowerhalf", fMotherLogic, false, 0);
    fnosedownLogic->SetVisAttributes(BlueVisAtt);
    //fnosedownLogic->SetVisAttributes(G4VisAttributes::Invisible);



    //inner volume of the nose of the Cherenkov detector:

    G4double point1x_smallnewhex = (-24.5*cm + thickness_nose_PVC/TMath::Sin(TMath::ATan(23.5/14.7)));
    G4double point1y_smallnewhex = 0.0*cm;

    G4double point4x_smallnewhex = (28.0*cm - thickness_nose_PVC/TMath::Sin(TMath::ATan(19.5/14.0)));
    G4double point4y_smallnewhex = 0.0*cm;

    G4double point2x_smallnewhex = (point1y_smallnewhex  - 20./14.*point1x_smallnewhex)/(-20./10.5-20./14.);
    G4double point2y_smallnewhex = -20./10.5*point2x_smallnewhex;

    G4double point3x_smallnewhex = (point4y_smallnewhex+24./15.5*point4x_smallnewhex)/(24./12.5+24./15.5);
    G4double point3y_smallnewhex = 24./12.5*point3x_smallnewhex;

    G4double point5x_smallnewhex = (point4y_smallnewhex-19.5/14.*point4x_smallnewhex)/(-19.5/14.-19.5/14.);
    G4double point5y_smallnewhex = -19.5/14.0*point5x_smallnewhex;

    G4double point6x_smallnewhex = (point1y_smallnewhex+23.5/14.7*point1x_smallnewhex)/(23.5/9.8+23.5/14.7);
    G4double point6y_smallnewhex = 23.5/9.8*point6x_smallnewhex;

    std::vector<G4TwoVector> vertices_hex_nose_upperhalf_inner;

    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point3x_smallnewhex,point3y_smallnewhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point2x_smallnewhex,point2y_smallnewhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));//same as the base of the nose frame inner volume
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point3x_newhex,point3y_newhex));
    vertices_hex_nose_upperhalf_inner.push_back(G4TwoVector(-1.*point2x_newhex,point2y_newhex));


    G4GenericTrap* hex_nose_upperhalf_inner= new G4GenericTrap("hex_nose_upperhalf_inner",(35.*cm-0.12*mm)/2.,vertices_hex_nose_upperhalf_inner);//

    std::vector<G4TwoVector> vertices_hex_nose_lowerhalf_inner;

    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point6x_smallnewhex,point6y_smallnewhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point5x_smallnewhex,point5y_smallnewhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point1x_newhex,point1y_newhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point6x_newhex,point6y_newhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point5x_newhex,point5y_newhex));
    vertices_hex_nose_lowerhalf_inner.push_back(G4TwoVector(-1.*point4x_newhex,point4y_newhex));

    G4GenericTrap* hex_nose_lowerhalf_inner= new G4GenericTrap("hex_nose_lowerhalf_inner",(35.*cm-0.12*mm)/2.,vertices_hex_nose_lowerhalf_inner);

    fnoseinnerupLogic=new G4LogicalVolume(hex_nose_upperhalf_inner,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"hex_nose_upperhalf_inner");
    fnoseinnerupPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.12*mm/2.0), fnoseinnerupLogic, "hex_nose_upperhalf_inner", fnoseupLogic, false, 0);
    fnoseinnerupLogic->SetVisAttributes(GreenVisAtt);
    //fnoseinnerupLogic->SetVisAttributes(G4VisAttributes::Invisible);

    fnoseinnerdownLogic=new G4LogicalVolume(hex_nose_lowerhalf_inner,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"hex_nose_lowerhalf_inner");
    fnoseinnerdownPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,0.12*mm/2.0), fnoseinnerdownLogic, "hex_nose_lowerhalf_inner", fnosedownLogic, false, 0);
    fnoseinnerdownLogic->SetVisAttributes(GreenVisAtt);
    //fnoseinnerdownLogic->SetVisAttributes(G4VisAttributes::Invisible);


    //front window at the nose (hexagonal shape is divided into two parts at the x-axis)
    std::vector<G4TwoVector> vertices_frontwindow_nose_upperhalf;

    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point3x_smallnewhex,point3y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point2x_smallnewhex,point2y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point3x_smallnewhex,point3y_smallnewhex));
    vertices_frontwindow_nose_upperhalf.push_back(G4TwoVector(-1.*point2x_smallnewhex,point2y_smallnewhex));

    G4GenericTrap* frontwindow_nose_upperhalf_pvc= new G4GenericTrap("frontwindow_nose_upperhalf_pvc",5./2.*mm,vertices_frontwindow_nose_upperhalf);
    G4GenericTrap* frontwindow_nose_upperhalf_mylar= new G4GenericTrap("frontwindow_nose_upperhalf_mylar",0.07/2.*mm,vertices_frontwindow_nose_upperhalf);
    G4GenericTrap* frontwindow_nose_upperhalf_tetlar= new G4GenericTrap("frontwindow_nose_upperhalf_tetlar",0.05/2.*mm,vertices_frontwindow_nose_upperhalf);


    std::vector<G4TwoVector> vertices_frontwindow_nose_lowerhalf;

    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point6x_smallnewhex,point6y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point5x_smallnewhex,point5y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point1x_smallnewhex,point1y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point6x_smallnewhex,point6y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point5x_smallnewhex,point5y_smallnewhex));
    vertices_frontwindow_nose_lowerhalf.push_back(G4TwoVector(-1.*point4x_smallnewhex,point4y_smallnewhex));

    G4GenericTrap* frontwindow_nose_lowerhalf_pvc= new G4GenericTrap("frontwindow_nose_lowerhalf_pvc",5./2.*mm,vertices_frontwindow_nose_lowerhalf);
    G4GenericTrap* frontwindow_nose_lowerhalf_mylar= new G4GenericTrap("frontwindow_nose_lowerhalf_tetlar",0.07/2.*mm,vertices_frontwindow_nose_lowerhalf);
    G4GenericTrap* frontwindow_nose_lowerhalf_tetlar= new G4GenericTrap("frontwindow_nose_lowerhalf_tetlar",0.05/2.*mm,vertices_frontwindow_nose_lowerhalf);

    ffrontwindow_noseup_pvcLogic=new G4LogicalVolume(frontwindow_nose_upperhalf_pvc,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"frontwindow_nose_upperhalf_pvc");
    ffrontwindow_noseup_pvcPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.2345*cm), ffrontwindow_noseup_pvcLogic, "frontwindow_nose_upperhalf_pvc", fnoseinnerupLogic, false, 0);
    ffrontwindow_noseup_pvcLogic->SetVisAttributes(BlueVisAtt);
    //ffrontwindow_noseup_pvcLogic->SetVisAttributes(G4VisAttributes::Invisible);

    ffrontwindow_nosedown_pvcLogic=new G4LogicalVolume(frontwindow_nose_lowerhalf_pvc,fNistManager->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"),"frontwindow_nose_lowerhalf_pvc");
    ffrontwindow_nosedown_pvcPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.2345*cm), ffrontwindow_nosedown_pvcLogic, "frontwindow_nose_lowerhalf_pvc", fnoseinnerdownLogic, false, 0);
    ffrontwindow_nosedown_pvcLogic->SetVisAttributes(BlueVisAtt);
    //ffrontwindow_nosedown_pvcLogic->SetVisAttributes(G4VisAttributes::Invisible);

    G4Tubs* frontholeup= new G4Tubs("frontholeup",0.0*cm,29.3/2.*cm, 5.0/2.*mm,0.,180.*deg);
    G4Tubs* frontholedown= new G4Tubs("frontholedown",0.0*cm,29.3/2.*cm, 5./2.*mm,0.,180.*deg);

    G4RotationMatrix* rm_hole = new G4RotationMatrix();
    rm_hole->rotateX(-180.*deg);

    ffrontholeupLogic=new G4LogicalVolume(frontholeup,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"frontholeup");
    ffrontholeupPhysi=new G4PVPlacement(0, G4ThreeVector(-1.75*cm,0.0*cm,0.0*cm), ffrontholeupLogic, "frontholeup", ffrontwindow_noseup_pvcLogic, false, 0);
    ffrontholeupLogic->SetVisAttributes(GreenVisAtt);

   ffrontholedownLogic=new G4LogicalVolume(frontholedown,fNistManager->FindOrBuildMaterial("A2_Cherenkovgas"),"frontholedown");
    ffrontholedownPhysi=new G4PVPlacement(rm_hole, G4ThreeVector(-1.75*cm,0.0*cm,0.0*cm), ffrontholedownLogic, "frontholedown", ffrontwindow_nosedown_pvcLogic, false, 0);
    ffrontholedownLogic->SetVisAttributes(GreenVisAtt);


    ffrontwindow_noseup_mylarLogic=new G4LogicalVolume(frontwindow_nose_upperhalf_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"frontwindow_nose_upperhalf_mylar");
    ffrontwindow_noseup_mylarPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.4915*cm), ffrontwindow_noseup_mylarLogic, "frontwindow_nose_upperhalf_mylar", fnoseupLogic, false, 0);//thickness_nose/2.0
    ffrontwindow_noseup_mylarLogic->SetVisAttributes(CyanVisAtt);
    //ffrontwindow_noseup_mylarLogic->SetVisAttributes(G4VisAttributes::Invisible);

    ffrontwindow_nosedown_mylarLogic=new G4LogicalVolume(frontwindow_nose_lowerhalf_mylar,fNistManager->FindOrBuildMaterial("G4_MYLAR"),"frontwindow_nose_lowerhalf_mylar");
    ffrontwindow_nosedown_mylarPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.4915*cm), ffrontwindow_nosedown_mylarLogic, "frontwindow_nose_lowerhalf_mylar", fnosedownLogic, false, 0);//thickness_nose/2.0
    ffrontwindow_nosedown_mylarLogic->SetVisAttributes(CyanVisAtt);
    //ffrontwindow_nosedown_mylarLogic->SetVisAttributes(G4VisAttributes::Invisible);

    ffrontwindow_nose_upperhalf_tetlarLogic=new G4LogicalVolume(frontwindow_nose_upperhalf_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"frontwindow_nose_upperhalf_tetlar");
    ffrontwindow_nose_upperhalf_tetlarPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.4975*cm), ffrontwindow_nose_upperhalf_tetlarLogic, "frontwindow_nose_upperhalf_tetlar", fnoseupLogic, false, 0);//thickness_nose/2.0
    ffrontwindow_nose_upperhalf_tetlarLogic->SetVisAttributes(MagentaVisAtt);
    //ffrontwindow_nose_upperhalf_tetlarLogic->SetVisAttributes(G4VisAttributes::Invisible);

    ffrontwindow_nose_lowerhalf_tetlarLogic=new G4LogicalVolume(frontwindow_nose_lowerhalf_tetlar,fNistManager->FindOrBuildMaterial("A2_Tedlar"),"frontwindow_nose_lowerhalf_tetlar");
    ffrontwindow_nose_lowerhalf_tetlarPhysi=new G4PVPlacement(0, G4ThreeVector(0.0*cm,0.0*cm,-17.4975*cm), ffrontwindow_nose_lowerhalf_tetlarLogic, "frontwindow_nose_lowerhalf_tetlar", fnosedownLogic, false, 0);//thickness_nose/2.0
    ffrontwindow_nose_lowerhalf_tetlarLogic->SetVisAttributes(MagentaVisAtt);
    //ffrontwindow_nose_lowerhalf_tetlarLogic->SetVisAttributes(G4VisAttributes::Invisible);

// now the mirror of the Cherenkov detector

    G4double X_SemiAxis = 52.0*cm, mirrorThickness = 3*mm;
    G4double Y_SemiAxis = 43.5*cm;
    G4double Z_SemiAxis = 16.0*cm;

    G4RotationMatrix* rm_mirror = new G4RotationMatrix();
    rm_mirror->rotateY(-26.*deg);

    G4Ellipsoid *InnerOblate = new G4Ellipsoid("innerObl", X_SemiAxis+0.01*mm, Y_SemiAxis, Z_SemiAxis+0.01*mm, -0.1*cm, 16.0*cm );
    G4Ellipsoid *OuterOblate = new G4Ellipsoid("OuterOblate", X_SemiAxis+mirrorThickness, Y_SemiAxis+mirrorThickness, Z_SemiAxis+mirrorThickness, -0.1*cm, 16.0*cm );

    G4SubtractionSolid* solidOblateSub = new G4SubtractionSolid("solidOblateSub", OuterOblate, InnerOblate,0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm));

    fmirrorLogic=new G4LogicalVolume(solidOblateSub,fNistManager->FindOrBuildMaterial("A2_Pyrex"),"solidOblateSub");
    fmirrorPhysi=new G4PVPlacement(rm_mirror, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), fmirrorLogic, "solidOblateSub", finnerboxvolumeLogic, false, 0);
    fmirrorLogic->SetVisAttributes(BlueVisAtt);
    //fmirrorLogic->SetVisAttributes(G4VisAttributes::Invisible);

//the frame of the mirror
    G4Box* mirrorframeboxfull=new G4Box("mirrorframeboxfull",35./2.*cm,104./2.*cm,4.5/2.*cm);
    G4Box* mirrorframeboxtocut=new G4Box("mirrorframeboxtocut",35.01/2.*cm,95./2.*cm,4.51/2.*cm);
    G4SubtractionSolid* mirrorframebox = new G4SubtractionSolid("mirrorframebox", mirrorframeboxfull, mirrorframeboxtocut, 0, G4ThreeVector(0.*cm,0.*cm,0.*cm));

    mirrorframeboxLogic=new G4LogicalVolume(mirrorframebox,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"mirrorframebox");
    mirrorframeboxPhysi=new G4PVPlacement(rm_mirror, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), mirrorframeboxLogic, "mirrorframebox", finnerboxvolumeLogic, false, 0);
    mirrorframeboxLogic->SetVisAttributes(GrayVisAtt);
    //mirrorframeboxLogic->SetVisAttributes(G4VisAttributes::Invisible);

    std::vector<G4TwoVector> vertices_mirrorframe_outer;

    vertices_mirrorframe_outer.push_back(G4TwoVector(17.5*cm,52.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(60.*cm,38.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(60.*cm,-38.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(17.5*cm,-52.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(17.5*cm,52.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(60.*cm,38.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(60.*cm,-38.*cm));
    vertices_mirrorframe_outer.push_back(G4TwoVector(17.5*cm,-52.*cm));

    G4GenericTrap* mirrorframe_outer= new G4GenericTrap("mirrorframe_outer",4.5/2.*cm,vertices_mirrorframe_outer);

    std::vector<G4TwoVector> vertices_mirrorframe_inner;

    vertices_mirrorframe_inner.push_back(G4TwoVector(17.5*cm,47.5*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(55.5*cm,33.48*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(55.5*cm,-33.48*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(17.5*cm,-47.5*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(17.5*cm,47.5*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(55.5*cm,33.48*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(55.5*cm,-33.48*cm));
    vertices_mirrorframe_inner.push_back(G4TwoVector(17.5*cm,-47.5*cm));

    G4GenericTrap* mirrorframe_inner= new G4GenericTrap("mirrorframe_inner",4.51/2.*cm,vertices_mirrorframe_inner);

    G4SubtractionSolid* mirrorframetrap = new G4SubtractionSolid("mirrorframetrap", mirrorframe_outer, mirrorframe_inner, 0, G4ThreeVector(-0.1*cm,0.*cm,0.*cm));

    mirrorframetrapLogic=new G4LogicalVolume(mirrorframetrap,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"),"mirrorframetrap");
    mirrorframetrapPhysi=new G4PVPlacement(rm_mirror, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), mirrorframetrapLogic, "mirrorframetrap", finnerboxvolumeLogic, false, 0);
    mirrorframetrapLogic->SetVisAttributes(GrayVisAtt);
    //mirrorframetrapLogic->SetVisAttributes(G4VisAttributes::Invisible);

    std::vector<G4TwoVector> vertices_mirrorframe_outer_copy;

    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-17.5*cm,52.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-17.5*cm,-52.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-60.*cm,-38.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-60.*cm,38.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-17.5*cm,52.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-17.5*cm,-52.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-60.*cm,-38.*cm));
    vertices_mirrorframe_outer_copy.push_back(G4TwoVector(-60.*cm,38.*cm));

    G4GenericTrap* mirrorframe_outer_copy= new G4GenericTrap("mirrorframe_outer_copy",4.5/2.*cm,vertices_mirrorframe_outer_copy);

    std::vector<G4TwoVector> vertices_mirrorframe_inner_copy;

    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-17.5*cm,47.5*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-17.5*cm,-47.5*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-55.5*cm,-33.48*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-55.5*cm,33.48*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-17.5*cm,47.5*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-17.5*cm,-47.5*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-55.5*cm,-33.48*cm));
    vertices_mirrorframe_inner_copy.push_back(G4TwoVector(-55.5*cm,33.48*cm));

    G4GenericTrap* mirrorframe_inner_copy= new G4GenericTrap("mirrorframe_inner_copy",4.51/2.*cm,vertices_mirrorframe_inner_copy);

    G4SubtractionSolid* mirrorframetrap_copy = new G4SubtractionSolid("mirrorframetrap_copy", mirrorframe_outer_copy, mirrorframe_inner_copy, 0, G4ThreeVector(0.1*cm,0.*cm,0.*cm));

    mirrorframetrapcopyLogic=new G4LogicalVolume(mirrorframetrap_copy,fNistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL"), "mirrorframetrap_copy");
    mirrorframetrapcopyPhysi=new G4PVPlacement(rm_mirror, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), mirrorframetrapcopyLogic, "mirrorframetrap_copy", finnerboxvolumeLogic, false, 0);
    mirrorframetrapcopyLogic->SetVisAttributes(GrayVisAtt);
    //mirrorframetrapcopyLogic->SetVisAttributes(G4VisAttributes::Invisible);

//round plate around mirror attached to the mirror frame
   //G4Tubs* roundplate= new G4Tubs("roundplate",87.1/2.*cm,91.1/2.*cm, 2*mm,0.,140*deg);

    std::vector<G4TwoVector> vertices_mirrorframe_inner2;

    vertices_mirrorframe_inner2.push_back(G4TwoVector(17.5*cm,47.25*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(55.25*cm,33.*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(55.25*cm,-33.*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(17.5*cm,-47.25*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(17.5*cm,47.25*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(55.25*cm,33.*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(55.25*cm,-33.*cm));
    vertices_mirrorframe_inner2.push_back(G4TwoVector(17.5*cm,-47.25*cm));

    std::vector<G4TwoVector> vertices_mirrorframe_inner_copy2;

    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-17.5*cm,47.25*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-17.5*cm,-47.25*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-55.25*cm,-33.*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-55.25*cm,33.*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-17.5*cm,47.25*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-17.5*cm,-47.25*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-55.25*cm,-33.*cm));
    vertices_mirrorframe_inner_copy2.push_back(G4TwoVector(-55.25*cm,33.*cm));


    G4GenericTrap* mirrorframe_inner2= new G4GenericTrap("mirrorframe_inner2", 0.3*cm,vertices_mirrorframe_inner2);
    G4GenericTrap* mirrorframe_inner_copy2= new G4GenericTrap("mirrorframe_inner_copy2",0.3*cm,vertices_mirrorframe_inner_copy2);
    G4Box* mirrorframeboxtocut2=new G4Box("mirrorframeboxtocut2",34.5/2.*cm,94.5/2.*cm,0.28*cm);
    G4Box* boxtoadd=new G4Box("mirrorframeboxtocut",60.0/2.*cm,40./2.*cm,0.3*cm);

    G4UnionSolid* mirrorframe_trap_left = new G4UnionSolid("mirrorframe_trap_left", mirrorframe_inner2, boxtoadd,0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm));
    G4UnionSolid* mirrorframe_trap_left_box = new G4UnionSolid("mirrorframe_trap_left_box", mirrorframe_trap_left, mirrorframeboxtocut2,0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm));
    G4UnionSolid* mirrorframe_trap_all = new G4UnionSolid("mirrorframe_trap_all", mirrorframe_trap_left_box, mirrorframe_inner_copy2, 0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm));

   G4EllipticalTube *Innerplate= new G4EllipticalTube("innerplate", X_SemiAxis+mirrorThickness, Y_SemiAxis+mirrorThickness, mirrorThickness+0.3*cm);
   G4SubtractionSolid* roundplate = new G4SubtractionSolid("roundplate", mirrorframe_trap_all, Innerplate,0, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm));

   G4EllipticalTube *Outerplate= new G4EllipticalTube("Outerplate", X_SemiAxis+mirrorThickness+9*cm, Y_SemiAxis+mirrorThickness+9*cm, mirrorThickness);
   G4IntersectionSolid* roundplate1 = new G4IntersectionSolid("roundplate1", roundplate, Outerplate);

    roundplateLogic=new G4LogicalVolume(roundplate1,fNistManager->FindOrBuildMaterial("G4_Al"), "roundplate1");
    roundplatePhysi=new G4PVPlacement(rm_mirror, G4ThreeVector(0.0*cm,0.0*cm,0.0*cm), roundplateLogic, "roundplate1", finnerboxvolumeLogic, false, 0);//.
    roundplateLogic->SetVisAttributes(RedVisAtt);
    //roundplateLogic->SetVisAttributes(G4VisAttributes::Invisible);


}
