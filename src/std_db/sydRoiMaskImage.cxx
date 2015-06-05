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
syd::RoiMaskImage::RoiMaskImage():TableElementBase()
{
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
std::string syd::RoiMaskImage::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << roitype->name << " "
     << *image;
  return ss.str();
}
// --------------------------------------------------------------------


// --------------------------------------------------
bool syd::RoiMaskImage::operator==(const RoiMaskImage & p)
{
  bool b = (id == p.id and
            *image == *p.image and
            *roitype == *p.roitype);
  return b;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::RoiMaskImage::OnDelete(syd::Database * db)
{
  //  LOG(FATAL) << "TODO OnDelete for RoiMaskImage not yet implemented.";
  db->AddToDeleteList(*image); //?? -> should I delete the associated image ?
}
// --------------------------------------------------
