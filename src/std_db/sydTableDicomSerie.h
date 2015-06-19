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

#ifndef SYDTABLEDICOMSERIE_H
#define SYDTABLEDICOMSERIE_H

// syd
#include "sydPrintTable.h"
#include "sydTable.h"
#include "sydDicomSerie-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  /// Specialization of Dump for DicomSeries (sort by acquisition_date)
  template<>
  void syd::Table<syd::DicomSerie>::Dump(std::ostream & os, const std::string & format, const std::vector<syd::IdType> & ids);

  /// Specialization of Dump for DicomSeries
  template<>
  void syd::Table<syd::DicomSerie>::Dump(std::ostream & os, const std::string & format, const std::vector<syd::DicomSerie> & dicoms);

} // namespace syd
// --------------------------------------------------------------------

#endif
