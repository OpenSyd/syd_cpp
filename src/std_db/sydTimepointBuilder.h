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

#ifndef SYDTIMEPOINTBUILDER_H
#define SYDTIMEPOINTBUILDER_H

// syd
#include "sydStudyDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Timepoint. A Timepoint is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class TimepointBuilder {

  public:
    /// Constructor.
    TimepointBuilder(syd::StudyDatabase * db);

    /// Destructor (empty)
    ~TimepointBuilder() {}

    void SetDatabase(syd::StudyDatabase * db) { db_ = db; }

    enum GuessState { GuessFound, GuessNotFound, GuessError };

    void SetTag(syd::Tag & tag);
    void InsertDicomSerie(syd::DicomSerie & dicom);
    GuessState GuessTimepointForThisDicomSerie(syd::DicomSerie & dicom, syd::Timepoint & tp);
    GuessState GuessIfDicomCanBeInThisTimepoint(syd::DicomSerie & dicom, syd::Timepoint & timepoint);
    syd::Timepoint CreateTimepoint(syd::DicomSerie & dicom);
    void AddDicom(syd::Timepoint & timepoint, syd::DicomSerie & dicom);


    void Update();

  protected:
    TimepointBuilder();
    syd::StudyDatabase * db_;
    syd::Tag tag_;
    double intra_timepoint_min_hour_diff_;
    double intra_timepoint_max_hour_diff_;
    double inter_timepoint_min_hour_diff_;

  }; // class TimepointBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
