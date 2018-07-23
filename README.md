# A2Geant4

A2 Geant4 simulation

### Recently added features
* storage of primary particle IDs for CB/TAPS hits
* PID III (S. Kay)
* Cherenkov detector (F. Afzal)
* support for xz-compressed magnetic field maps
* Pizza detector
* support for cryogenic He-3 target
* possibility to disable MWPC hit storage

### Installation

#### Dependencies
* Geant4 10.2 (10.3 seems to give erratic results)
* ROOT 5 or 6
* CMake 2.6

#### Installation
```
cd /some/directory
git clone https://github.com/werthm/A2Geant4.git
cd A2Geant4
mkdir build
cd build
cmake ..
make -j
```

### Documentation

#### Cryogenic Targets

| `/A2/det/useTarget Cryo`         | standard hydrogen target |
| `/A2/det/useTarget Cryo2`        | narrower hydrogen target |
| `/A2/det/targetMaterial G4_lH2`  | hydrogen                 |
| `/A2/det/targetMaterial A2_lD2`  | deuterium                |
| `/A2/det/targetMaterial A2_lHe3` | He-3                     |
| `/A2/det/targetMaterial A2_lHe4` | He-4                     |
| `/A2/det/useTarget CryoHe3`      | He3/He3 target           |

#### Solid Targets

| `/A2/det/useTarget Solid`            | solid target |
| `/A2/det/targetMaterial G4_Li`       | Lithium      |
| `/A2/det/targetMaterial G4_GRAPHITE` | Graphite     |
| `/A2/det/targetMaterial G4_Ca`       | Calcium      |
| `/A2/det/targetMaterial G4_Pb`       | Lead         |

#### Polarized Targets

| `/A2/det/useTarget Polarized`                  | solid target                    |
| `/A2/det/targetMaterial A2_HeButanol`          | Butanol/Helium mix              |
| `/A2/det/targetMaterial A2_HeDButanol`         | D-Butanol/Helium mix            |
| `/A2/det/targetMagneticCoils Solenoidal`       | longitudinally polarized target |
| `/A2/det/targetMagneticCoils Saddle`           | transversely polarized target   |
| `/A2/det/setTargetMagneticFieldMap map.dat.xz` | magnetic field map              |

#### General Target Options
Command                        | Meaning
:----------------------------- | :------
`/A2/det/setTargetLength 2 cm` | set target length


