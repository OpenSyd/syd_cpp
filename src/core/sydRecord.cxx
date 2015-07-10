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
#include "sydRecord.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::Record::Record()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::Set(const syd::Database * db, const std::vector<std::string> & args)
{
  LOG(FATAL) << "The function Set(db, args) must be implemented for table " << GetTableName();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
bool syd::Record::IsEqual(const pointer p) const
{
  return (id == p->id);
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Record::CopyFrom(const pointer p)
{
  id = p->id;
}
// --------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::InitPrintTable(const syd::Database * db,
                                 syd::PrintTable & ta,
                                 const std::string & format) const
{
  ta.AddColumn("#id", 5);
  ta.AddColumn("#fields", 80);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::Record::DumpInTable(const syd::Database * db,
                              syd::PrintTable & ta,
                              const std::string & format) const
{
  ta << id << ToString();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & db) const
{
  //DD("Callback_const "+syd::ToString(event)+" "+GetTableName());
  if (event == odb::callback_event::pre_erase) {

  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Record::Callback(odb::callback_event event, odb::database & d)
{
  //DD("Callback "+syd::ToString(event)+" "+GetTableName());
  if (event == odb::callback_event::pre_load) {
    //DD(" load db pointer ?"); //FIXME here
    auto search = syd::Database::ListOfLoadedDatabases.find(&d);
    if (search == syd::Database::ListOfLoadedDatabases.end()) {
      LOG(FATAL) << "Error db not find FIXME";
    }
    //DD(search->second->GetFilename());
    db_ = search->second;
  }
}
// --------------------------------------------------
