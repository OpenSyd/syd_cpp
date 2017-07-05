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
#include "sydDicomUtils.h"
#include "sydCommon.h"

// gdcm (form itk)
#include "gdcmAttribute.h"

// --------------------------------------------------------------------
std::string syd::ConvertDicomDateToStringDate(std::string date, std::string time)
{
  std::string result;
  if (date.empty()) return "";
  if (date == empty_value) return "";
  result= date.substr(0,4)+"-"+date.substr(4,2)+"-"+date.substr(6,2);
  if (time.empty()) return result;
  if (time == empty_value) return "";
  result= result+" "+ time.substr(0,2)+":"+time.substr(2,2);
  return result;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::SearchDicomTagNameFromTagKey(std::string tagkey)
{
  /*
    I do not know how to retrive the private tag name. The code below
    is a very limited attempt to get it by looking on specific private
    tag group. There is probably a better way to do that.
  */

  // Modify tag_key "0011|103b" -> "0011|003b"
  std::string tag_key = tagkey;
  tag_key[5] = '0';
  tag_key[6] = '0';

  gdcm::PrivateTag tag;
  tag.ReadFromPipeSeparatedString(tag_key.c_str());
  const gdcm::Global & g = gdcm::Global::GetInstance();
  const gdcm::Dicts & dicts = g.GetDicts();
  const gdcm::PrivateDict & pdict = dicts.GetPrivateDict();

  // Limited list of owner to search
  std::vector<std::string> owner_list =
    {"GEMS_GENIE_1",
     "GEMS_XELPRV_01",
     "PHILIPS NM -Private",
     "Philips Imaging DD 001",
     "Philips Imaging DD 002",
     "Philips Imaging DD 124",
     "SIEMENS MED NM",
     "ELSCINT1"
    };
  for(auto owner:owner_list) {
    tag.SetOwner(owner.c_str());
    if (pdict.FindDictEntry(tag)) {
      const gdcm::DictEntry & entry = pdict.GetDictEntry(tag);
      return entry.GetName();
    }
  }
  return "Unknown";
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
itk::GDCMImageIO::Pointer syd::ReadDicomHeader(std::string filename)
{
  typedef itk::GDCMImageIO ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  dicomIO->SetFileName(filename);
  dicomIO->SetLoadPrivateTags(true);
  dicomIO->SetLoadSequences(true);
  try {
    dicomIO->ReadImageInformation();
  } catch (std::exception & e) {
    EXCEPTION("Error cannot read '" << filename << "' (it is not a dicom file ?) "
              << e.what() << ".");
  }
  return dicomIO;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
double syd::GetTagDoubleValueFromTagKey(itk::GDCMImageIO::Pointer dicomIO,
                                        const std::string & key,
                                        const double & defaultValue)
{
  std::ostringstream oss;
  oss << defaultValue;
  std::string r = GetTagValueFromTagKey(dicomIO, key, oss.str());
  if (r == "")
   return defaultValue;
  double rr = stod(r);
  return rr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
gdcm::Reader syd::ReadDicomStructHeader(std::string filename)
{
  gdcm::Reader reader;
  reader.SetFileName(filename.c_str());
  if (!reader.Read()) {
    EXCEPTION("Error cannot read '" << filename
              << "' (it is not a dicom struct file ?)");
  }

  const gdcm::DataSet & dataset = reader.GetFile().GetDataSet();
  gdcm::MediaStorage ms;
  ms.SetFromFile(reader.GetFile());
  // (3006,0020) SQ (Sequence with explicit length #=4) # 370, 1 StructureSetROISequence
  gdcm::Tag tssroisq(0x3006,0x0020);
  if (!dataset.FindDataElement(tssroisq)) {
    EXCEPTION("Error cannot read tag 0x3006,0x0020");
  }

  gdcm::Tag troicsq(0x3006,0x0039);
  if (!dataset.FindDataElement(troicsq)) {
    EXCEPTION("Error cannot read tag 0x3006,0x0039");
  }

  return reader;
}
// --------------------------------------------------------------------


