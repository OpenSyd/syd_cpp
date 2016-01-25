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

#ifndef SYDFIELDDESCRIPTION_H
#define SYDFIELDDESCRIPTION_H

// syd
#include "sydCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class TableDescription;

  /// Describe the content of a Field, from a OO point of view, and
  /// make the link with underlying sql description.
  class FieldDescription {

  public:

    FieldDescription(syd::TableDescription * t);

    void SetName(std::string name, std::string type);
    std::string GetName() const { return name_; }
    std::string GetType() const { return type_; }

    std::string GetSQLTableName() const;
    // void SetSQLTableName(std::string name);

    friend std::ostream& operator<<(std::ostream& os, const FieldDescription & d) { return d.Print(os); }
    friend std::ostream& operator<<(std::ostream& os, const FieldDescription * d) { return d->Print(os); }
    std::ostream & Print(std::ostream & os) const;

  protected:
    syd::TableDescription * table_;
    std::string name_;
    std::string type_;

  };

} // end namespace
// --------------------------------------------------------------------

#endif
