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
#include "sydTablePatient.h"
#include "sydTableFile.h"

// --------------------------------------------------------------------
template<>
std::string syd::ComputeRelativeFolder(const syd::Database * db,
                                       const syd::ImageTransform & transfo)
{
  if (transfo.fixed_image == NULL) {
    LOG(FATAL) << "Could not ComputeRelativeFolder for this ImageTransfo because fixed_image is null: " << transfo;
  }
  std::string f = ComputeRelativeFolder(db, *transfo.fixed_image->patient)+PATH_SEPARATOR+"transform";
  return f;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
std::string syd::GetRelativePath<syd::ImageTransform>(const syd::Database * db,
                                                      const syd::ImageTransform & e)
{
  if (e.config_file == NULL) {
    LOG(FATAL) << "No associated file with this ImageTransform: " << e;
  }
  return syd::GetRelativePath(db, *e.config_file);
}
// --------------------------------------------------------------------
