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

#ifndef SYDIMAGEHELPER_H
#define SYDIMAGEHELPER_H

// syd
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This file contains helpers function that are hopefully helpful
  /// to create and update syd::Image table. All functions are static
  /// in a class for clarity.
  ///
  /// Example of use
  ///  syd::ImageHelper::InsertMhdFiles(image, filename);
  class ImageHelper
  {
  public:

    /// If File are already associated with the image, remove them
    /// frist.  Then create new File and copy an mhd image in the db,
    /// Image must be persistent.
    static void InsertMhdFiles(syd::Image::pointer image,
                               std::string filename);

    /// Copy image properties from the 'like' image (modality,
    /// injection patient etc). Everything except the properties
    /// related to the image content itself (pixel_type, size etc).
    static void CopyInformation(syd::Image::pointer image,
                                const syd::Image::pointer like);

    /// Retrieve the syd::PixelUnit and set to the image
    static void SetPixelUnit(syd::Image::pointer image, std::
                             string pixel_unit);

    /// Retrieve the syd::Injection and set to the image
    static void SetInjection(syd::Image::pointer image,
                             std::string injection);

    /// Retrieve the syd::DicomSerie and add it to the image
    static void AddDicomSerie(syd::Image::pointer image,
                              syd::IdType id);

    static void UpdateMhdImageProperties(syd::Image::pointer image);

    static void UpdateMhdImageProperties(syd::Image::pointer image,
                                         itk::ImageIOBase::Pointer header);

    template<class ArgsInfo>
    static void UpdateImagePropertiesFromCommandLine(syd::Image::pointer image,
                                                     ArgsInfo & args_info);

  }; // end class
} // namespace syd

#include "sydImageHelper.txx"
// --------------------------------------------------------------------

#endif
