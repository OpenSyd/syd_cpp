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

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  class RoiType {
  public:

#pragma db id auto
    IdType        id;
    std::string   name;

    // --------------------------------------------------
    // Constructor, Destructor
    ~RoiType() {}
    RoiType() {}
    RoiType(const RoiType & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    RoiType & operator= (const RoiType & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const RoiType & t) {
      id = t.id;
      name = t.name;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const RoiType & p) {
      os << p.id << " " << p.name;
      return os;
    }
    // --------------------------------------------------


  }; // class RoiType
} // namespace syd
// --------------------------------------------------------------------
