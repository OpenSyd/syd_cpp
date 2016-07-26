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

#ifndef SYDDICOMCOMMON_H
#define SYDDICOMCOMMON_H

// syd
#include "sydCommon.h"

// dcmtk
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcistrmf.h"
#include "dcmtk/oflog/helpers/loglog.h"

//--------------------------------------------------------------------
namespace syd {

  /// Open a dicom file, return a DcmFileFormat that contains dicom
  /// tag. Return false if could not open the file
  bool OpenDicomFile(std::string filename, DcmFileFormat & dfile);
  //itk::GDCMImageIO::Pointer OpenDicomFile(std::string filename);

  /// Return the key of a tag knowing his name
  DcmTagKey GetTagKey(std::string tagName);

  /// Return the value of a tag from is name
  DcmElement * GetTagValue(DcmObject * dest, std::string tagName);

  /// Return the value of a tag from is keya
  DcmElement * GetTagValue(DcmObject * dest, DcmTagKey & key);

  /// Return the tag value as a string
  std::string GetTagValueString(DcmObject *dset, std::string tagName);

  /// Return the tag value as a ushort
  ushort GetTagValueUShort(DcmObject *dset, std::string tagName);

  /// Return the tag value as a double
  double GetTagValueDouble(DcmObject *dset, std::string tagName);

  /// Concat date and time (from dicom) to "2015-04-01 10:00"
  std::string ConvertDicomDateToStringDate(std::string date, std::string time);

} // end namespace

#endif
