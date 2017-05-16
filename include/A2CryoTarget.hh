
#ifndef A2CryoTarget_h
#define A2CryoTarget_h 1

#include "A2Target.hh"

class A2CryoTarget: public  A2Target
{

public:
  enum A2CryoTargetType {
    kCryo1,
    kCryo2,
    kCryoHe
  };

  A2CryoTarget(A2CryoTargetType type);
  ~A2CryoTarget();

  virtual G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogi);
  virtual G4VPhysicalVolume* Construct1(G4LogicalVolume *MotherLogi);
  virtual G4VPhysicalVolume* Construct2(G4LogicalVolume *MotherLogic);
  void SetTargetLength(G4double Len){fTargetLength=Len;}

private:
 G4double fTargetLength;
 A2CryoTargetType fType;

};
#endif
