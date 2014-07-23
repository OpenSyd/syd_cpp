/*=========================================================================
  Program:   syd

  Authors belong to:
  - University of LYON              http://www.universite-lyon.fr/
  - Léon Bérard cancer center       http://www.centreleonberard.fr
  - CREATIS CNRS laboratory         http://www.creatis.insa-lyon.fr

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the copyright notices for more information.

  It is distributed under dual licence

  - BSD        See included LICENSE.txt file
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

#include <string>
#include <iostream>
#include <odb/core.hxx>

// --------------------------------------------------------------------
#pragma db object
class Serie
{
public:

#pragma db id auto
  unsigned long Id;

  unsigned long StudyId;
  std::string FOV;
  std::string MHDFilename;
  std::string DataSetUID;
  std::string AcqDate;
  std::string AcqDurationInSec;
  std::string DicomFilename;
  std::string ReconstructionDate;
  double TimeFromInjectionInHours;
  double PixelScale;

  friend std::ostream& operator<<(std::ostream& os, const Serie & p) {
    os << p.Id << " " << p.StudyId << " " << p.FOV << " " << p.MHDFilename;
    return os;
  }

};
// --------------------------------------------------------------------
