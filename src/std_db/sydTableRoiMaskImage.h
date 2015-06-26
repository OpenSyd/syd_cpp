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

#ifndef SYDTABLEROIMASKIMAGE_H
#define SYDTABLEROIMASKIMAGE_H

// syd
#include "sydRoiMaskImage.h"

// --------------------------------------------------------------------
namespace syd {

  template<class RoiMaskImage>
  void FindRoiMaskImage(RoiMaskImage & mask, syd::Database * db, syd::Patient & patient,
                        syd::RoiType & roitype, syd::DicomSerie & dicom);

  template<class RoiMaskImage>
  void FindRoiMaskImage(RoiMaskImage & mask, syd::Database * db, syd::Patient & patient,
                        syd::RoiType & roitype, syd::Image & image);

  template<>
  std::string ComputeRelativeFolder(const syd::Database * db, const syd::RoiMaskImage & mask);

#include "sydTableRoiMaskImage.txx"
}
// --------------------------------------------------------------------

#endif
