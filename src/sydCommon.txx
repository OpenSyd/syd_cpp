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
template<uint group, uint element>
void ReadTagString(const gdcm::DataSet & ds, std::string & value)
{
  gdcm::Attribute<group,element> att;
  att.SetFromDataSet(ds);
  value = att.GetValue();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<uint group, uint element, class T>
void ReadTagFromSeq(const gdcm::DataSet & ds, uint seqgroup, uint seqelement, T & value, int n)
{
  gdcm::Tag tag(seqgroup, seqelement);
  const gdcm::DataElement& csq = ds.GetDataElement(tag);
  gdcm::SmartPointer<gdcm::SequenceOfItems> sq = csq.GetValueAsSQ();
  gdcm::DataSet & nds = sq->GetItem(n).GetNestedDataSet();
  gdcm::Attribute<group,element >attribute;
  attribute.SetFromDataSet(nds);
  value = attribute.GetValue();
}
// --------------------------------------------------------------------


template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}
