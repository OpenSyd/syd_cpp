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
syd::RoiMaskImage::RoiMaskImage():Image()
{
  roitype = NULL;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ToString() const
{
  std::stringstream ss ;
  ss << syd::Image::ToString() << " "
     << roitype->name << " ";
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ComputeRelativeFolder() const
{
  return syd::Image::ComputeRelativeFolder()+PATH_SEPARATOR+"roi";
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::RoiMaskImage::IsEqual(const pointer p) const
{
  bool b = (syd::Image::IsEqual(p) and
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
void syd::RoiMaskImage::Callback(odb::callback_event event, odb::database & db) const
{
  syd::Image::Callback(event,db);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::Callback(odb::callback_event event, odb::database & db)
{
  syd::Image::Callback(event,db);
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::RoiMaskImage::ComputeDefaultAbsolutePath(syd::Database * db) const
{
  // fast initial check (useful but not sufficient)
  if (id == -1) {
    LOG(FATAL) << "Could not compute a default filename for this image, the object is not persistant: " << this;
  }
  std::ostringstream oss;
  oss << roitype->name << "_" << id << ".mhd";
  std::string mhd_filename = oss.str();
  std::string mhd_relative_path = ComputeRelativeFolder()+PATH_SEPARATOR;
  std::string mhd_path = db->ConvertToAbsolutePath(mhd_relative_path+mhd_filename);
  return mhd_path;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::InitTable(syd::PrintTable & ta) const
{
  // Need to check if Image::InitTable has been already called. If not we do it.
   if (ta.GetFormat() == "default") {
    if (ta.GetColumn("id") == -1) syd::Image::InitTable(ta);
    ta.AddColumn("roi", 12);
  }
  if (ta.GetFormat() == "file") {
    if (ta.GetColumn("id") == -1) syd::Image::InitTable(ta);
  }
  if (ta.GetFormat() == "filelist") {
    //if (ta.GetColumn("file") == -1) syd::Image::InitTable(ta);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::DumpInTable(syd::PrintTable & ta) const
{
  syd::Image::DumpInTable(ta);
  if (ta.GetFormat() == "default") {
    // Additional column if needed
    if (ta.GetColumn("roi") == -1) InitTable(ta);
    ta.Set("roi", roitype->name);
  }
}
// --------------------------------------------------
