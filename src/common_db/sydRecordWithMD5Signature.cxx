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
#include "sydRecordWithMD5Signature.h"
#include "sydDatabase.h"

// --------------------------------------------------------------------
syd::RecordWithMD5Signature::RecordWithMD5Signature()
{
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
void syd::RecordWithMD5Signature::Callback(odb::callback_event event,
                                           odb::database & odb,
                                           syd::Database * db) const
{
  if (event == odb::callback_event::pre_persist) {
    DD("persist with md5");
    md5 = ComputeMD5();
  }

  if (event == odb::callback_event::pre_update) {
    DD("Update with md5");
    md5 = ComputeMD5();
  }

  // Events in Callback const : persist, update, erase
  // event load can only be here if the non-const version does not exist
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithMD5Signature::InitTable(syd::PrintTable & ta) const
{
  ta.AddFormat("md5", "Display the MD5 signature of the records");
  auto & f = ta.GetFormat();
  if (f == "md5") {
    ta.AddColumn("md5");
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RecordWithMD5Signature::DumpInTable(syd::PrintTable & ta) const
{
  auto & f = ta.GetFormat();
  if (f == "md5") {
    ta.Set("md5", md5);
  }
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RecordWithMD5Signature::ComputeMD5() const
{
  MD5 m;
  auto s = ToStringForMD5();
  m.update((char*)s.c_str(), s.size());
  m.finalize();
  return m.hexdigest();
}
// --------------------------------------------------------------------
