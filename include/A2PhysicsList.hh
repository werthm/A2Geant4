// Wrapper for physics list classes
// Author: Dominik Werthmueller, 2018

#include "G4Version.hh"

#if G4VERSION_NUMBER >= 1000 && G4VERSION_NUMBER < 1030
#include "A2PhysicsList_10.hh"
#elif G4VERSION_NUMBER >= 1030 && G4VERSION_NUMBER < 1050
#include "A2PhysicsList_10_3.hh"
#elif G4VERSION_NUMBER >= 1050
#include "A2PhysicsList_10_5.hh"
#endif

