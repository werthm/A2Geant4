// Abstract file-based event generator
// Author: Dominik Werthmueller, 2018

#include "TMath.h"

#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

#include "A2FileGenerator.hh"

//______________________________________________________________________________
A2FileGenerator::A2FileGenerator(const char* filename, EFileGenType type)
{
    // Constructor.

    // init members
    fType = type;
    fFileName = filename;
    fNEvents = 0;
    fWeight = 1;
}

//______________________________________________________________________________
void A2FileGenerator::A2GenParticle_t::SetCorrectMass(G4bool usePDG)
{
    // Init the mass of the particle based on the particle definition and
    // the 4-momentum.

    if (usePDG && fDef)
    {
        // set mass
        fM = fDef->GetPDGMass();

        //printf("mass of %s is %.2f\n", fDef->GetParticleName().c_str(), TMath::Sqrt(fE*fE - fP.mag2()));

        // recalculate momentum
        G4double p_old = fP.mag();
        G4double p_new = TMath::Sqrt(fE*fE - fM*fM);
        fP.set(fP.x()/p_old*p_new, fP.y()/p_old*p_new, fP.z()/p_old*p_new);
    }
    else
    {
        // check for defined massless particles
        if (fDef && fDef->GetPDGMass() == 0)
            fM = 0;
        else
            fM = TMath::Sqrt(fE*fE - fP.mag2());
    }
}

//______________________________________________________________________________
void A2FileGenerator::A2GenParticle_t::Print(const char* pre) const
{
    // Print the content of this class.

    G4cout << pre << "Name                : " << (fDef ? fDef->GetParticleName() : "unknown") << G4endl
           << pre << "Momentum            : " << fP << G4endl
           << pre << "Energy              : " << fE << G4endl
           << pre << "Mass                : " << fM << G4endl
           << pre << "Vertex              : " << fX << G4endl
           << pre << "Vertex time         : " << fT << G4endl
           << pre << "Is tracked          : " << (fIsTrack ? "yes" : "no") << G4endl;
}

//______________________________________________________________________________
void A2FileGenerator::SetParticleIsTrack(G4int p, G4bool t)
{
    // Set the tracking flag for the particle 'p'.

    // check if particle can be tracked
    if (t && !fPart[p].fDef)
        return;

    fPart[p].fIsTrack = t;
}

//______________________________________________________________________________
void A2FileGenerator::GenerateVertexCylinder(G4double t_length, G4double t_center,
                                             G4double b_diam)
{
    // Create a random vertex within the target-beam cylinder.

    // randomize the vertex within the beam spot and the target
    Double_t vX = 1e10;
    Double_t vY = 1e10;
    Double_t beamRad = b_diam / 2.;
    while (vX*vX+vY*vY > beamRad*beamRad)
    {
        vX = beamRad * (2. * G4UniformRand() - 1.);
        vY = beamRad * (2. * G4UniformRand() - 1.);
    }
    Double_t vZ = t_length / 2. * (2. * G4UniformRand() - 1.) + t_center;
    fVertex.set(vX, vY, vZ);
}

//______________________________________________________________________________
void A2FileGenerator::Print() const
{
    // Print the content of this class.

    G4String type;
    if (fType == kMkin)
        type = "Mkin";
    else if (fType == kPluto)
        type = "Pluto";
    else if (fType == kPlutoCocktail)
        type = "Pluto Cocktail";
    else if (fType == kGiBUU)
        type = "GiBUU";
    else
        type = "Unknown";

    G4cout << "Generator type      : " << type << G4endl
           << "File name           : " << fFileName << G4endl
           << "Number of events    : " << fNEvents << G4endl
           << "Event weight        : " << fWeight << G4endl
           << "Number of particles : " << fPart.size() << G4endl
           << "Primary vertex      : " << fVertex << G4endl
           << "Beam particle" << G4endl;
    fBeam.Print("  ");
    for (G4int i = 0; i < (G4int)fPart.size(); i++)
    {
        G4cout << "Particle " << i+1 << G4endl;
        fPart[i].Print("  ");
    }
}

