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

#ifndef SYDTABLEOFRECORDS_H
#define SYDTABLEOFRECORDS_H

// syd
#include "sydPatient.h"

// --------------------------------------------------------------------
namespace syd {

  class TableOfRecords {
  public:
    TableOfRecords();
    void Set(syd::Record::vector records);
    void Print(std::ostream & os) const;
    void Print(std::ostream & os, syd::Record::pointer r) const;
    void AddField(std::string field_name);

  protected:
    syd::Record::vector records;
    std::vector<std::function<std::string(syd::Record::pointer)>> fields;
  };

} // namespace syd

//#include "sydTableOfRecords.txx"
// --------------------------------------------------------------------

#endif
