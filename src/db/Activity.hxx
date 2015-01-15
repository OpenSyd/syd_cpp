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

// std
#include <string>
#include <iostream>
#include <memory>

// syd
#include "sydCommon.h"

// odb
#include <odb/core.hxx>

// --------------------------------------------------------------------
namespace syd {
#pragma db object
  class Activity
  {
  public:

#pragma db id auto
    IdType        id;
    IdType        patient_id;
    IdType        roi_type_id;
    std::string   peak_mean_position;
    std::string   peak_std_position;
    double        fit_lambda;
    double        fit_A;
    double        fit_error;
    unsigned int  fit_nb_points;
    std::string   fit_comment;
    double        time_integrated_counts_by_mm3;

    // --------------------------------------------------
    Activity() {
      peak_mean_position = "";
      peak_std_position = "";
      fit_lambda = 0.0;
      fit_A = 0.0;
      fit_error = 0.0;
      fit_nb_points = 0;
      fit_comment = "";
      time_integrated_counts_by_mm3 = 0.0;
    }
    ~Activity() {}
    Activity(const Activity & other) { copy(other); }
    // --------------------------------------------------


    // --------------------------------------------------
    Activity & operator= (const Activity & other) {
      if (this != &other) { copy(other); }
      return *this;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    friend std::ostream& operator<<(std::ostream& os, const Activity & p) {
      os << p.id << " " << p.patient_id << " " << p.roi_type_id << " " << p.peak_mean_position << " " << p.fit_lambda;
      return os;
    }
    // --------------------------------------------------


    // --------------------------------------------------
    void copy(const Activity & t) {
      id = t.id;
      patient_id = t.patient_id;
      roi_type_id = t.roi_type_id;
      peak_mean_position = t.peak_mean_position;
      peak_std_position = t.peak_std_position;
      fit_lambda = t.fit_lambda;
      fit_A = t.fit_A;
      fit_error = t.fit_error;
      fit_nb_points = t.fit_nb_points;
      fit_comment = t.fit_comment;
    }
    // --------------------------------------------------

  }; // class Activity
} // namespace syd
// --------------------------------------------------------------------
