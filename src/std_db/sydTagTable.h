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

#ifndef SYDTAGTABLE_H
#define SYDTAGTABLE_H

// syd
#include "sydTag.h"
#include "sydDatabase.h"
#include "sydTag-odb.hxx"

// --------------------------------------------------------------------
namespace syd {

  template<class Tag>
  class TagTable: public syd::Table<Tag> {
  public:

    // Each patient in different folder
    std::string ComputeRelativeFolder(const Tag & p);

    void Set(Tag & e, const std::vector<std::string> & arg);

    //    void FindTag(Tag & p, const std::string & arg);
    //template<class Tag>
    // void FindTags(std::vector<Tag> & tags, syd::Database * db, const std::string & names);
  };

#include "sydTagTable.txx"
}
// --------------------------------------------------------------------

#endif