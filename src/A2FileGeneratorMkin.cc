// event generator reading mkin-files
// Author: Dominik Werthmueller, 2018

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include "TTree.h"

#include "A2FileGeneratorMkin.hh"
#include "MCNtuple.h"

using namespace CLHEP;

const G4int A2FileGeneratorMkin::fgMaxParticles = 99;

//______________________________________________________________________________
A2FileGeneratorMkin::A2FileGeneratorMkin(const char* filename)
    : A2FileGeneratorTree(filename, kMkin, "h1")
{
    // Constructor.

    // init members
    fNPart = 0;
    fPartPBr = new Float_t*[fgMaxParticles];
    fPartVBr = new Float_t*[fgMaxParticles];
    for (Int_t i = 0; i < fgMaxParticles; i++)
    {
        fPartPBr[i] = 0;
        fPartVBr[i] = 0;
    }
}

//______________________________________________________________________________
A2FileGeneratorMkin::~A2FileGeneratorMkin()
{
    // Destructor.

    if (fPartPBr)
    {
        for (G4int i = 0; i < fNPart; i++)
        {
            if (fPartPBr[i])
                delete [] fPartPBr[i];
            if (fPartVBr[i])
                delete [] fPartVBr[i];
        }
        delete [] fPartPBr;
        delete [] fPartVBr;
    }
}

//______________________________________________________________________________
G4bool A2FileGeneratorMkin::Init()
{
    // Init the file event reader.

    // call parent method
    if (!A2FileGeneratorTree::Init())
        return false;

    // link vertex
    LinkBranch("X_vtx", &fVertexBr[0]);
    LinkBranch("Y_vtx", &fVertexBr[1]);
    LinkBranch("Z_vtx", &fVertexBr[2]);

    // link beam, assume photon beam
    LinkBranch("Px_bm", &fBeamBr[0]);
    LinkBranch("Py_bm", &fBeamBr[1]);
    LinkBranch("Pz_bm", &fBeamBr[2]);
    LinkBranch("Pt_bm", &fBeamBr[4]);
    LinkBranch("En_bm", &fBeamBr[3]);
    fBeam.fDef = G4ParticleTable::GetParticleTable()->FindParticle(22);
    fBeam.fM = 0;
    fBeam.fIsTrack = false;

    // number of branches
    G4int nbr = fTree->GetNbranches();

    // read particles
    fNPart = 0;
    TString prefix;
    TObjArray* objarray = fTree->GetListOfBranches();
    for (G4int i = 0; i < fgMaxParticles; i++)
    {
        // format branch-prefix for particle i
        prefix.Form("Px_l%02d", i+1);

        // loop over branches
        for (G4int j = 0; j < nbr; j++)
        {
            // get branch name
            TBranch* branch = (TBranch*)objarray->At(j);
            TString bname = TString(branch->GetName());

            // check branch name
            if (bname.BeginsWith(prefix))
            {
                // extract GEANT3 ID
                TString g3_id_str(bname);
                g3_id_str.ReplaceAll(prefix, "");
                G4int g3_id = g3_id_str.Atoi();

                // create array
                fPartPBr[fNPart] = new Float_t[5];

                // link particle momenta branches
                LinkBranch(TString::Format("Px_l%02d%02d", i+1, g3_id).Data(), &fPartPBr[fNPart][0]);
                LinkBranch(TString::Format("Py_l%02d%02d", i+1, g3_id).Data(), &fPartPBr[fNPart][1]);
                LinkBranch(TString::Format("Pz_l%02d%02d", i+1, g3_id).Data(), &fPartPBr[fNPart][2]);
                LinkBranch(TString::Format("Pt_l%02d%02d", i+1, g3_id).Data(), &fPartPBr[fNPart][4]);
                LinkBranch(TString::Format("En_l%02d%02d", i+1, g3_id).Data(), &fPartPBr[fNPart][3]);

                // link particle vertex branches if available
                if (fTree->GetBranch(TString::Format("Vx_l%02d%02d", i+1, g3_id).Data()))
                {
                    fPartVBr[fNPart] = new Float_t[4];
                    LinkBranch(TString::Format("Vx_l%02d%02d", i+1, g3_id).Data(), &fPartVBr[fNPart][0], false);
                    LinkBranch(TString::Format("Vy_l%02d%02d", i+1, g3_id).Data(), &fPartVBr[fNPart][1], false);
                    LinkBranch(TString::Format("Vz_l%02d%02d", i+1, g3_id).Data(), &fPartVBr[fNPart][2], false);
                    LinkBranch(TString::Format("Vt_l%02d%02d", i+1, g3_id).Data(), &fPartVBr[fNPart][3], false);
                    G4cout << "A2FileGeneratorMkin::Init(): Found vertex branches for particle " << g3_id <<
                           " with index " << fNPart+1 << G4endl;
                }

                // look-up particle
                G4ParticleDefinition* partDef = G4ParticleTable::GetParticleTable()->FindParticle(GetPDGfromG3(g3_id));
                if (!partDef && g3_id != 0)
                {
                    G4int Z, A, L, J;
                    G4double E;
                    if (G4IonTable::GetNucleusByEncoding(GetPDGfromG3(g3_id), Z, A, L, E, J))
                        partDef = G4ParticleTable::GetParticleTable()->GetIonTable()->GetIon(Z, A, L, 0.0, J);
                }

                // add particle
                if (partDef)
                {
                    // kaon0S bugfix
                    if (g3_id == 16 && partDef->GetPDGEncoding() == 130)
                        partDef = G4ParticleTable::GetParticleTable()->FindParticle(310);

                    // user info
                    G4cout << "A2FileGeneratorMkin::Init(): Adding a " << partDef->GetParticleName()
                           << " (Geant3 ID: " << g3_id << ", PDG ID: " << partDef->GetPDGEncoding() << ")"
                           << " as index " << fNPart+1 << G4endl;
                }
                else
                {
                    // user info
                    G4cout << "A2FileGeneratorMkin::Init(): Undefined particle (GEANT3 ID: " << g3_id
                           << ", PDG ID: " << GetPDGfromG3(g3_id) << "), check MCNtuple.h!" << G4endl;
                    G4cout << "Particle with index " << fNPart+1 << " cannot be tracked!" << G4endl;
                }

                // add event particle
                A2GenParticle_t part;
                part.fDef = partDef;
                fPart.push_back(part);

                // try next particle
                fNPart++;
                break;
            }
        }
    }

    return true;
}

//______________________________________________________________________________
G4bool A2FileGeneratorMkin::ReadEvent(G4int event)
{
    // Read the event 'event'.

    // call parent method
    if (!A2FileGeneratorTree::ReadEvent(event))
        return false;

    // set vertex event data
    fVertex.set(fVertexBr[0]*cm,
                fVertexBr[1]*cm,
                fVertexBr[2]*cm);

    // set beam event data
    fBeam.fP.set(fBeamBr[0]*fBeamBr[4]*GeV,
                 fBeamBr[1]*fBeamBr[4]*GeV,
                 fBeamBr[2]*fBeamBr[4]*GeV);
    fBeam.fE = fBeamBr[3]*GeV;

    // set particle event data
    for (G4int i = 0; i < fNPart; i++)
    {
        // momentum, energy, mass
        fPart[i].fP.set(fPartPBr[i][0]*fPartPBr[i][4]*GeV,
                        fPartPBr[i][1]*fPartPBr[i][4]*GeV,
                        fPartPBr[i][2]*fPartPBr[i][4]*GeV);
        fPart[i].fE = fPartPBr[i][3]*GeV;
        fPart[i].SetCorrectMass();

        // vertex
        if (fPartVBr[i])
        {
            fPart[i].fX.set(fPartVBr[i][0]*cm,
                            fPartVBr[i][1]*cm,
                            fPartVBr[i][2]*cm);
            fPart[i].fT = fPartVBr[i][3]*ns;
        }
        else
        {
            fPart[i].fX = fVertex;
            fPart[i].fT = 0;
        }

        // tracking flag
        if (fPart[i].fDef && fPart[i].fDef->GetPDGStable())
            fPart[i].fIsTrack = true;
    }

    return true;
}

//______________________________________________________________________________
G4int A2FileGeneratorMkin::GetMaxParticles()
{
    // Return the maximum number of particles.

    return fNPart;
}

