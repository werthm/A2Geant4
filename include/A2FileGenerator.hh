// Abstract file-based event generator
// Author: Dominik Werthmueller, 2018

#ifndef A2FileGenerator_h
#define A2FileGenerator_h 1

#include <vector>

#include "G4ThreeVector.hh"

class G4ParticleDefinition;

class A2FileGenerator
{

public:
    struct A2GenParticle_t {
        G4ParticleDefinition* fDef; // particle definition (mass not used)
        G4ThreeVector fP;           // momentum [MeV]
        G4double fE;                // total energy [MeV]
        G4double fM;                // mass [MeV]
        G4ThreeVector fX;           // vertex [mm]
        G4double fT;                // vertex time [ns]
        G4bool fIsTrack;            // tracking flag
        A2GenParticle_t() : fDef(0), fP(0, 0, 0), fE(0),
                            fM(0), fX(0, 0, 0), fT(0), fIsTrack(false) { }
        void SetCorrectMass(G4bool usePDG = false);
        void Print(const char* pre = "") const;
    };

    enum EFileGenType {
        kNone,
        kMkin,
        kPluto,
        kPlutoCocktail,
        kGiBUU
    };

protected:
    EFileGenType fType;                     // type of file generator
    G4String fFileName;                     // input file name
    G4int fNEvents;                         // number of events
    G4double fWeight;                       // event weight
    A2GenParticle_t fBeam;                  // beam particle
    G4ThreeVector fVertex;                  // primary vertex [mm]
    std::vector<A2GenParticle_t> fPart;     // list of particles

public:
    A2FileGenerator(const char* filename, EFileGenType type);
    virtual ~A2FileGenerator() { }

    virtual G4bool Init() = 0;
    virtual G4bool ReadEvent(G4int event) = 0;
    virtual G4int GetMaxParticles() = 0;

    EFileGenType GetType() const { return fType; }
    const G4String& GetFileName() const { return fFileName; }
    G4int GetNEvents() const { return fNEvents; }
    G4double GetWeight() const { return fWeight; }
    const G4ThreeVector& GetVertex() const { return fVertex; }
    const A2GenParticle_t& GetBeam() const { return fBeam; }

    G4int GetNParticles() const { return fPart.size(); }
    const A2GenParticle_t& GetParticle(G4int p) const { return fPart[p]; }
    G4ParticleDefinition* GetParticleDefinition(G4int p) const { return fPart[p].fDef; }
    const G4ThreeVector& GetParticleMomentum(G4int p) const { return fPart[p].fP; }
    G4ThreeVector GetParticleDirection(G4int p) const { return G4ThreeVector(fPart[p].fP).unit(); }
    G4double GetParticleEnergy(G4int p) const { return fPart[p].fE; }
    G4double GetParticleKineticEnergy(G4int p) const { return fPart[p].fE - fPart[p].fM; }
    const G4ThreeVector& GetParticleVertex(G4int p) const { return fPart[p].fX; }
    G4double GetParticleTime(G4int p) const { return fPart[p].fT; }
    G4bool IsParticleTrack(G4int p) const { return fPart[p].fIsTrack; }

    void SetParticleIsTrack(G4int p, G4bool t = true);
    void SetWeight(G4double w) { fWeight = w; }

    void GenerateVertexCylinder(G4double t_length, G4double t_center,
                                G4double b_diam);

    void Print() const;
};

#endif

