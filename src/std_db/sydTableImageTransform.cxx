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

#include "sydTableImageTransform.h"
#include "sydDatabase.h"
#include "sydTablePatient.h"

// --------------------------------------------------------------------
// std::string syd::GetRelativeFolder(const syd::Database * db, const syd::ImageTransform & t)
// {
//   if (t.fixed_image == NULL) {
//     LOG(FATAL) << "Could not GetRelativeFolder for this ImageTransfo because fixed_image is null: " << t;
//   }
//   std::string folder = syd::GetRelativeFolder(db, *(t.fixed_image->patient)) + PATH_SEPARATOR + "transform";
//   DD(folder);
//   return folder;
// }
// // --------------------------------------------------------------------

template<>
std::string syd::ComputeRelativeFolder(const syd::Database * db, const syd::ImageTransform & transfo)
{
  if (transfo.fixed_image == NULL) {
    LOG(FATAL) << "Could not ComputeRelativeFolder for this ImageTransfo because fixed_image is null: " << transfo;
  }
  std::string f = ComputeRelativeFolder(db, *transfo.fixed_image->patient)+PATH_SEPARATOR+"transform";
  return f;
}
