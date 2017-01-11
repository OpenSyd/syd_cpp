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
#include "sydRoiMaskImageTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(RoiMaskImage);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
/*
template<> void syd::RecordTraits<syd::RoiMaskImage>::
Sort(syd::RoiMaskImage::vector & v, const std::string & type) const
{
  auto temp = syd::ConvertToVectorOfRecords(v);
  syd::RecordTraits<syd::Image>::GetTraits()->Sort(temp, type);
  v = syd::CastFromVectorOfRecords<syd::RoiMaskImage>(temp);
  }*/
// --------------------------------------------------------------------


