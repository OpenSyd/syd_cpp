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
void syd::StandardDatabase::Find(std::vector<std::shared_ptr<RecordType>> & records,
                                 const std::vector<std::string> & patterns,
                                 const std::vector<std::string> & exclude)
{
  DD("Find std");
  Query(records);
  std::vector<std::shared_ptr<RecordType>> results;
  DD(records.size());
  for(auto r:records) {
    std::string s = r->ToString();
    std::size_t found = std::string::npos; // not found
    DD(s);
    for(auto p:patterns) {
      DD(p);
      found = s.find(p);
      if (found == std::string::npos) continue;
    }
    DD(found);
    if (found != std::string::npos) results.push_back(r);
    }
  records = results;
}
// --------------------------------------------------------------------
