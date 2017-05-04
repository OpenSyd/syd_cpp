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

#ifndef SYDDICOMUTILS_H
#define SYDDICOMUTILS_H

// syd
#include "sydCommon.h"

// itk (gdcm)
#include <itkImage.h>
#include <itkMetaDataObject.h>
#include <itkGDCMImageIO.h>
#include "gdcmGlobal.h"
#include "gdcmDictEntry.h"
#include "gdcmDicts.h"

// --------------------------------------------------------------------
namespace syd {

  /// Concat date and time (from dicom) to "2015-04-01 10:00"
  std::string ConvertDicomDateToStringDate(std::string date,
                                           std::string time);

  /// Search dicom tag name in private dictionnary
  std::string SearchDicomTagNameFromTagKey(std::string tagkey);

  /// Open dicom header
  itk::GDCMImageIO::Pointer ReadDicomHeader(std::string filename);

  /// Retrive the tag value with the correct type
  template<class TagType>
  TagType GetTagValueFromTagKey(itk::GDCMImageIO::Pointer dicomIO,
                                const std::string & key,
                                const TagType & defaultValue);

  /// Retrive the tag value with the correct type
  double GetTagDoubleValueFromTagKey(itk::GDCMImageIO::Pointer dicomIO,
                                     const std::string & key,
                                     const double & defaultValue);

#include "sydDicomUtils.txx"

} // end namespace
// --------------------------------------------------------------------

#endif
