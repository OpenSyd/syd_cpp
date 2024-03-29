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
#include "sydDicomStruct.h"

// --------------------------------------------------------------------
namespace syd {

#pragma db object polymorphic pointer(std::shared_ptr) table("syd::RoiMaskImage") callback(Callback)
  /// Store information about a contour transformed as a binary
  /// image. Consider the background is 0 and the foreground is 1
  class RoiMaskImage:
    public syd::Image {
  public:

    DEFINE_TABLE_CLASS(RoiMaskImage);

#pragma db not_null
    /// Foreign Key. Associated RoiType id
    syd::RoiType::pointer roitype;

    // Link the mask to a dicomStruct if it was created from a DicomStruct (not necessarily)
    // Cannot use the DicomSerie id from syd::Image
    syd::DicomStruct::vector dicomsStruct;

    /// Write the element as a string
    virtual std::string ToShortString() const;
    virtual std::string AllFieldsToString() const {return ToShortString();}

    /// Callback : delete the associated image when the roimaskimage is deleted.
    void Callback(odb::callback_event, odb::database&) const;
    void Callback(odb::callback_event, odb::database&);

    /// Compute the default image path (based on the patient's name)
    virtual std::string ComputeDefaultRelativePath();

    /// Compute the default image mhd filename (based on id + modality)
    virtual std::string ComputeDefaultMHDFilename();

  protected:
    RoiMaskImage();

  }; // end class
}
// --------------------------------------------------------------------

#endif
