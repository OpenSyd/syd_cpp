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

#ifndef SYDTABLEIMAGE_H
#define SYDTABLEIMAGE_H

// syd
#include "sydPrintTable.h"
#include "sydTable.h"
#include "sydImage-odb.hxx"
#include "sydDatabasePath.h"

// --------------------------------------------------------------------
namespace syd {

  /// Specialization of Dump for Images (sort by acquisition_date)
  template<>
  void syd::Table<syd::Image>::Dump(std::ostream & os, const std::string & format, const std::vector<syd::IdType> & ids);

  /// Specialization of Dump for Images
  template<>
  void syd::Table<syd::Image>::Dump(std::ostream & os, const std::string & format, const std::vector<syd::Image> & images);

  template<class Image>
  void FindImages(std::vector<Image> & images, syd::Database * db, syd::Patient & patient, std::vector<syd::Tag> & tags);

  //  std::string GetAbsolutePath(const syd::Database *db, const syd::Image & image);

  // template<>
  // std::string GetRelativeFolder<syd::Image>(syd::Database * db, const syd::Image & e);

  template<>
  std::string GetRelativeFilePath<syd::Image>(const syd::Database * db, const syd::Image & e);


  template<>
  std::string ComputeRelativeFolder(const syd::Database * db, const syd::Image & e);

#include "sydTableImage.txx"

} // namespace syd
// --------------------------------------------------------------------

#endif
