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
#include "sydFileTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(File);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
template<> void syd::RecordTraits<syd::File>::
Sort(syd::File::vector & v, const std::string & type) const
{
  DD("specific file sort");
  if (type == "id")
    return std::sort(begin(v), end(v), [v](pointer a, pointer b) {
        return a->id < b->id; });
  if (type == "default" or type=="filename" or type == "name" or type=="")
    return std::sort(begin(v), end(v), [v](pointer a, pointer b) {
        return a->filename < b->filename; });
  LOG(0) << "Available sort type: 'id' or 'filename' (or 'name')";
}
*/
// --------------------------------------------------------------------
