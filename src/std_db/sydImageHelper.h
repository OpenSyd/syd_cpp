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
#include "sydDicomSerieHelper.h"

// --------------------------------------------------------------------
namespace syd {

  /// Create a new image, copy the filename to the db
  syd::Image::pointer InsertMhdImage(std::string filename,
                                     syd::Patient::pointer patient,
                                     std::string modality="image");

  /// Create 2 new Files for mhd/raw
  syd::File::vector InsertMhdFiles(syd::Database * db,
                                   std::string from_filename,
                                   std::string to_relative_path,
                                   std::string to_filename);

  /// Create a new image from a DicomSerie. Pixel type could be float,
  /// short, auto etc
  syd::Image::pointer InsertImageFromDicom(syd::DicomSerie::pointer dicom,
                                           std::string pixel_type);



  /// Read the attached file and set image spacing, size dimension,
  /// and pixel_type. The image is not updated
  void SetImageInfoFromFile(syd::Image::pointer image);

  /// Compute the default image path (based on the patient's name)
  std::string GetDefaultImageRelativePath(syd::Image::pointer image);

  /// Compute the default image mhd filename (based on id + modality)
  std::string GetDefaultMhdImageFilename(syd::Image::pointer image);

  /// Fill som image properties from option given in args_info
  template<class ArgsInfo>
  void SetImagePropertiesFromCommandLine(syd::Image::pointer image,
                                         ArgsInfo & args_info);

  /// Retrieve the syd::PixelUnit and set to the image
  void SetPixelUnit(syd::Image::pointer image, std::string pixel_unit);

  /// Retrieve the syd::Injection and set to the image
  void SetInjection(syd::Image::pointer image, std::string injection);

  /// Retrieve the syd::DicomSerie from its id and add it to the image
  void AddDicomSerie(syd::Image::pointer image, syd::IdType dicom_id);

  // Scale image's pixels. Will force the pixel type to be float
  void ScaleImage(syd::Image::pointer image, double s);

  ///// OLD BELOW


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
    // static void InsertMhdFiles(syd::Image::pointer image,
    //                            std::string filename,
    //                            bool moveFlag = false); // true=copy ; false=move

    /// Copy image properties from the 'like' image (modality,
    /// injection patient etc). Everything except the properties
    /// related to the image content itself (pixel_type, size etc).
    static void CopyInformation(syd::Image::pointer image,
                                const syd::Image::pointer like);


    static void UpdateMhdImageProperties(syd::Image::pointer image);

    static void UpdateMhdImageProperties(syd::Image::pointer image,
                                         itk::ImageIOBase::Pointer header);

    static bool IsSameImage(syd::Image::pointer a,
                            syd::Image::pointer b,
                            bool checkHistoryFlag=false);

    static void CheckSameImageAndFiles(syd::Image::pointer a,
                                       syd::Image::pointer b);

  }; // end class
} // namespace syd

#include "sydImageHelper.txx"
// --------------------------------------------------------------------

#endif
