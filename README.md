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

