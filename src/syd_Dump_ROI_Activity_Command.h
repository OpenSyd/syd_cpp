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

#ifndef SYD_DUMP_ROI_ACTIVITY_COMMAND_H
#define SYD_DUMP_ROI_ACTIVITY_COMMAND_H

// syd
#include "sydCommand.h"

namespace syd {

  // --------------------------------------------------------------------
  class Dump_ROI_Activity_Command: public syd::Command {

  public:
    Dump_ROI_Activity_Command(sydQuery & db);

    void SetArgs(char ** inputs, int n);
    void Run();

  protected:
    std::vector<RoiStudy> roistudies;
    std::vector<Study> studies;
    std::vector<RoiType> roitypes;

  };
  // --------------------------------------------------------------------


  class GetActivityFromRoiStudy {
  public:
    GetActivityFromRoiStudy(sydQuery * _db):db(_db) {
      Property prop;
      db->Load<Property>(prop, odb::query<Property>::Name == "CalibrationFactor");
      k = atof(prop.Value.c_str());
      k = 1.0/k;
    }

    bool operator()(int i, int j, unsigned long study_id, unsigned long roitype_id, double & v) {
      RoiStudy roistudy;
      typedef odb::query<RoiStudy> q;
      bool b = db->LoadFirstIfExist<RoiStudy>(roistudy, q::StudyId == study_id && q::RoiTypeId == roitype_id);
      if (b) v = roistudy.TimeIntegratedMeanActivity;

      // Conversion to concentration
      Study study = db->GetById<Study>(study_id);
      double ia = study.InjectedQuantityInMBq;
      double density = roistudy.Density;
      v = (k*v)/(ia*density)*1000.0;

      return b;
    }
    sydQuery * db;
    double k;
  };


  //FIXME to put elsewhere
  class DumpSimpleTable {
  public:
    DumpSimpleTable(int nb_row, int nb_column):nb_row_(nb_row), nb_column_(nb_column) {}

    void set_row_ids(std::vector<unsigned long> & v) { row_ids_ = v; }
    void set_column_ids(std::vector<unsigned long> & v) { column_ids_ = v; }

    void set_row_names(std::vector<std::string> & v) { row_names_ = v; }
    void set_column_names(std::vector<std::string> & v) { column_names_ = v; }
    void set_title(std::string t) { title_ = t; }
    void set_precision(int p) { precision_ = p; }
    void set_column_widths(int f, std::vector<int> & v) { first_column_width_ = f ; column_widths_ = v; }

    int nb_row_;
    int nb_column_;
    int first_column_width_;
    std::vector<unsigned long> row_ids_;
    std::vector<std::string> row_names_;
    std::vector<unsigned long> column_ids_;
    std::vector<int> column_widths_;
    std::vector<std::string> column_names_;
    std::string title_;
    std::vector<std::vector<double> > values_;
    std::vector<std::vector<bool> > value_exist_;
    int precision_;

    void Dump(std::ostream & os) {

      // Check all sizes
      assert(row_ids_.size() == nb_row);
      assert(row_names_.size() == nb_row);
      assert(column_ids_.size() == nb_column);
      assert(column_names_.size() == nb_column);
      assert(column_widths_.size() == nb_column);
      assert(values_.size() == nb_row);
      assert(value_exist_.size() == nb_row);

      // First line is the column header
      os.setf(std::ios::fixed, std::ios::floatfield); // set fixed floating format
      os << std::setw(first_column_width_) << title_;
      for(auto j=0; j<nb_column_; j++) os << std::setw(column_widths_[j]) << column_names_[j];
      os << std::endl;

      // Dump table
      for(auto i=0; i<nb_row_; i++) {
        os << std::setw(first_column_width_) << row_names_[i];
        assert(values_.size() == nb_column);
        assert(value_exist_.size() == nb_column);
        for(auto j=0; j<nb_column_; j++) {
          if (value_exist_[i][j])
            os << std::setw(column_widths_[j]) << std::setprecision(precision_) << values_[i][j];
          else {
            os << std::setw(column_widths_[j]) << "-";
          }
        }
        os << std::endl;
      }

    }

    template<class F>
    void FillValues(F * f) {
      values_.resize(nb_row_);
      value_exist_.resize(nb_row_);
      for(auto i=0; i<nb_row_; i++) {
        values_[i].resize(nb_column_);
        value_exist_[i].resize(nb_column_);
        for(auto j=0; j<nb_column_; j++) {
          double v = 0.0;
          bool b = (*f)(i,j,row_ids_[i], column_ids_[j], v);
          if (b) values_[i][j] = v;
          value_exist_[i][j] = b;
        }
      }
    }


  };




} // end namespace

#endif
