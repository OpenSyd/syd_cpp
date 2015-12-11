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

#ifndef SYDPRINTTABLE_H
#define SYDPRINTTABLE_H

// syd
#include "sydCommon.h"

// std
#include <map>

//--------------------------------------------------------------------
namespace syd {


  struct PrintColumn { // FIXME change name TableColumn
    int width;
    std::string title;
    int index;
    int precision=0;
    int max_width=99;
    bool trunc_by_end_flag=false;
  };

  struct PrintRow {
    std::vector<std::string> values;
  };

  struct PrintFormat {
    std::string name;
    std::string help;
  };


  /// Helpers class to dump a table (line/column) of values,
  /// controling the precision and the column size
  class PrintTable {
  public:
    PrintTable();

    void Init();

    syd::PrintColumn & AddColumn(std::string name, int precision=0);
    void AddRow();

    void Set(int col, const std::string & value);
    void Set(int col, const double & value);
    void Set(const std::string & col, const std::string & value);
    void Set(const std::string & col, const double & value);

    void Print(std::ostream & out);

    int GetNumberOfColumns() const { return columns_.size(); }
    int GetColumn(std::string col);

    const std::string & GetFormat() const { return current_format_name_; }
    void AddFormat(std::string name, std::string help);
    void SetFormat(std::string name);
    std::vector<syd::PrintFormat> & GetFormats() { return formats_; }

    template<class RecordType>
    void Dump(const std::vector<std::shared_ptr<RecordType>> & records,
              std::ostream & os=std::cout);

    void SetHeaderFlag(bool b) { header_flag_= b; }
    bool GetHeaderFlag() const { return header_flag_; }

  protected:
    std::vector<std::vector<std::string>> values;
    int current_line;
    int current_column;

    std::string current_table_;
    std::vector<syd::PrintColumn> columns_;
    std::vector<syd::PrintRow> rows_;
    void DumpRow(const syd::PrintRow & row, std::ostream & out);
    std::map<std::string, int> map_column;

    std::string current_format_name_;
    std::vector<syd::PrintFormat> formats_;

    bool header_flag_;

  };

#include "sydPrintTable.txx"

} // end namespace

#endif /* end #define SYDPRINTTABLE_H_H */
