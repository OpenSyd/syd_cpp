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
template<class RecordType>
typename RecordType::vector syd::BindTo(const syd::Record::vector & records)
{
  typename RecordType::vector results;
  for(auto & r:records) {
    auto x = std::dynamic_pointer_cast<RecordType>(r);
    if (x == NULL) {
      EXCEPTION("The record is not a " << typeid(RecordType).name()
                << " (not subclass of), it is a " << r->GetTableName()
                << std::endl
                << "It cannot be converted (fc BindTo).");
    }
    results.push_back(x);
  }
  return results;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<class RecordType>
typename syd::Record::vector syd::BindFrom(const typename RecordType::vector & records)
{
  syd::Record::vector results;
  for(auto r:records) {
    //    results.push_back(std::static_pointer_cast<syd::Record>(r));
    results.push_back( std::make_shared<syd::Record>(static_cast<syd::Record*>(r.get())));
  }
  return results;
}
// --------------------------------------------------------------------


