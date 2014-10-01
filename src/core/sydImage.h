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

#ifndef SYDIMAGE_H
#define SYDIMAGE_H

// syd
#include "sydDicomCommon.h"

// itk
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itkGDCMSeriesFileNames.h>

// --------------------------------------------------------------------
namespace syd {

  //--------------------------------------------------------------------
  // return md5 of the raw image
  std::string ConvertDicomSPECTFileToImage(std::string dicom_filename, std::string mhd_filename);
  std::string ConvertDicomCTFolderToImage(std::string dicom_path, std::string mhd_filename);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  template<class ImageType>
  void WriteImage(typename ImageType::Pointer image, std::string filename);
  template<class ImageType>
  typename ImageType::Pointer ReadImage(std::string filename);
  //--------------------------------------------------------------------


  //--------------------------------------------------------------------
  void CopyMHDImage(std::string from, std::string to, int verbose_level=2);
  void RenameMHDImage(std::string from, std::string to, int verbose_level=2);
  void RenameOrCopyMHDImage(std::string from, std::string to, int verbose_level, bool erase);
  void DeleteMHDImage(std::string filename);
  //--------------------------------------------------------------------


#include "sydImage.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
