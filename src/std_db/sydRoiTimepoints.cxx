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
#include "sydRoiTimepoints.h"

// --------------------------------------------------------------------
syd::RoiTimepoints::RoiTimepoints():syd::Timepoints()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiTimepoints::ToString() const
{
  std::stringstream ss ;
  ss << syd::Timepoints::ToString() << " (";
  for(auto stat:roi_statistics)
    ss << stat->id << " ";
  ss << ")";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiTimepoints::DumpInTable_default(syd::PrintTable & ta) const
{
  syd::Timepoints::DumpInTable_default(ta);
  std::string ids = "";
  for(auto stat:roi_statistics) ids = std::to_string(stat->id)+" ";
  ta.Set("stat", ids);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiTimepoints::ToStringForMD5() const
{
  std::stringstream ss;
  ss << syd::Timepoints::ToStringForMD5();
  for(auto stat:roi_statistics)
    ss << stat->id << " ";
  return ss.str();
}
// --------------------------------------------------------------------

