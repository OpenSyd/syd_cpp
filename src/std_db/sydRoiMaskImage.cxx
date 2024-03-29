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
#include "sydStandardDatabase.h"
#include "sydRecordTraits.h"

DEFINE_TABLE_IMPL(RoiMaskImage);

// --------------------------------------------------------------------
syd::RoiMaskImage::RoiMaskImage():Image()
{
  roitype = nullptr;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ToShortString() const
{
  std::stringstream ss ;
  ss << syd::Image::ToString() << " "
     << (roitype == nullptr ? empty_value:roitype->name) << " ";
  if (dicomsStruct.size() == 0)
    ss << empty_value;
  else {
    for (auto d:dicomsStruct)
      ss << d->id << " ";
  }
  auto s = ss.str();
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiMaskImage::Callback(odb::callback_event event,
                                 odb::database & db) const
{
  syd::Image::Callback(event,db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::RoiMaskImage::Callback(odb::callback_event event,
                                 odb::database & db)
{
  syd::Image::Callback(event,db);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ComputeDefaultRelativePath()
{
  auto s = syd::Image::ComputeDefaultRelativePath();
  s = s+PATH_SEPARATOR+"roi"; // in the 'roi' folder
  return s;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ComputeDefaultMHDFilename()
{
  if (!IsPersistent()) {
    EXCEPTION("Image must be persistent (in the db) to "
              << "use ComputeDefaultMHDFilename.");
  }
  if (roitype == NULL) {
    EXCEPTION("Cannot compute the DefaultMHDFilename "
              << "roitype have to be set before.");
  }
  std::ostringstream oss;
  oss << roitype->name << "_" << id << ".mhd";
  return oss.str();
}
// --------------------------------------------------------------------
