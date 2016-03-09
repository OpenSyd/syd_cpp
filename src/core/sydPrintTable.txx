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

#include "sydPrintTable.h"

//------------------------------------------------------------------
template<class RecordType>
void syd::PrintTable::Dump(const std::vector<std::shared_ptr<RecordType>> & records,
                           std::ostream & os)
{
  if (records.size() == 0) return;

  // Initialisation
  std::streambuf * buf = os.rdbuf();
  mCurrentOutput = new std::ostream(buf);
  current_table_ = records[0]->GetTableName();
  if (GetFormat() == "") SetFormat("default");
  AddFormat("help", "Display list of formats and stop");
  AddFormat("default", "Default format");
  //  records[0]->InitTable(*this); // MUST BE CALLED Before !

  // Display help if needed
  if (GetFormat() == "help") {
    os << "Available formats for table '" << records[0]->GetTableName() << "' are: " << std::endl;
    std::ostringstream ss;
    for(auto f:formats_) {
      ss << "\t- " << f.name << ": "  << f.help << std::endl;
    }
    os << ss.str();
    return;
  }

  // Check that format name exists
  auto f = GetFormat();
  auto p = std::find_if(formats_.begin(), formats_.end(),
                        [&f](const syd::PrintFormat &v) { return v.name == f; });
  if (p == formats_.end()) {
    std::ostringstream ss;
    for(auto f:formats_) {
      ss << "\t- " << f.name << ": "  << f.help << std::endl;
    }
    EXCEPTION("Table format '" << GetFormat() << "' not known. Known formats are:"
              << std::endl << ss.str());
  }

  // Get the data
  for(auto r:records) {
    AddRow();
    r->DumpInTable(*this); // FIXME change the fc name (SetInTable)
  }

  // Real print
  Print(os);
}
//------------------------------------------------------------------
