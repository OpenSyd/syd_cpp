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

// std
#include <string>
#include <iostream>
#include <memory>

// syd
#include "sydCommon.h"

// odb
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  class RawImage {
  public:

#pragma db id auto
    IdType        id;
    IdType        patient_id;
    std::string   filename;
    std::string   path;
    std::string   md5;
    std::string   pixel_type;

    // --------------------------------------------------
    // Constructor, Destructor
    ~RawImage() {}
    RawImage() {}
    RawImage(const RawImage & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    RawImage & operator= (const RawImage & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const RawImage & t) {
      id = t.id;
      patient_id = t.patient_id;
      filename = t.filename;
      path = t.path;
      md5 = t.md5;
      pixel_type = t.pixel_type;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const RawImage & p) {
      os << p.id << " " << p.filename << " " << p.pixel_type << " " << p.patient_id;
      return os;
    }
    // --------------------------------------------------


  }; // class RawImage
} // namespace syd
// --------------------------------------------------------------------
