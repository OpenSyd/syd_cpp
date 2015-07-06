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
#include "sydTag.h"

// --------------------------------------------------
syd::Tag::Tag():syd::Record("")
{
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Tag::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << label << " "
     << description;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Tag::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and label == p->label and
          description == p->description);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  label = p->label;
  description = p->description;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert patient, please set <label> <description>";
  }
  label = arg[0];
  description = arg[1];
}
// --------------------------------------------------



// --------------------------------------------------
void syd::Tag::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Tag': " << std::endl
              << "\tdefault: id label description" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("label", 8);
  ta.AddColumn("description", 25);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Tag::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << label << description;
}
// --------------------------------------------------

// --------------------------------------------------
/*std::string syd::GetTagLabels(const std::vector<std::shared_ptr<syd::Tag>> & tags)
{
  std::ostringstream os;
  if (tags.size() == 0) return "no_tag";
  os << tags[0]->label;
  for(auto i=1; i<tags.size(); i++) os << "," << tags[i]->label;
  return os.str();
  }*/
// --------------------------------------------------
