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
#include "sydTableRoiMaskImage.h"
#include "sydDatabasePath.h"

// --------------------------------------------------------------------
template<>
std::string syd::ComputeRelativeFolder(const syd::Database * db, const syd::RoiMaskImage & mask)
{
  if (mask.image == NULL) {
    LOG(FATAL) << "Could not CreateRelativeFolder for this RoiMaskImage because image is null: " << mask;
  }
  std::string f = ComputeRelativeFolder(db, *mask.image->patient)+PATH_SEPARATOR+"roi";
  return f;
}
// --------------------------------------------------------------------
