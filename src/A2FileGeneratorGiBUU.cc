// event generator reading GiBUU ROOT files
// Author: Dominik Werthmueller, 2019

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

#include "TMath.h"
#include "TTree.h"

#include "A2FileGeneratorGiBUU.hh"

using namespace CLHEP;

const G4int A2FileGeneratorGiBUU::fgMaxParticles = 99;

//______________________________________________________________________________
A2FileGeneratorGiBUU::A2FileGeneratorGiBUU(const char* filename)
    : A2FileGeneratorTree(filename, kGiBUU, "RootTuple")
{
    // Constructor.

    // init members
    fReaderWeight = 0;
    fReaderCode = 0;
    fReaderPx = 0;
    fReaderPy = 0;
    fReaderPz = 0;
    fReaderE = 0;
    fReaderX = 0;
    fReaderY = 0;
    fReaderZ = 0;
}

//______________________________________________________________________________
A2FileGeneratorGiBUU::~A2FileGeneratorGiBUU()
{
    // Destructor.

}

//______________________________________________________________________________
G4bool A2FileGeneratorGiBUU::Init()
{
    // Init the file event reader.

    // call parent method
    if (!A2FileGeneratorTree::Init())
        return false;

    // link branches
    LinkBranch("weight", &fReaderWeight);
    LinkBranch("barcode", &fReaderCode);
    LinkBranch("Px", &fReaderPx);
    LinkBranch("Py", &fReaderPy);
    LinkBranch("Pz", &fReaderPz);
    LinkBranch("E", &fReaderE);
    if (fTree->GetBranch("x"))
    {
        LinkBranch("x", &fReaderX);
        LinkBranch("y", &fReaderY);
        LinkBranch("z", &fReaderZ);
    }

    return true;
}

//______________________________________________________________________________
G4bool A2FileGeneratorGiBUU::ReadEvent(G4int event)
{
    // Read the event 'event'.

    // call parent method
    if (!A2FileGeneratorTree::ReadEvent(event))
        return false;

    // clear particles
    fPart.clear();

    // event weight
    SetWeight(fReaderWeight);

    // loop over particles
    for (UInt_t i = 0; i < fReaderCode->size(); i++)
    {
        // look-up particle
        Int_t pdg = fReaderCode->at(i);
        G4ParticleDefinition* partDef = G4ParticleTable::GetParticleTable()->FindParticle(pdg);
        if (!partDef)
        {
            G4int Z, A, L, J;
            G4double E;
            if (G4IonTable::GetNucleusByEncoding(pdg, Z, A, L, E, J))
                partDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z, A, L, 0.0, J);
        }
        if (!partDef)
        {
            // user info
            G4cout << "A2FileGeneratorGiBUU::Init(): Undefined particle with PDG ID " << pdg
                   << " will not be tracked!" << G4endl;
            continue;
        }

        // check for off-shell particles
        Double_t e = 1000 * fReaderE->at(i);
        switch (partDef->GetPDGEncoding())
        {
            case  111: // pi0
            case  211: // pi+
            case -211: // pi+
                if (e < 138.0)
                    continue;
                else
                    break;
            case 2212: // proton
            case 2112: // neutron
                if (e < 938.0)
                    continue;
                else
                    break;
            default:
                G4cout << "A2FileGeneratorGiBUU::Init(): No off-shell cuts defined for particle " <<
                       partDef->GetParticleName() << G4endl;
        }

        // set event particle
        A2GenParticle_t part;
        part.fDef = partDef;
        part.fP.set(fReaderPx->at(i)*GeV, fReaderPy->at(i)*GeV, fReaderPz->at(i)*GeV);
        part.fE = fReaderE->at(i)*GeV;
        part.SetCorrectMass(true);
        part.fX = fVertex;
        part.fT = 0;
        part.fIsTrack = true;

        // add event particle
        if (!std::isnan(part.fP.mag()) && fPart.size() < fgMaxParticles)
            fPart.push_back(part);
    }

    return true;
}

//______________________________________________________________________________
G4int A2FileGeneratorGiBUU::GetMaxParticles()
{
    // Return the maximum number of particles.

    return fgMaxParticles;
}

