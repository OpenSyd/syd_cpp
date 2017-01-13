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
#include "sydDicomSerieTraits.h"

// --------------------------------------------------------------------
DEFINE_TABLE_TRAITS_IMPL(DicomSerie);
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<> void syd::RecordTraits<syd::DicomSerie>::
BuildMapOfSortFunctions(CompareFunctionMap & map)
{
  MergeRecordMapOfSortFunctions(map);
  // New sort comparison
  auto f = [](pointer a, pointer b) -> bool
    { return a->dicom_acquisition_date < b->dicom_acquisition_date; };
  map["date"] = f;
  map[""] = f; // make it the default
  map["recon_date"] = [](pointer a, pointer b) -> bool
    { return a->dicom_reconstruction_date < b->dicom_reconstruction_date; };
}
// --------------------------------------------------------------------
