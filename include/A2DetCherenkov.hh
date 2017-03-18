
#ifndef A2DetCherenkov_h
#define A2DetCherenkov_h 1

#include "A2Detector.hh"

class A2DetCherenkov : public A2Detector
{
public:
  
  A2DetCherenkov();
  ~A2DetCherenkov();

  G4VPhysicalVolume* Construct(G4LogicalVolume *MotherLogic);
  void CheckOverlapAndAbort(G4VPhysicalVolume* vol, const G4String& location);
  void MakeCherenkov();

  private:
  G4bool fIsCheckOverlap;

  G4LogicalVolume *fAirBoxLogic;
  G4LogicalVolume *fAlBoxLogic;
  G4LogicalVolume *fbackAlBoxLogic;
  G4LogicalVolume *finnerboxvolumeLogic;
  G4LogicalVolume *fsmallboxLogic;
  G4LogicalVolume *ftriAlLogic;
  G4LogicalVolume *finnertriLogic;

  G4LogicalVolume *fbackwindowframedownLogic;
  G4LogicalVolume *fbackwindowframerightLogic;
  G4LogicalVolume *fbackwindowframeupLogic;
  G4LogicalVolume *fbackwindowframeleftLogic;

  G4LogicalVolume *fbackwindowdownLogic;
  G4LogicalVolume *fbackwindowrightLogic;
  G4LogicalVolume *fbackwindowupLogic;
  G4LogicalVolume *fbackwindowleftLogic;

  G4LogicalVolume *fbackwindowMylardownLogic;
  G4LogicalVolume *fbackwindowMylarrightLogic;
  G4LogicalVolume *fbackwindowMylarupLogic;
  G4LogicalVolume *fbackwindowMylarleftLogic;

  G4LogicalVolume *fbackwindowTetlardownLogic;
  G4LogicalVolume *fbackwindowTetlarrightLogic;
  G4LogicalVolume *fbackwindowTetlarupLogic;
  G4LogicalVolume *fbackwindowTetlarleftLogic;

  G4LogicalVolume *frontwindowupperLogic;
  G4LogicalVolume *frontwindowlowerLogic;

  G4LogicalVolume *fnoseupLogic;
  G4LogicalVolume *fnosedownLogic;
  G4LogicalVolume *fnoseinnerupLogic;
  G4LogicalVolume *fnoseinnerdownLogic;

  G4VPhysicalVolume *fAirBoxPhysi;
  G4VPhysicalVolume *fAlBoxPhysi;
  G4VPhysicalVolume *fbackAlBoxPhysi;
  G4VPhysicalVolume *finnerboxvolumePhysi;
  G4VPhysicalVolume *fsmallboxPhysi;
  G4VPhysicalVolume *ftriAlPhysi;
  G4VPhysicalVolume *finnertriPhysi;

  G4VPhysicalVolume *fbackwindowframedownPhysi;
  G4VPhysicalVolume *fbackwindowframerightPhysi;
  G4VPhysicalVolume *fbackwindowframeupPhysi;
  G4VPhysicalVolume *fbackwindowframeleftPhysi;

  G4VPhysicalVolume *fbackwindowdownPhysi;
  G4VPhysicalVolume *fbackwindowrightPhysi;
  G4VPhysicalVolume *fbackwindowupPhysi;
  G4VPhysicalVolume *fbackwindowleftPhysi;

  G4VPhysicalVolume *fbackwindowMylardownPhysi;
  G4VPhysicalVolume *fbackwindowMylarrightPhysi;
  G4VPhysicalVolume *fbackwindowMylarupPhysi;
  G4VPhysicalVolume *fbackwindowMylarleftPhysi;

  G4VPhysicalVolume *fbackwindowTetlardownPhysi;
  G4VPhysicalVolume *fbackwindowTetlarrightPhysi;
  G4VPhysicalVolume *fbackwindowTetlarupPhysi;
  G4VPhysicalVolume *fbackwindowTetlarleftPhysi;

  G4VPhysicalVolume *frontwindowupperPhysi;
  G4VPhysicalVolume *frontwindowlowerPhysi;

  G4VPhysicalVolume *fnoseupPhysi;
  G4VPhysicalVolume *fnosedownPhysi;
  G4VPhysicalVolume *fnoseinnerupPhysi;
  G4VPhysicalVolume *fnoseinnerdownPhysi;

  G4LogicalVolume *ffrontwindow_noseup_pvcLogic;
  G4VPhysicalVolume *ffrontwindow_noseup_pvcPhysi;
  G4LogicalVolume *ffrontwindow_nosedown_pvcLogic;
  G4VPhysicalVolume *ffrontwindow_nosedown_pvcPhysi;

  G4LogicalVolume *ffrontholeupLogic;
  G4VPhysicalVolume *ffrontholeupPhysi;

  G4LogicalVolume *ffrontholedownLogic;
  G4VPhysicalVolume *ffrontholedownPhysi;

  G4LogicalVolume *ffrontwindow_noseup_mylarLogic;
  G4VPhysicalVolume *ffrontwindow_noseup_mylarPhysi;
  G4LogicalVolume *ffrontwindow_nosedown_mylarLogic;
  G4VPhysicalVolume *ffrontwindow_nosedown_mylarPhysi;

  G4LogicalVolume *ffrontwindow_nose_upperhalf_tetlarLogic;
  G4VPhysicalVolume *ffrontwindow_nose_upperhalf_tetlarPhysi;
  G4LogicalVolume *ffrontwindow_nose_lowerhalf_tetlarLogic;
  G4VPhysicalVolume *ffrontwindow_nose_lowerhalf_tetlarPhysi;

  G4LogicalVolume *fnoseframeupLogic;
  G4LogicalVolume *fnoseframedownLogic;
  G4VPhysicalVolume *fnoseframeupPhysi;
  G4VPhysicalVolume *fnoseframedownPhysi;

  G4LogicalVolume *fnoseframeinnerupLogic;
  G4LogicalVolume *fnoseframeinnerdownLogic;
  G4VPhysicalVolume *fnoseframeinnerupPhysi;
  G4VPhysicalVolume *fnoseframeinnerdownPhysi;

 G4LogicalVolume *fmirrorLogic;
 G4VPhysicalVolume *fmirrorPhysi;

 G4LogicalVolume *steelframebox1Logic;
G4VPhysicalVolume  *steelframebox1Physi;

 G4LogicalVolume *steelframebox1copyLogic;
G4VPhysicalVolume  *steelframebox1copyPhysi;

 G4LogicalVolume *steelframebox2Logic;
G4VPhysicalVolume  *steelframebox2Physi;

 G4LogicalVolume *steelframebox2copyLogic;
G4VPhysicalVolume  *steelframebox2copyPhysi;


 G4LogicalVolume *steelframebox3Logic;
G4VPhysicalVolume  *steelframebox3Physi;

 G4LogicalVolume *steelframebox4Logic;
G4VPhysicalVolume  *steelframebox4Physi;

 G4LogicalVolume *steelframebox5Logic;
G4VPhysicalVolume  *steelframebox5Physi;

 G4LogicalVolume *steelframebox5copy1Logic;
G4VPhysicalVolume  *steelframebox5copy1Physi;

 G4LogicalVolume *steelframebox5copy2Logic;
G4VPhysicalVolume  *steelframebox5copy2Physi;

 G4LogicalVolume *steelframebox5copy3Logic;
G4VPhysicalVolume  *steelframebox5copy3Physi;

 G4LogicalVolume *steelframebox6Logic;
G4VPhysicalVolume  *steelframebox6Physi;

 G4LogicalVolume *steelframebox6copy1Logic;
G4VPhysicalVolume  *steelframebox6copy1Physi;

 G4LogicalVolume *steelframebox6copy2Logic;
G4VPhysicalVolume  *steelframebox6copy2Physi;

 G4LogicalVolume *steelframebox6copy3Logic;
G4VPhysicalVolume  *steelframebox6copy3Physi;

 G4LogicalVolume *steelframebox7Logic;
G4VPhysicalVolume  *steelframebox7Physi;

 G4LogicalVolume *steelframebox7copy1Logic;
G4VPhysicalVolume  *steelframebox7copy1Physi;

 G4LogicalVolume *steelframebox7copy2Logic;
G4VPhysicalVolume  *steelframebox7copy2Physi;

 G4LogicalVolume *steelframebox7copy3Logic;
G4VPhysicalVolume  *steelframebox7copy3Physi;

 G4LogicalVolume *steelframebox7copy4Logic;
G4VPhysicalVolume  *steelframebox7copy4Physi;

 G4LogicalVolume *steelframebox7copy5Logic;
G4VPhysicalVolume  *steelframebox7copy5Physi;

 G4LogicalVolume *steelframebox8Logic;
G4VPhysicalVolume  *steelframebox8Physi;

 G4LogicalVolume *steelframebox8copy1Logic;
G4VPhysicalVolume  *steelframebox8copy1Physi;

 G4LogicalVolume *steelframebox8copy2Logic;
G4VPhysicalVolume  *steelframebox8copy2Physi;

 G4LogicalVolume *steelframebox8copy3Logic;
G4VPhysicalVolume  *steelframebox8copy3Physi;

 G4LogicalVolume *steelframebox8copy4Logic;
G4VPhysicalVolume  *steelframebox8copy4Physi;

 G4LogicalVolume *steelframebox8copy5Logic;
G4VPhysicalVolume  *steelframebox8copy5Physi;

 G4LogicalVolume *mirrorframeboxLogic;
G4VPhysicalVolume  *mirrorframeboxPhysi;

 G4LogicalVolume *mirrorframetrapLogic;
G4VPhysicalVolume  *mirrorframetrapPhysi;

 G4LogicalVolume *mirrorframetrapcopyLogic;
G4VPhysicalVolume  *mirrorframetrapcopyPhysi;

 G4LogicalVolume *roundplateLogic;
G4VPhysicalVolume  *roundplatePhysi;

G4LogicalVolume *steelframebox9Logic;
G4VPhysicalVolume  *steelframebox9Physi;

G4LogicalVolume *steelframebox9copyLogic;
G4VPhysicalVolume  *steelframebox9copyPhysi;

G4LogicalVolume *fsteelframe_trapLogic;
G4VPhysicalVolume *fsteelframe_trapPhysi;

G4LogicalVolume *fsteelframe_trap2Logic;
G4VPhysicalVolume *fsteelframe_trap2Physi;

G4LogicalVolume *fsteelframe_trap3Logic;
G4VPhysicalVolume *fsteelframe_trap3Physi;

G4LogicalVolume *fsteelframe_trap4Logic;
G4VPhysicalVolume *fsteelframe_trap4Physi;


};

#endif
