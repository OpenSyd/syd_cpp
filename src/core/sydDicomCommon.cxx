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
    LOG(FATAL) << "Unrecognized tag name: '" << tagName << "'";
  }
  dcmDataDict.unlock();
  return dicent->getKey();
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::GetTagValue(DcmObject *dset, DcmTagKey & key)
{
  // Search for the tag values from the key. Only the first instance is printed
  DcmStack stack;
  char * v;
  if (dset->search(key, stack, ESM_fromHere, OFTrue) == EC_Normal) {
    unsigned long n = stack.card();
    if (n==0) {
      LOG(FATAL) << "Tag value not found for '"<< key.toString() << "'";
    }
    if (n>1) {
      // DD(n);
      // LOG(WARNING) << "Several values found for tag " << key.toString();
    }
    DcmObject *dobj = stack.top();
    DcmElement * e = static_cast<DcmElement*>(dobj);
    e->getString(v);
    if (v==NULL) return "";
    return std::string(v);
  }
  else { // not found EC_TagNotFound
    return "";
  }
}
//--------------------------------------------------------------------


//--------------------------------------------------------------------
std::string syd::GetTagValue(DcmObject *dset, std::string tagName)
{
  DcmTagKey k = GetTagKey(tagName);
  return GetTagValue(dset, k);
}
//--------------------------------------------------------------------
