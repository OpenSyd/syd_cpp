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

#ifndef SYDDICOMSERIEHELPER_H
#define SYDDICOMSERIEHELPER_H

// syd
#include "sydDicomSerie.h"
#include "sydImageUtils.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  // Write the dicom into a mhd file, according to the given
  // pixel_type. If pixel_type is 'auto', the header of the dicom is
  // read to guess the pixel_type.
  void WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                       std::string pixel_type,
                       std::string mhd_filename);

  // Write the dicom in the DicomSerie to a mhd file
  template<class ImageType>
  void WriteDicomToMhd(syd::DicomSerie::pointer dicom,
                       std::string mhd_filename);

  // Read a dicom into an itk_image
  template<class ImageType>
  typename ImageType::Pointer
  ReadDicomSerieImage(syd::DicomSerie::pointer dicom);

  /// Helper function (will change)
  template<typename F>
  F GetFctByPixelType(std::map<std::string, F> & map,
                           std::string pixel_type);

  /// Anonymize dicom serie
  void AnonymizeDicomSerie(syd::DicomSerie::pointer dicom);

}
#include "sydDicomSerieHelper.txx"
// --------------------------------------------------------------------

#endif
