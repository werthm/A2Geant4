
#include "A2Hit.hh"
#include "G4Color.hh"
#include "G4VisAttributes.hh"

G4Allocator<A2Hit> A2HitAllocator;


A2Hit::A2Hit()
{
  fEdep=0;
  fPos.setRThetaPhi(0,0,0);
  fID=0;
  fTime=0;
}


A2Hit::~A2Hit()
{ ;}


A2Hit::A2Hit(const A2Hit& right)
  :G4VHit()
{
  fEdep=right.fEdep;
  fPos+right.fPos;
  fID=right.fID;
  fTime=right.fTime;
  fPartE=right.fPartE;
}


const A2Hit& A2Hit::operator=(const A2Hit& right)
{

  fEdep=right.fEdep;
  fPos+right.fPos;
  fID=right.fID;
  fTime=right.fTime;
  fPartE=right.fPartE;
  return *this;
}


//int A2Hit::operator==(const A2Hit& right) const
//{
//  return 0;
//}


void A2Hit::Draw()
{

}

G4int A2Hit::GetNParticles()
{
    // Return the number of contributing particles.

    G4int n = 0;
    for (size_t i = 0; i < fPartE.size(); i++)
        if (fPartE[i] > 0) n++;
    return n;
}

G4int A2Hit::GetParticle()
{
    // Return the index of the particle with the highest
    // energy deposition.

    G4int p = 0;
    G4double emax = 0;
    for (size_t i = 0; i < fPartE.size(); i++)
    {
        if (fPartE[i] > emax)
        {
            p = i+1;
            emax = fPartE[i];
        }
    }
    return p;
}

void A2Hit::AddPartEnergy(G4int p, G4double energy)
{
    // resize list if necessary
    if (p > (G4int)fPartE.size())
        fPartE.resize(p, 0.0);

    //G4cout << "Hit " << fID << " adding " << energy
    //       << " by particle " << p
    //       << " now at " << fEdep<< G4endl;

    // add energy
    fPartE[p-1] += energy;
}

void A2Hit::Print()
{
    G4cout << "A2Hit: "
           << " fEdep: " << fEdep
           << " fID: " << fID
           << " fTime: " << fTime
           << " part: " << GetParticle()
           << " #part: " << GetNParticles()
           << " fPartE: ";
    for (size_t i = 0; i < fPartE.size(); i++)
        G4cout << fPartE[i] << "(" << i+1 << ") ";

    G4cout << G4endl;
}

