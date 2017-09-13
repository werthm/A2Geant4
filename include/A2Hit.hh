
#ifndef A2Hit_h
#define A2Hit_h 1

#include <deque>

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"



class A2Hit : public G4VHit
{
public:

  A2Hit();
  ~A2Hit();
  A2Hit(const A2Hit&);
  const A2Hit& operator=(const
						A2Hit&);
  //int operator==(const A2Hit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  virtual void Draw();
  void Print();

protected:

  G4double fEdep;  // Energy deposited in detector
  G4ThreeVector fPos; // Position of the hit (in what frame?)
  G4int fID; // ID of detector hit
  G4double fTime; // global time of hit
  std::deque<G4double> fPartE; // energies deposited by primary particles

public:

  void AddEnergy(G4double de) {fEdep += de;};
  void SetPos(G4ThreeVector pos) {fPos=pos;};
  void SetID(G4int i) {fID = i;};
  void SetTime(G4double t) {fTime = t;};
  void AddPartEnergy(G4int p, G4double energy);

  G4double GetEdep() { return fEdep; };
  G4ThreeVector GetPos() { return fPos; };
  G4int GetID() { return fID; };
  G4double GetTime() { return fTime; };
  G4int GetNParticles();
  G4int GetParticle();
};


typedef G4THitsCollection<A2Hit> A2HitsCollection;

extern G4Allocator<A2Hit> A2HitAllocator;


inline void* A2Hit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) A2HitAllocator.MallocSingle();
  return aHit;
}


inline void A2Hit::operator delete(void* aHit)
{
  A2HitAllocator.FreeSingle((A2Hit*) aHit);
}

#endif

