A2Geant4
========

A2 Geant4 simulation

## Table of contents
* [Release notes](#release-notes)
* [Installation](#installation)
* [Quick start](#quick-start)
* [Simulation commands](#simulation-commands)
* [Detector setup commands](#detector-setup-commands)

## Release notes

### 0.2.0beta
* generalized interface for input event-files
* added tracked particles to metadata

### 0.1.0
* fixed bug related to compiler optimizations for Geant4 >= 10.3 (S. Gardner)
* G4ExtrudedSolid-based CB-crystal implementation for Geant4 >= 10.4 (S. Gardner)
* informative metadata in output file
* nicer tracking progress information
* better support for different Geant4 releases

### pre-0.1.0
* storage of primary particle IDs for CB/TAPS hits
* PID III (S. Kay)
* Cherenkov detector (F. Afzal)
* support for xz-compressed magnetic field maps
* Pizza detector
* support for cryogenic He-3 target
* possibility to disable MWPC hit storage

## Installation

### Dependencies
* Geant4 10.2/10.3/10.4
* ROOT 5 or 6
* CMake 2.6

### Installation

#### Method 1: Getting the git master branch
```
cd /some/directory
git clone https://github.com/werthm/A2Geant4.git
cd A2Geant4
mkdir build
cd build
cmake ..
make -j
```

#### Method 2: Install a release
Download a [release](releases), unpack and compile it:
```
cd /some/directory
wget https://github.com/werthm/A2Geant4/archive/v0.1.0.tar.gz
tar xvfz v0.1.0.tar.gz
cd A2Geant4-0.1.0
mkdir build
cd build
cmake ..
make -j
```

## Quick start

### Interactive GUI mode
```
build/A2Geant4 --gui
```

### Batch-mode
```
build/A2Geant4 --mac=macros/your_macro.mac --det=macros/DetectorSetup.mac --if=input.root --of=output.root
```

## Simulation commands

### Physics
Command                            | Meaning
:--------------------------------- |:-------
`/A2/physics/Physics QGSP_BERT`    | select physics list
`/A2/physics/ListPhysics`          | show available physics lists
`/A2/physics/SetRegion CB`         | select region for tracking cut (CB, TAPS, TAPSV, PID, MWPC, Pizza)
`/A2/physics/RegionCut 0.1 mm`     | set tracking cut for selected region
`/A2/physics/CutGamma 0.1 mm`      | set tracking cut for photons
`/A2/physics/CutEl 0.1 mm`         | set tracking cut for electrons
`/A2/physics/CutPos 0.1 mm`        | set tracking cut for positrons
`/A2/physics/CutProt 0.1 mm`       | set tracking cut for protons
`/A2/physics/CutsAll 0.1 mm`       | set the same tracking cut for photons, electrons, positrons and protons

### Generator
Command                                | Meaning
:------------------------------------- |:-------
`/A2/generator/Seed 3243434`           | set the seed of the random number generator
`/A2/generator/NToBeTracked 3`         | set the number of particles to be tracked
`/A2/generator/Track 1`                | set the index of a particle to be tracked
`/A2/generator/InputFile input.root`   | set the event input file (sets mode to 2)
`/A2/generator/Mode 1`                 | select generator mode (0=G4 CLI generator, 1=phase-space, 2=file input, 3=overlap debug)
`/A2/generator/SetTMin 200 MeV`        | minimum kinetic energy for a particle in the phase-space generator
`/A2/generator/SetTMax 450 MeV`        | maximum kinetic energy for a particle in the phase-space generator
`/A2/generator/SetThetaMin 0 deg`      | minimum polar angle for a particle in the phase-space generator
`/A2/generator/SetThetaMax 120 deg`    | maximum polar angle for a particle in the phase-space generator
`/A2/generator/SetBeamXSigma 10 mm`    | x-sigma of incoming beam
`/A2/generator/SetBeamYSigma 10 mm`    | y-sigma of incoming beam
`/A2/generator/SetTargetZ0 0 mm`       | target z-position
`/A2/generator/SetTargetThick 0.4 mm`  | target length
`/A2/generator/SetTargetRadius 0.5 cm` | target radius
`/A2/generator/SetSplitTheta 5 deg`    | theta split-angle for overlap generator

### Event-saver
Command                              | Meaning
:----------------------------------- |:-------
`/A2/event/setOutputFile ouput.root` | set the tracked-event output file
`/A2/event/storePrimaries false`     | disable storage of primary particle indices for CB/TAPS hits

## Detector setup commands

### Crystal Ball
Command                            | Meaning
:--------------------------------- |:-------
`/A2/det/useCB 1`                  | use CB (0=off, 1=on)
`/A2/det/setHemiGap 0.4 0.4 -1 cm` | upper air gap, lower air gap, geometry (>0: Prakhov, <0: old)
`/A2/det/setCBCrystGeo extr`       | CB crystal geometry (trap=G4Trap, extr=G4ExtrudedSolid (default for Geant4 >= 10.4))

### TAPS
Command                               | Meaning
:------------------------------------ |:-------
`/A2/det/useTAPS 1`                   | use TAPS (0=off, 1=on)
`/A2/det/setTAPSFile data/taps07.dat` | location of TAPS geometry file (taps07.dat, taps.dat)
`/A2/det/setTAPSZ 146.35 cm`          | distance target-TAPS
`/A2/det/setTAPSN 384`                | number of TAPS crystals (384, 510)
`/A2/det/setTAPSPbWO4Rings 2`         | number of PbWO4 rings (1, 2)

### PID
Command                 | Meaning
:-----------------------|:-------
`/A2/det/usePID 2`      | use PID (0=off, 1=PID I, 2=PID II, 3=PID III)
`/A2/det/setPIDZ 0. cm` | PID z-shift

### MWPC
Command                 | Meaning
:-----------------------|:-------
`/A2/det/useMWPC 2`     | use MWPC (0=off, 1=without anode wires, 2=with anode wires, 10/20: without/with wires but no readout)

### Cherenkov
Command                  | Meaning
:------------------------|:-------
`/A2/det/useCherenkov 0` | use Cherenkov detector (0=off, 1=on)

### TOF-walls
Command                           | Meaning
:---------------------------------|:-------
`/A2/det/useTOF 0`                | use TOF-walls (0=off, 1=on)
`/A2/det/setTOFFile data/TOF.par` | location of TOF-walls geometry file

### Pizza detector
Command                             | Meaning
:-----------------------------------|:-------
`/A2/det/usePizza 0`                | use the Pizza detector (0=off, 1=on)
`/A2/det/setPizzaZ 162 cm`          | distance target-Pizza detector

### Cryogenic Targets
Command                          | Meaning
:------------------------------- |:-------
`/A2/det/useTarget Cryo`         | standard hydrogen target
`/A2/det/useTarget Cryo2`        | narrower hydrogen target
`/A2/det/useTarget CryoHe3`      | He3/He3 target
`/A2/det/targetMaterial G4_lH2`  | hydrogen
`/A2/det/targetMaterial A2_lD2`  | deuterium
`/A2/det/targetMaterial A2_lHe3` | He-3
`/A2/det/targetMaterial A2_lHe4` | He-4

### Solid Targets
Command                              | Meaning
:----------------------------------- |:-------
`/A2/det/useTarget Solid`            | solid target
`/A2/det/targetMaterial G4_Li`       | Lithium
`/A2/det/targetMaterial G4_GRAPHITE` | Graphite
`/A2/det/targetMaterial G4_Ca`       | Calcium
`/A2/det/targetMaterial G4_Pb`       | Lead

### Polarized Targets
Command                                        | Meaning
:--------------------------------------------- |:-------
`/A2/det/useTarget Polarized`                  | solid target
`/A2/det/targetMaterial A2_HeButanol`          | Butanol/Helium mix
`/A2/det/targetMaterial A2_HeDButanol`         | D-Butanol/Helium mix
`/A2/det/targetMagneticCoils Solenoidal`       | longitudinally polarized target
`/A2/det/targetMagneticCoils Saddle`           | transversely polarized target
`/A2/det/setTargetMagneticFieldMap map.dat.xz` | magnetic field map (data/wouter_field_map.dat.xz, data/field_map_jul_13_pos.dat.xz)

### General Target Options
Command                        | Meaning
:----------------------------- |:-------
`/A2/det/setTargetLength 2 cm` | set target length

