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
#include "sydRoiMaskImage.h"
#include "sydDatabase.h"
#include "sydTable.h"
#include "sydImage.h"
#include "sydImage-odb.hxx"

// --------------------------------------------------------------------
syd::RoiMaskImage::RoiMaskImage():Record("")
{
  image = NULL;
  roitype = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ToString() const
{
  std::string i = "unset";
  if (image != NULL) i = image->ToString();
  std::stringstream ss ;
  ss << id << " "
     << roitype->name << " "
     << i;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  image = p->image;
  roitype = p->roitype;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::RoiMaskImage::IsEqual(const pointer p) const
{
  bool b = (syd::Record::IsEqual(p) and
            image->id == p->image->id and
            roitype->id == p->roitype->id);
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  LOG(FATAL) << "To insert a RoiMaskImage, use sydInsertRoiMaskImage";
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Image': " << std::endl
              << "\tdefault: id patient tags size spacing dicoms" << std::endl
              << "\tfile: full path" << std::endl;
    return;
  }
  if (format == "file") {
    ta.AddColumn("#file", 100);
  }
  else {
    ta.AddColumn("#id", 5);
    ta.AddColumn("p", 8);
    ta.AddColumn("roi", 15);
    ta.AddColumn("tags", 20);
    ta.AddColumn("size", 12);
    ta.AddColumn("spacing", 25);
    ta.AddColumn("dicom_id", 15);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
   if (format == "file") {
     if (image->files.size() == 0) {
       ta << "no_file";
       return;
     }
     //const syd::StandardDatabase * db = std::dynamic_cast<const syd::StandardDatabase*>(d);
     ta << d->ConvertToAbsolutePath(image->files[0]->path+PATH_SEPARATOR+image->files[0]->filename);
   }
   else {
     ta << id << image->patient->name << roitype->name << GetTagLabels(image->tags)
        << syd::ArrayToString<int, 3>(image->size) << syd::ArrayToString<double, 3>(image->spacing);
     std::string dicom;
     for(auto d:image->dicoms) dicom += syd::ToString(d->id)+" ";
     ta << dicom;
   }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::Sort(syd::RoiMaskImage::vector & v, const std::string & type)
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->image->dicoms.size() == 0) return true;
              if (b->image->dicoms.size() == 0) return false;
              return a->image->dicoms[0]->acquisition_date <
                b->image->dicoms[0]->acquisition_date;
            });
}
// --------------------------------------------------
