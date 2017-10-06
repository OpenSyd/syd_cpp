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

// --------------------------------------------------------------------
template<class TagType>
TagType GetTagValueFromTagKey(itk::GDCMImageIO::Pointer dicomIO,
                              const std::string & key,
                              const TagType & defaultValue)
{
  TagType v = defaultValue;
  typedef itk::MetaDataDictionary DictionaryType;
  const DictionaryType & dictionary = dicomIO->GetMetaDataDictionary();
  typedef itk::MetaDataObject< TagType > MetaDataTagType;
  DictionaryType::ConstIterator tagItr = dictionary.Find(key);
  DictionaryType::ConstIterator end = dictionary.End();
  if (tagItr != end) {
    typename MetaDataTagType::ConstPointer entryvalue =
      dynamic_cast<const MetaDataTagType *>(tagItr->second.GetPointer());
    if (entryvalue) {
      v = entryvalue->GetMetaDataObjectValue();
    }
  }
  else
    LOG(sydlog::WARNING) << "Tag is not found or does not exist: " << key << std::endl;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TagType>
TagType GetTagValueFromSequence(const gdcm::SequenceOfItems* sequence,
                                uint16_t group, uint16_t element)
{
  gdcm::Item item = sequence->GetItem(1);
  gdcm::Tag tag(group, element);
  if (!item.FindDataElement(tag)) {
    EXCEPTION("Problem locating dicom tag " << group << "|" << element << std::endl);
  }
  TagType value = *((TagType *) item.GetDataElement(tag).GetByteValue()->GetPointer());
  return value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class TagType>
TagType GetTagValueFromStringSequence(const gdcm::SequenceOfItems* sequence,
                                      uint16_t group, uint16_t element)
{
  gdcm::Item item = sequence->GetItem(1);
  gdcm::Tag tag(group, element);
  if (!item.FindDataElement(tag)) {
    EXCEPTION("Problem locating dicom tag " << group << "|" << element << std::endl);
  }
  std::string p_value( item.GetDataElement(tag).GetByteValue()->GetPointer(), item.GetDataElement(tag).GetByteValue()->GetLength() );
  TagType value = (TagType)atof(p_value.c_str());
  return value;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<uint16_t Group, uint16_t Element>
std::string GetTagValueAsString(const gdcm::DataSet & dataset)
{
  gdcm::Attribute<Group,Element> tag;
  tag.SetFromDataSet(dataset);
  auto s = tag.GetValue();
  StripNullTerminatedChar(s);
  return s;
}
// --------------------------------------------------------------------

