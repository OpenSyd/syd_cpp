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

//--------------------------------------------------------------------
namespace syd {

  bool OpenDicomFile(std::string filename, bool contIfNotDicom, DcmFileFormat & dfile);

  DcmTagKey GetTagKey(std::string tagName);
  DcmElement * GetTagValue(DcmObject * dest, std::string tagName);
  DcmElement * GetTagValue(DcmObject * dest, DcmTagKey & key);
  std::string GetTagValueString(DcmObject *dset, std::string tagName);
  ushort GetTagValueUShort(DcmObject *dset, std::string tagName);
  double GetTagValueDouble(DcmObject *dset, std::string tagName);

  class DicomSerieInfo {
  public:
    std::string serie_UID_;
    std::vector<std::string> filenames_;
  };


} // end namespace

#endif
