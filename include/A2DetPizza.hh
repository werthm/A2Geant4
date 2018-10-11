// Pizza detector
// Author: Dominik Werthmueller, 2016

#ifndef A2DetPizza_h
#define A2DetPizza_h 1

#include "A2Detector.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Region;
class A2SD;
class A2VisSD;

class A2DetPizza : public A2Detector
{

protected:
    G4double fZPos;             // z-distance from target center to scintillator face
    G4bool fIsCheckOverlap;     // flag for volume overlap check
    G4Region* fRegionPizza;     // region of pizza detector
    A2SD* fPizzaSD;             // sensitive detector
    A2VisSD* fPizzaVisSD;       // sensitive detector (interactive)

public:
    A2DetPizza(G4double zpos);
    virtual ~A2DetPizza();

    void SetCheckOverlap(G4bool b) { fIsCheckOverlap = b; }

    virtual G4VPhysicalVolume* Construct(G4LogicalVolume* motherLogic);

    static const G4double fgDefaultZPos;
};

#endif

