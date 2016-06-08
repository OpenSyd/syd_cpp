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

#ifndef SYDPRINTTABLE2_H
#define SYDPRINTTABLE2_H

// syd
#include "sydRecord.h"
#include "sydPrintTableRow.h"
#include "sydPrintTableColumnInfo.h"

// std
#include <map>

//--------------------------------------------------------------------
namespace syd {

  /// Helpers class to dump a table (line/column) of values,
  /// controling the precision and the column size
  class PrintTable2 {
  public:
    PrintTable2();

    void Dump(syd::Record::vector::const_iterator start,
              syd::Record::vector::const_iterator end);

    void Set(std::string column_name, std::string value);
    void Set(std::string column_name, double value, int precision=-1);
    void Set(syd::PrintTableColumnInfo::pointer column,
             std::string value);

    std::string GetFormat() const;
    syd::PrintTableColumnInfo::pointer GetColumnInfo(std::string column_name);
    syd::PrintTableColumnInfo::pointer GetColumnInfo(int col);

  protected:
    std::map<std::string, int> columns_name_to_indices;
    std::vector<syd::PrintTableColumnInfo::pointer> columns;
    syd::PrintTableRow::vector rows_;
    syd::PrintTableRow::pointer current_row_;

    std::vector<int> GetColumnsIndices();

  };

#include "sydPrintTable2.txx"

} // end namespace

#endif /* end #define SYDPRINTTABLE2_H_H */
