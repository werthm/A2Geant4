// event generator reading Pluto-files
// Author: Dominik Werthmueller, 2018-2019

#ifdef WITH_PLUTO

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

#include "TTreeReader.h"

#include "A2FileGeneratorPluto.hh"

using namespace CLHEP;

const G4int A2FileGeneratorPluto::fgMaxParticles = 99;
const G4int A2FileGeneratorPluto::fgPlutoG4Conversion[70] =
{
    // from Pluto/src/PStdData.cc
    /* 0: dummy     */   0,            /* 1: Photon    */  22,
    /* 2: Positron  */ -11,            /* 3: Electron  */  11,
    /* 4: Neutrino  */  12,            /* 5: mu+       */ -13,
    /* 6: mu-       */  13,            /* 7: pi0       */ 111,
    /* 8: pi+       */ 211,            /* 9: pi-       */-211,
    /*10: K0 long   */ 130,            /*11: K+        */ 321,
    /*12: K-        */-321,            /*13: Neutron   */2112,
    /*14: Proton    */2212,            /*15: Antiproton*/-2212,
    /*16: K0 short  */ 310,            /*17: eta       */ 221,
    /*18: Lambda    */3122,            /*19: Sigma+    */3222,
    /*20: Sigma0    */3212,            /*21: Sigma-    */3112,
    /*22: Xi0       */3322,            /*23: Xi-       */3312,
    /*24: Omega-    */3334,            /*25: Antineutrn*/-2112,
    /*26: Antilambda*/-3122,           /*27: Antisigma-*/-3112,
    /*28: Antisigma0*/-3212,           /*29: Antisigma+*/-3222,
    /*30: Antixi0   */-3322,           /*31: Antixi+   */-3312,
    /*32: Antiomega+*/-3334,           /*33: File      */  0,
    /*34: Delta0    */2114,            /*35: Delta++   */2224,
    /*36: Delta+    */2214,            /*37: Delta-    */1114,
    /*38: NP11+     */  0,             /*39: ND13+     */  0,
    /*40: NS11+     */  0,             /*41: rho0      */ 113,
    /*42: rho+      */ 213,            /*43: rho-      */-213,
    /*44: NULL      */  0,             /*45: Deuteron  */  1000010020,
    /*46: Tritium   */  1000010030,    /*47: Alpha     */  1000020040,
    /*48: NULL      */  0,             /*49: He3       */  1000020030,
    /*50: dimuon    */  0,             /*51: dilepton  */  0,
    /*52: omega     */ 223,            /*53: eta'      */ 331,
    /*54: sigma     */  0,             /*55: phi       */ 333,
    /*56: Delta0*P33*/  0,             /*57: Delta++ *P33*/0,
    /*58: Delta+*P33*/  0,             /*59: Delta- *P33 */0,
    /*60: Delta0*S31*/  0,             /*61: Delta++ *S31*/0,
    /*62: Delta+*S31*/  0,             /*63: Delta- *S31 */0,
    /*64: NP110     */  0,             /*65: ND130     */  0,
    /*66: NS110     */  0,             /*67: J/Psi     */ 443,
    /*68: Psi'      */  0,             /*69: pn        */  0
};

//______________________________________________________________________________
A2FileGeneratorPluto::A2FileGeneratorPluto(const char* filename)
    : A2FileGeneratorTree(filename, kPluto, "data")
{
    // Constructor.

    // init members
    fReader = 0;
    fReaderPart = 0;
}

//______________________________________________________________________________
A2FileGeneratorPluto::~A2FileGeneratorPluto()
{
    // Destructor.

    if (fReaderPart)
        delete fReaderPart;
    if (fReader)
        delete fReader;
}

//______________________________________________________________________________
G4bool A2FileGeneratorPluto::Init()
{
    // Init the file event reader.

    // call parent method
    if (!A2FileGeneratorTree::Init())
        return false;

    // create tree reader
    fReader = new TTreeReader(fTree);

    // create particle reader array
    fReaderPart = new TTreeReaderArray<PParticle>(*fReader, "Particles");

    // check for cocktail
    if (fTree->GetMinimum("Npart") != fTree->GetMaximum("Npart"))
        fType = kPlutoCocktail;

    return true;
}

//______________________________________________________________________________
G4bool A2FileGeneratorPluto::ReadEvent(G4int event)
{
    // Read the event 'event'.

    // read event
    if (!fReader->Next())
        return false;

    // clear particles
    fPart.clear();

    // loop over particles
    for (UInt_t i = 0; i < fReaderPart->GetSize(); i++)
    {
        // get pluto particle
        const PParticle& ppart = fReaderPart->At(i);

        // set event particle
        A2GenParticle_t part;
        part.fDef = PlutoToG4(ppart.ID());
        part.fP.set(ppart.Px()*GeV, ppart.Py()*GeV, ppart.Pz()*GeV);
        part.fE = ppart.E()*GeV;
        part.SetCorrectMass();
        part.fX.set(fVertex.x() + ppart.X()*mm,
                    fVertex.y() + ppart.Y()*mm,
                    fVertex.z() + ppart.Z()*mm);
        part.fT = ppart.T() * ns;

        // check for stable particles to be tracked
        if (ppart.ID() < 1000 && ppart.GetDaughterIndex() == -1)
        {
            part.fIsTrack = true;
        }
        // check for pseudo beam-particle
        else if (ppart.ID() > 1000)
        {
            // extract beam and target indices
            //G4int beam_id = fPartID[i] % 1000;
            G4int target_id = ppart.ID() / 1000;

            // get target mass (assume target at rest)
            G4ParticleDefinition* target_def = PlutoToG4(target_id);
            G4double target_mass = 0;
            if (target_def)
            {
                target_mass = target_def->GetPDGMass() / 1000;
            }
            else
            {
                G4cout << "A2FileGeneratorPluto::ReadEvent(): Unknown ID of target particle ("
                       << target_id << ")" << G4endl;
            }

            // set beam (assume photon beam);
            fBeam.fDef = G4ParticleTable::GetParticleTable()->FindParticle(22);
            fBeam.fP.set(ppart.Px()*GeV, ppart.Py()*GeV, ppart.Pz()*GeV);
            fBeam.fE = (ppart.E() - target_mass)*GeV;
            fBeam.fM = 0;
            fBeam.fIsTrack = false;
        }

        // add event particle
        fPart.push_back(part);
    }

    return true;
}

//______________________________________________________________________________
G4ParticleDefinition* A2FileGeneratorPluto::PlutoToG4(Int_t id)
{
    // Convert a particle with Pluto ID 'id' to a Geant4 particle definition.

    // check for valid Pluto particle ID range
    if (id >= 0 && id < 70)
    {
        G4ParticleDefinition* partDef = G4ParticleTable::GetParticleTable()->FindParticle(fgPlutoG4Conversion[id]);
        if (!partDef)
        {
            G4int Z, A, L, J;
            G4double E;
            if (G4IonTable::GetNucleusByEncoding(fgPlutoG4Conversion[id], Z, A, L, E, J))
                partDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z, A, L, 0.0, J);
        }
        return partDef;
    }
    else
        return 0;
}

//______________________________________________________________________________
G4int A2FileGeneratorPluto::GetMaxParticles()
{
    // Return the maximum number of particles.

    return fTree->GetMaximum("Npart");
}

#endif

