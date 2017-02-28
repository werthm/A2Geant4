#include <fstream>
#include "TString.h"
#include "A2MagneticField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "CLHEP/Units/SystemOfUnits.h"

using namespace CLHEP;

//______________________________________________________________________________________________________
// G4FieldManager*  A2MagneticField::GetGlobalFieldManager()
// {
//   return G4TransportationManager::GetTransportationManager()->GetFieldManager();
// }

//______________________________________________________________________________________________________
A2MagneticField::A2MagneticField()
{
  fFieldMap = NULL;
  
  //
//   GetGlobalFieldManager()->SetDetectorField(this);
//   GetGlobalFieldManager()->CreateChordFinder(this);
}

//______________________________________________________________________________________________________
A2MagneticField::~A2MagneticField()
{
  // Deallocate memory
  if(fFieldMap)
  {
    for(int i=0; i<fPointsN[0]; ++i)
    {
      for(int j=0; j<fPointsN[1]; ++j)
      {
	delete [] fFieldMap[i][j];
      }
      delete [] fFieldMap[i];
    }
    delete [] fFieldMap;
  }
}

//______________________________________________________________________________________________________
// Read field map from a field_map.dat file and fill fFieldMap array
G4bool A2MagneticField::ReadFieldMap(const G4String &nameFileMap)
{
  // Print info
  G4cout.setf(std::ios_base::unitbuf);
  G4cout << "A2MagneticField::ReadFieldMap() Reading target magnetic field map " << nameFileMap;

  // Open input stream
  FILE* fin = 0;
  TString name(nameFileMap.data());
  if (name.EndsWith(".xz"))
  {
    G4cout << " (compressed)...";
    fin = popen(TString::Format("xzcat %s", name.Data()), "r");
  }
  else
  {
    G4cout << " (not compressed)...";
    fin = fopen(name.Data(), "r");
  }

  // In case of an error return FALSE
  if(!fin)
  {
    G4cout << " **ERROR** - File " << nameFileMap << " not found." << G4endl;
    return false;
  }

  // Read Xmin, Xmax, dX, Ymin, Ymax, dY, Zmin, Zmax, dZ and calculate number of points
  char line[256];
  for(G4int i=0; i<3; ++i)
  {
    fgets(line, 256, fin);
    G4int ret = sscanf(line, "%lf%lf%lf", &fPointMin[i], &fPointMax[i], &fPointStep[i]);
    if (ret != 3)
    {
      G4cout << " **ERROR** - Wrong header format." << G4endl;
      if (name.EndsWith(".xz")) pclose(fin);
      else fclose(fin);
      return false;
    }
    fPointStep[i] *= cm;
    fPointMin[i]   = fPointMin[i]*cm - fPointStep[i]/2.;
    fPointMax[i]   = fPointMax[i]*cm + fPointStep[i]/2.;
    fPointsN[i]    = ceil((fPointMax[i] - fPointMin[i])/fPointStep[i]);
  }

  // Allocate memory for fFieldMap
  fFieldMap = new A2Bvector**[fPointsN[0]];
  for(int i=0; i<fPointsN[0]; ++i)
  {
    fFieldMap[i] = new A2Bvector*[fPointsN[1]];
    for(int j=0; j<fPointsN[1]; ++j)
    {
      fFieldMap[i][j] = new A2Bvector[fPointsN[2]];
    }
  }

  // Read the magnetic field map
  G4int nline = 0;
  G4double p[3], b[3];
  G4int iPoint[3];
  while (fgets(line, 256, fin))
  {
    // read data
    G4int ret = sscanf(line, "%lf%lf%lf%lf%lf%lf", p, p+1, p+2, b, b+1, b+2);
    if (ret != 6)
    {
      G4cout << " **ERROR** - Wrong data format." << G4endl;
      if (name.EndsWith(".xz")) pclose(fin);
      else fclose(fin);
      return false;
    }

    // Calculate x,y,z indexes of the point
    iPoint[0] = GetPointIndex(p[0]*cm,fPointMin[0],fPointStep[0]); // index x
    iPoint[1] = GetPointIndex(p[1]*cm,fPointMin[1],fPointStep[1]); // index y
    iPoint[2] = GetPointIndex(p[2]*cm,fPointMin[2],fPointStep[2]); // index z

    // Fill the fFiledMap array
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][0] = b[0]*gauss; // Bx
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][1] = b[1]*gauss; // By
    fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][2] = b[2]*gauss; // Bz

    nline++;
  }

  // Close the field map file
  if (name.EndsWith(".xz")) pclose(fin);
  else fclose(fin);

  // OK => return true
  if (nline == fPointsN[0]*fPointsN[1]*fPointsN[2])
  {
    G4cout << " OK (read " << nline << " data points)"<< G4endl;
    return true;
  }
  else
  {
    G4cout << " **ERROR** - Could not read all " << fPointsN[0]*fPointsN[1]*fPointsN[2]
           << " data points."<< G4endl;
    return false;
  }
}

//______________________________________________________________________________________________________
// Get the magnetic induction vector value for the given point
void A2MagneticField::GetFieldValue(const G4double point[4], G4double *field) const
{
  // Set default magnetic field
  field[0] = field[1] = field[2] = 0.;
  
  // Check bounds
  if(point[0] < fPointMin[0] || point[0] > fPointMax[0]) return; // x
  if(point[1] < fPointMin[1] || point[1] > fPointMax[1]) return; // y
  if(point[2] < fPointMin[2] || point[2] > fPointMax[2]) return; // z
  
  // Get indexes of fFieldMap array which correspond to the point
  G4int iPoint[3] = {
                     GetPointIndex(point[0],fPointMin[0],fPointStep[0]), // x
                     GetPointIndex(point[1],fPointMin[1],fPointStep[1]), // y
                     GetPointIndex(point[2],fPointMin[2],fPointStep[2])  // z
                    };
  
  // Set field
  field[0] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][0];
  field[1] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][1];
  field[2] = fFieldMap[iPoint[0]][iPoint[1]][iPoint[2]][2];
}

//______________________________________________________________________________________________________
// Get index of the point "q" in an arithmetic progression with the first element "q0" and step "d"
// inline G4int A2MagneticField::GetPointIndex(const G4double &q, const G4double &q0, const G4double &d) const
// {
//   return (q-q0)/d;
// }
