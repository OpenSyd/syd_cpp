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
  //image = NULL;
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
void syd::RoiMaskImage::CopyFrom(const pointer p)
{
  syd::Image::CopyFrom(p);
  roitype = p->roitype;
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
void syd::RoiMaskImage::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  syd::Image::InitPrintTable(db, ta, format);
  if (format != "file") {
    ta.AddColumn("roi", 15);
  }
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  if (ta.GetNumberOfColumns() == 7)  {
    ta.AddColumn("roi", 15);
  }
  syd::Image::DumpInTable(d, ta, format);
  if (format != "file") {
    ta << roitype->name;
  }
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
