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

#ifndef SYDDATABASEDESCRIPTION_H
#define SYDDATABASEDESCRIPTION_H

// syd
#include "sydTableDescription.h"

// --------------------------------------------------------------------
namespace syd {

  class Database;

  /// Describe the content of a database, from a OO point of view, and
  /// make the link with underlying sql description.
  class DatabaseDescription {

  public:
    void Init(const syd::Database * db); // FIXME to set const

    void ReadDatabaseSchema(const syd::Database * db);

    void SetInitialized(bool b) { initializedFlag_ = b; }
    bool IsInitialized() const { return initializedFlag_ == true; }
    bool FindTableDescription(std::string table_name, syd::TableDescription ** d);
    bool FindTableDescriptionFromSQLName(std::string table_name, syd::TableDescription ** d);

    std::vector<syd::TableDescription*> & GetTablesDescription() { return tables_; }

    // FIXME pointer + check already exist
    void AddTableDescription(syd::TableDescription * t) { tables_.push_back(t); }

    friend std::ostream& operator<<(std::ostream & os,
                                    const DatabaseDescription & d) { return d.Print(os); }
    friend std::ostream& operator<<(std::ostream & os,
                                    const DatabaseDescription * d) { return d->Print(os); }
    std::ostream & Print(std::ostream & os = std::cout) const;

  protected:
    bool initializedFlag_;
    std::string name;
    std::vector<syd::TableDescription*> tables_;

  }; // end class

} // end namespace
// --------------------------------------------------------------------

#endif
