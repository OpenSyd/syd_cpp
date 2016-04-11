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
#include "sydRoiStatistic.h"

// std
#include <set>

// --------------------------------------------------
syd::RoiStatistic::RoiStatistic():
  syd::Record(),
  syd::RecordWithHistory()
{
  image = NULL;
  mask = NULL;
  mean = std_dev = n = min = max = sum = 0.0;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::RoiStatistic::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << image->patient->name << " "
     << image->id << " "
     << image->pixel_value_unit->name << " "
     << GetLabels(image->tags) << " "
     << (mask!= NULL? syd::ToString(mask->roitype->name):"no_mask") << " "
     << mean << " " << std_dev << " "  << n << " "
     << min << " " << max << " " << sum;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::InitTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::InitTable(ta);
  if (ta.GetColumn("id") == -1) ta.AddColumn("id");
  ta.AddColumn("p");
  ta.AddColumn("image");
  ta.AddColumn("mask");
  ta.AddColumn("unit");
  ta.AddColumn("tags");
  ta.AddColumn("mean",3);
  ta.AddColumn("sd",3);
  ta.AddColumn("n");
  ta.AddColumn("min",3);
  ta.AddColumn("max",3);
  ta.AddColumn("sum",1);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::DumpInTable(syd::PrintTable & ta) const
{
  syd::RecordWithHistory::DumpInTable(ta);
  ta.Set("id", id);
  ta.Set("p", image->patient->name);
  ta.Set("image", image->id);
  ta.Set("mask", (mask != NULL ? mask->roitype->name:"no_mask"));
  ta.Set("unit", image->pixel_value_unit->name);
  ta.Set("tags", GetLabels(image->tags));
  ta.Set("mean", mean);
  ta.Set("sd", std_dev);
  ta.Set("n", n);
  ta.Set("min", min);
  ta.Set("max", max);
  ta.Set("sum", sum);
}
// --------------------------------------------------
