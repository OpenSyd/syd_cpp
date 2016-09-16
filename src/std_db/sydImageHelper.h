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
#include "sydImageExtractSlices.h"
#include "sydImageGeometricalMean.h"
#include "sydPixelUnitHelper.h"

// --------------------------------------------------------------------
namespace syd {

  /// Create a new image, copy the filename to the db
  syd::Image::pointer InsertImageFromFile(std::string filename,
                                          syd::Patient::pointer patient,
                                          std::string modality="image");

  /// Create a new image and save the itk as a mhd in the db
  template<typename ImageType>
  syd::Image::pointer InsertImage(typename ImageType::Pointer itk_image,
                                  syd::Patient::pointer patient,
                                  std::string modality="image");

  /// Create a new image from a DicomSerie. Pixel type could be float,
  /// short, auto etc
  syd::Image::pointer InsertImageFromDicomSerie(syd::DicomSerie::pointer dicom,
                                                std::string pixel_type);

  /// Create a new image by stitching 2 dicoms
  syd::Image::pointer InsertStitchDicomImage(syd::DicomSerie::pointer dicom1,
                                             syd::DicomSerie::pointer dicom2,
                                             double threshold_cumul,
                                             double skip_slices);

  /// Create 2 new Files for mhd/raw
  syd::File::vector InsertFilesFromMhd(syd::Database * db,
                                       std::string from_filename,
                                       std::string to_relative_path,
                                       std::string to_filename);

  /// Read the attached file and set image spacing, size dimension,
  /// and pixel_type. The image is not updated
  void SetImageInfoFromFile(syd::Image::pointer image);

  /// Coopy image info from the like file
  void SetImageInfoFromImage(syd::Image::pointer image,
                             const syd::Image::pointer like);

  /// Set some information from the dicom (acquisition_date, modality etc)
  void SetImageInfoFromDicomSerie(syd::Image::pointer image,
                                  const syd::DicomSerie::pointer dicom);

  /// Fill som image properties from option given in args_info
  template<class ArgsInfo>
  void SetImageInfoFromCommandLine(syd::Image::pointer image,
                                   ArgsInfo & args_info);

  /// Retrieve the syd::PixelUnit and set to the image
  void SetPixelUnit(syd::Image::pointer image, std::string pixel_unit);

  /// Retrieve the syd::Injection and set to the image
  void SetInjection(syd::Image::pointer image, std::string injection);

  /// Retrieve the syd::DicomSerie from its id and add it to the image
  void AddDicomSerie(syd::Image::pointer image, syd::IdType dicom_id);

  // Scale image's pixels. Will force the pixel type to be float
  void ScaleImage(syd::Image::pointer image, double s);

  // Check if the images and the associated files are similar
  bool CheckSameImageAndFiles(syd::Image::pointer a,
                              syd::Image::pointer b);

  // Check if the image content are the same
  bool IsSameImage(syd::Image::pointer a,
                   syd::Image::pointer b,
                   bool checkHistoryFlag=false);


  // Compute the geometrical mean of a planar image. Consider composed
  // of 4 slices
  syd::Image::pointer InsertImageGeometricalMean(const syd::Image::pointer input,
                                                 double k=0.5);


  /// Crop an image like another one
  void CropImageLike(syd::Image::pointer image,
                     const syd::Image::pointer like);

  /// Compute the activity in MBq by detected counts
  double ComputeActivityInMBqByDetectedCounts(syd::Image::pointer image);

  /// Flip image if negative spacing is detected
  bool FlipImageIfNegativeSpacing(syd::Image::pointer image);

} // namespace syd

#include "sydImageHelper.txx"
// --------------------------------------------------------------------

#endif
