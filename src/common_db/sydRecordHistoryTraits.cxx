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

  - BSD        See included LICENSE.txt RecordHistory
  - CeCILL-B   http://www.cecill.info/licences/Licence_CeCILL-B_V1-en.html
  ===========================================================================**/

// syd
#include "sydRecordHistoryTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RecordHistory);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
template<> void syd::RecordTraits<syd::RecordHistory>::
Sort(syd::RecordHistory::vector & v, const std::string & type) const
{
  DD("specific RecordHistory sort");
  if (type == "id")
    std::sort(begin(v), end(v), [v](pointer a, pointer b) {
        return a->id < b->id; });
  if (type == "default" or type=="update")
    std::sort(begin(v), end(v), [v](pointer a, pointer b) {
        return a->update_date < b->update_date; });
  if (type == "help") {
    LOG(0) << "Available sort type: 'id' or 'update'";
  }
}
*/
// --------------------------------------------------------------------
