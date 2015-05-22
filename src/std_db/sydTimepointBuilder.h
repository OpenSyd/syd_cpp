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
#include "sydStandardDatabase.h"

// --------------------------------------------------------------------
namespace syd {

  /// This class is used to create a Timepoint. A Timepoint is defined
  /// with an injection, a patient, some tag, some associated dicom
  /// series and images.
  class TimepointBuilder {

  public:
    /// Constructor.
    TimepointBuilder(syd::StandardDatabase * db);

    /// Destructor (empty)
    ~TimepointBuilder() {}

    /// States when guessing if a dicom belong to a timepoint
    enum GuessState { GuessTimepointFound,
                      GuessNotThisTimepoint,
                      GuessSeveralTimepointsFound,
                      GuessDicomSerieAlreadyExist,
                      GuessCreateNewTimepoint };

    /// Set the current tag associated with the timepoint
    void SetTag(syd::Tag & tag);

    /// Max time difference between dicom in a timepoint. Used to
    /// guess if a dicom belong to an already existing timepoint, or
    /// if we should create a new one.
    void SetIntraTimepointMaxHourDiff(double a) { intra_timepoint_max_hour_diff_ = a; }

    /// Try to insert this dicom in a timepoint (heuristic)
    void InsertDicomSerie(syd::DicomSerie & dicom);

    /// Try to guess if the the dicomserie belong to an existing timepoint.
    GuessState GuessTimepointForThisDicomSerie(syd::DicomSerie & dicom,
                                               std::vector<syd::Timepoint> & tp);

    /// Try to guess if the dicomserie could be in this timepoint
    GuessState GuessIfDicomCanBeInThisTimepoint(syd::DicomSerie & dicom,
                                                syd::Timepoint & timepoint);

    /// Create a new timepoint from the current dicomserie
    syd::Timepoint CreateTimepoint(syd::DicomSerie & dicom);

    /// Add the dicom to the given timepoint
    void AddDicom(syd::Timepoint & timepoint, syd::DicomSerie & dicom);

  protected:
    /// Protected constructor. No need to use directly.
    TimepointBuilder();

    /// Set the pointer to the database
    void SetDatabase(syd::StandardDatabase * db) { db_ = db; }

    syd::StandardDatabase * db_;
    syd::Tag tag_;
    double intra_timepoint_max_hour_diff_;

  }; // class TimepointBuilder
} // namespace syd
// --------------------------------------------------------------------

#endif
