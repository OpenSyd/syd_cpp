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
syd::RoiStatistic::RoiStatistic():syd::Record()
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
     << image->patient->name
     << image->id << " "
     << image->pixel_value_unit->name
     << GetLabels(image->tags)
     << mask->id << " "
     << mean << " " << std_dev << " "  << n << " "
     << min << " " << max << " " << sum;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::RoiStatistic::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and
          image->id == p->image->id and
          mask->id == p->mask->id and
          mean == p->mean and
          std_dev == p->std_dev and
          n == p->n and
          min == p->min and
          max == p->max and
          sum == p->sum);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  image = p->image;
  mask = p->mask;
  mean = p->mean;
  std_dev = p->std_dev;
  n = p->n;
  min = p->min;
  max = p->max;
  sum = p->sum;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert a RoiStatistic, please use sydInsertRoiStatistic";
}
// --------------------------------------------------



// --------------------------------------------------
void syd::RoiStatistic::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'RoiStatistic': " << std::endl
              << "\tdefault: id name description" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("p", 8);
  ta.AddColumn("image", 8);
  ta.AddColumn("mask", 10);
  ta.AddColumn("unit", 12);
  ta.AddColumn("tags", 40);
  ta.AddColumn("mean", 15,3);
  ta.AddColumn("sd", 15,3);
  ta.AddColumn("n", 10);
  ta.AddColumn("min", 15,3);
  ta.AddColumn("max", 15,3);
  ta.AddColumn("sum", 20,3);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiStatistic::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << image->patient->name << image->id << mask->roitype->name
     << image->pixel_value_unit->name
     << GetLabels(image->tags)
     << mean << std_dev << n << min << max << sum;
}
// --------------------------------------------------
