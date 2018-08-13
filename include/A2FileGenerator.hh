// Abstract file-based event generator
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGenerator_h
#define A2FileGenerator_h 1

#include <vector>

#include "G4ThreeVector.hh"

class G4ParticleDefinition;

class A2FileGenerator
{

protected:
    struct A2GenParticle_t {
        G4ParticleDefinition* fDef; // particle definition (mass not used)
        G4ThreeVector fP;           // momentum
        G4double fE;                // total energy
        G4double fM;                // mass
        G4ThreeVector fX;           // vertex
        G4double fT;                // vertex time
        G4bool fIsTrack;            // tracking flag
        A2GenParticle_t() : fDef(0), fP(0, 0, 0), fE(0),
                            fM(0), fX(0, 0, 0), fT(0), fIsTrack(false) { }
        void Print(const char* pre = "");
    };

    G4String fFileName;                     // input file name
    A2GenParticle_t fBeam;                  // beam particle
    G4ThreeVector fVertex;                  // primary vertex
    std::vector<A2GenParticle_t> fPart;     // list of particles

public:
    A2FileGenerator(const char* filename);
    virtual ~A2FileGenerator() { }

    virtual G4bool Init() = 0;
    virtual G4bool ReadEvent(G4int event) = 0;

    G4int GetNParticles() const { return fPart.size(); }
    G4ParticleDefinition* GetParticleDefinition(G4int p) const { return fPart[p].fDef; }
    G4ThreeVector GetParticleDirection(G4int p) const { return G4ThreeVector(fPart[p].fP).unit(); }
    G4double GetParticleKineticEnergy(G4int p) const { return fPart[p].fE - fPart[p].fM; }
    G4bool IsParticleTrack(G4int p) const { return fPart[p].fIsTrack; }

    void SetParticleIsTrack(G4int p, G4bool t = true) { fPart[p].fIsTrack = t; }

    void Print();
};

#endif

