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

#pragma db object  polymorphic pointer(std::shared_ptr) table("syd::RoiMaskImage") callback(Callback)
  /// Store information about a contour transformed as a binary image.
  class RoiMaskImage: public syd::Image {
  public:

#pragma db not_null
    /// Foreign Key. Associated RoiType id
    syd::RoiType::pointer roitype;

    // ------------------------------------------------------------------------
    TABLE_DEFINE_I(RoiMaskImage, syd::RoiMaskImage, syd::Image);
    TABLE_DECLARE_MANDATORY_FUNCTIONS(RoiMaskImage);
    // ------------------------------------------------------------------------

    /// Standard folder
    virtual std::string ComputeRelativeFolder() const;

    /// When create a new image, compute a default name. Image *must* be persistant (with correct id)
    std::string ComputeDefaultAbsolutePath(syd::Database * db) const;

    /// Callback : delete the associated image when the roimaskimage is deleted.
    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    // FIXME
    virtual void InitTable(syd::PrintTable & table) const;
    virtual void DumpInTable(syd::PrintTable & table) const;

  protected:
    RoiMaskImage();

  }; // end class
}
// --------------------------------------------------------------------

#endif
