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

#ifndef SYDROIMASKIMAGE_H
#define SYDROIMASKIMAGE_H

// syd
#include "sydImage.h"
#include "sydRoiType.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object
  /// Store information about a contour transformed as a binary image.
  class RoiMaskImage : public syd::TableElementBase {
  public:

#pragma db id auto
    /// Id of the RoiMaskImage
    IdType id;

#pragma db not_null
    /// Foreign key, it must exist in the Image table.
    std::shared_ptr<syd::Image> image;

#pragma db not_null
    /// Foreign Key. Associated RoiType id
    std::shared_ptr<syd::RoiType> roitype;

    // ------------------------------------------------------------------------
    SET_TABLE_NAME("RoiMaskImage")
    RoiMaskImage();

    virtual std::string ToString() const;
    virtual std::string ToLargeString() const;

    bool operator==(const RoiMaskImage & p);
    bool operator!=(const RoiMaskImage & p) { return !(*this == p); }

    virtual void OnDelete(syd::Database * db);

  }; // end class
}
// --------------------------------------------------------------------

#endif
