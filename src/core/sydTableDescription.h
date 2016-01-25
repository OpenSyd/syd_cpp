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

#ifndef SYDTABLEDESCRIPTION_H
#define SYDTABLEDESCRIPTION_H

// syd
#include "sydCommon.h"
#include "sydFieldDescription.h"

// --------------------------------------------------------------------
namespace syd {

  /// Describe the content of a Table, from a OO point of view, and
  /// make the link with underlying sql description.
  class TableDescription {

  public:

    bool FindField(std::string field_name, syd::FieldDescription ** f);
    std::vector<syd::FieldDescription*> & GetFields() { return fields_; }

    std::string GetTableName() const { return table_name_; }
    std::string GetSQLTableName() const { return sql_table_name_; }

    void SetTableName(std::string t) { table_name_ = t; }
    void SetSQLTableName(std::string t) { sql_table_name_ = t; }

    void AddField(std::string name, std::string type);
    void AddField(const syd::FieldDescription * f);

    friend std::ostream& operator<<(std::ostream& os,
                                    const TableDescription & d) { return d.Print(os); }
    friend std::ostream& operator<<(std::ostream& os,
                                    const TableDescription * d) { return d->Print(os); }
    std::ostream & Print(std::ostream & os) const;

  protected:
    std::string table_name_;
    std::string sql_table_name_;
    std::vector<syd::FieldDescription*> fields_;
  };

} // end namespace
// --------------------------------------------------------------------

#endif
