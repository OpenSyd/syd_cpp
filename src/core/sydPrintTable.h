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
  };

  struct PrintRow {
    std::vector<std::string> values;
  };



  /// Helpers class to dump a table (line/column) of values, controling the precision and the column size
  class PrintTable {
  public:
    PrintTable();
    void AddColumn(std::string name, int width=-1, int digit=0, bool trunc_by_end_flag=true);
    void Init();
    PrintTable & operator<<(const double & value);
    PrintTable & operator<<(const std::string & value);

    void Set(int col, const std::string & value);
    void Set(int col, const double & value);

    void Set(const std::string & col, const std::string & value);
    void Set(const std::string & col, const double & value);

    void SkipLine();
    void Print(std::ostream & out);
    void SetColumnWidth(int col, int width);
    //    int GetNumberOfColumns() const { return headers.size(); }
    int GetNumberOfColumns() const { return columns_.size(); }
    void Endl();


    bool ColumnsAreDefined(const std::string & table_name);

    void SetColumnsAreDefined(const std::string & table_name);

    template<class RecordType>
    void Dump(const std::vector<std::shared_ptr<RecordType>> & records,
              std::ostream & os=std::cout);

    template<class RecordType>
    void InitTable();

  protected:
    std::vector<std::vector<std::string>> values;
    std::vector<std::string> headers;
    std::vector<int> width;

    std::vector<int> precision;
    std::vector<bool> trunc_by_end;
    int current_line;
    int current_column;


    std::vector<syd::PrintColumn> columns_;
    std::vector<syd::PrintRow> rows_;
    void DumpRow(const syd::PrintRow & row, std::ostream & out);
    std::map<std::string, int> map_column;
    std::map<std::string, bool> map_column_defined;
  };

#include "sydPrintTable.txx"

} // end namespace

#endif /* end #define SYDPRINTTABLE_H_H */
