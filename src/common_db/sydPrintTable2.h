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
#include <set>

//--------------------------------------------------------------------
namespace syd {

  struct FormatType;

  /// Helpers class to dump a table (line/column) of values,
  /// controling the precision and the column size
  class PrintTable2 {
  public:
    PrintTable2();

    void Dump(syd::Record::vector::const_iterator start,
              syd::Record::vector::const_iterator end);

    void SetFormat(std::string f);
    void Set(std::string column_name, std::string value, int width_max=50);
    void Set(std::string column_name, double value, int precision=-1);
    void Set(syd::PrintTableColumnInfo::pointer column,
             std::string value);
    void SetSingleRowFlag(bool b);
    void SetHeaderFlag(bool b) { use_header_flag_ = b; }

    void AddFormat(std::string f, std::string description="");

    std::string GetFormat() const;
    syd::PrintTableColumnInfo::pointer GetColumnInfo(std::string column_name);
    syd::PrintTableColumnInfo::pointer GetColumnInfo(int col);
    bool GetSingleRowFlag() const { return use_single_row_flag_; }

  protected:
    std::map<std::string, int> columns_name_to_indices;
    std::vector<syd::PrintTableColumnInfo::pointer> columns;
    syd::PrintTableRow::vector rows_;
    syd::PrintTableRow::pointer current_row_;
    std::string format_;
    std::set<FormatType> formats_;
    bool use_single_row_flag_;
    bool use_header_flag_;

    std::vector<int> GetColumnsIndices();
  };

  struct FormatType {
    std::string name;
    std::string description;
  };

  inline bool operator<(const FormatType& lhs, const FormatType& rhs){
    return lhs.name < rhs.name;
  }


#include "sydPrintTable2.txx"

} // end namespace

#endif /* end #define SYDPRINTTABLE2_H_H */
