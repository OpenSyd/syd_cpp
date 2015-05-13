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
  class RoiMaskImage {
  public:

#pragma db id auto
    IdType        id;
    IdType        mask_id;
    IdType        timepoint_id;
    IdType        roitype_id;
    double        volume_in_cc;
    double        density_in_g_cc;

    // --------------------------------------------------
    // Constructor, Destructor
    ~RoiMaskImage() {}
    RoiMaskImage() {}
    RoiMaskImage(const RoiMaskImage & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    RoiMaskImage & operator= (const RoiMaskImage & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const RoiMaskImage & t) {
      id = t.id;
      mask_id = t.mask_id;
      timepoint_id = t.timepoint_id;
      roitype_id = t.roitype_id;
      volume_in_cc = t.volume_in_cc;
      density_in_g_cc = t.density_in_g_cc;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const RoiMaskImage & p) {
      os << p.id << " " << p.mask_id << " " << p.timepoint_id << " " << p.roitype_id << " " << p.volume_in_cc;
      return os;
    }
    // --------------------------------------------------


  }; // class RoiMaskImage
} // namespace syd
// --------------------------------------------------------------------
