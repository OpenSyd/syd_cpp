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

// syd
#include "sydDicomCommon.h"

//--------------------------------------------------------------------
DcmTagKey syd::GetTagKey(std::string tagName)
{
  // Search for tag key from tag name
  const DcmDataDictionary &globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dicent = globalDataDict.findEntry(tagName.c_str());
  if (dicent == NULL) {
    dcmDataDict.unlock();
    EXCEPTION("Unrecognized tag name: '" << tagName << "'");
  }
  dcmDataDict.unlock();
  return dicent->getKey();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::GetTagValueString(DcmObject *dset, std::string tagName)
{
  char * v;
  DcmElement * e = GetTagValue(dset, tagName);
  if (e == NULL) return "";
  e->getString(v);
  //e->getUint16(d);
  if (v==NULL) return "";
  return std::string(v);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
ushort syd::GetTagValueUShort(DcmObject *dset, std::string tagName)
{
  DcmElement * e = GetTagValue(dset, tagName);
  if (e == NULL) return 0;
  ushort d;
  e->getUint16(d);
  return d;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
double syd::GetTagValueDouble(DcmObject *dset, std::string tagName)
{
  DcmElement * e = GetTagValue(dset, tagName);
  if (e == NULL) return 0;
  double d;
  e->getFloat64(d);
  return d;
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DcmElement * syd::GetTagValue(DcmObject * dset, std::string tagName)
{
  DcmTagKey key = syd::GetTagKey(tagName);
  return GetTagValue(dset, key);
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DcmElement * syd::GetTagValue(DcmObject * dset, DcmTagKey & key)
{
  // Search for the tag values from the key. Only the first instance is printed
  DcmStack stack;
  char * v;
  if (dset->search(key, stack, ESM_fromHere, OFTrue) == EC_Normal) {
    unsigned long n = stack.card();
    if (n==0) {
      EXCEPTION("Tag value not found for '"<< key.toString() << "'");
    }
    if (n>1) {
      // DD(n);
      // LOG(WARNING) << "Several values found for tag " << key.toString();
    }
    DcmObject *dobj = stack.top();
    return static_cast<DcmElement*>(dobj);
  }
  else { // not found EC_TagNotFound
    return NULL;
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
bool syd::OpenDicomFile(std::string filename, DcmFileFormat & dfile)
{
  // remove debug output of dcmtk
  OFLog::configure(OFLogger::OFF_LOG_LEVEL);
  const E_TransferSyntax xfer = EXS_Unknown; // auto detection
  const E_GrpLenEncoding groupLength = EGL_noChange;
  const E_FileReadMode readMode = ERM_autoDetect;
  const Uint32 maxReadLength = DCM_MaxReadLength;
  OFCondition cond =
    dfile.loadFile(filename.c_str(), xfer, groupLength, maxReadLength, readMode);
  return cond.good();
  // error message in cond.text()
}
//--------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::ConvertDicomDateToStringDate(std::string date, std::string time)
{
  std::string result;
  if (date.empty()) return "";
  result= date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2);
  if (time.empty()) return result;
  result= result+" "+ time.substr(0,2)+":"+time.substr(2,2);
  return result;
}
// --------------------------------------------------------------------
