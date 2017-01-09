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
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"
#include "sydTagHelper.h"

DEFINE_TABLE_IMPL(RoiStatistic);

// --------------------------------------------------
syd::RoiStatistic::RoiStatistic():
  syd::Record(),
  syd::RecordWithHistory(),
  syd::RecordWithTags(),
  syd::RecordWithComments()
{
  image = nullptr;
  mask = nullptr;
  mean = std_dev = n = min = max = sum = 0.0;
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::RoiStatistic::ToString() const
{
  std::stringstream ss ;
  ss << id << " ";
  if (image != nullptr) {
    ss << image->patient->name << " "
       << image->id << " "
       << (image->pixel_unit != nullptr? image->pixel_unit->name:empty_value)
       << " ";
  }
  else ss << empty_value;
  ss << syd::GetLabels(tags) << " "
     << (mask != nullptr? mask->roitype->name:"no_mask") << " "
     << mean << " " << std_dev << " "  << n << " "
     << min << " " << max << " " << sum
     << " " << GetAllComments();
  return ss.str();  auto s = ss.str();
  return trim(s);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::DumpInTable(syd::PrintTable & ta) const
{
  ta.Set("id", id);
  ta.Set("p", image->patient->name);
  ta.Set("image", image->id);
  ta.Set("mask", (mask != nullptr ? mask->roitype->name:"no_mask"));
  ta.Set("unit", (image->pixel_unit != nullptr? image->pixel_unit->name:empty_value));
  ta.Set("tags", GetLabels(tags));
  ta.Set("mean", mean, 7);
  ta.Set("sd", std_dev);
  ta.Set("n", n);
  ta.Set("min", min, 7);
  ta.Set("max", max, 7);
  ta.Set("sum", sum, 2);
  ta.Set("com", GetAllComments());
  syd::RecordWithHistory::DumpInTable(ta);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::Callback(odb::callback_event event,
                                 odb::database & db) const
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::Callback(odb::callback_event event,
                                 odb::database & db)
{
  syd::Record::Callback(event,db);
  syd::RecordWithHistory::Callback(event,db, db_);
}
// --------------------------------------------------
