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

typedef unsigned int IdType;

// --------------------------------------------------------------------
#pragma db object
class TimePoint
{
public:

#pragma db id auto
  IdType        id;
  //  IdType        patient_id; // not strictly needed (can be retrive by serie_id)
  IdType        serie_id;
  //std::string   acquisition_date; // not strictly needed (can be retrive by serie_id)
  //  std::string   n; // number in the set of time points FIXME
  std::string   ct_mhd;
  std::string   spect_mhd;

  friend std::ostream& operator<<(std::ostream& os, const TimePoint & p) {
    os << p.id << " " << " " << p.ct_mhd << " " << p.spect_mhd;
    return os;
  }

};
// --------------------------------------------------------------------
