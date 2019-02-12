// Generic solid target supporting different types used
// in past experiments.
// Author: Dominik Werthmueller, 2018-2019

#ifndef A2SolidTargetGeneric_h
#define A2SolidTargetGeneric_h 1

#include "A2Target.hh"

class A2SolidTargetGeneric : public A2Target
{

public:
    enum ESolidTargetType {
        kStandard,
        kOct_18,        // neutron skin October/November 2018
    };

protected:
    ESolidTargetType fType;     // target type

public:
    A2SolidTargetGeneric(ESolidTargetType type = kStandard);
    ~A2SolidTargetGeneric() { }

    virtual G4VPhysicalVolume* Construct(G4LogicalVolume* motherLogic, G4double z0 = 0);

};

#endif

