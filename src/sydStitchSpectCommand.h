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

#ifndef SYDSTITCHSPECTCOMMAND_H
#define SYDSTITCHSPECTCOMMAND_H

// syd
#include "sydDatabaseCommand.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"
#include "sydDicomCommon.h"

// --------------------------------------------------------------------
namespace syd {

  class StitchSpectCommand: public syd::DatabaseCommand
  {
  public:

    StitchSpectCommand(std::string db);

    typedef float PixelType;
    typedef itk::Image<PixelType, 3> ImageType;

    double get_threshold_cumul() const { return threshold_cumul_; }
    double get_skip_slices() const { return skip_slices_; }
    void set_threshold_cumul(double v) { threshold_cumul_ = v; }
    void set_skip_slices(double v) { skip_slices_ = v; }

    StitchSpectCommand(syd::StudyDatabase * db);
    ~StitchSpectCommand();

    void StitchSpect(std::vector<std::string> serie_ids);
    void StitchSpect(const Serie & serie);

  protected:
    void Initialization();
    double threshold_cumul_;
    double skip_slices_;

    std::shared_ptr<syd::ClinicDatabase> cdb_;
    std::shared_ptr<syd::StudyDatabase>  sdb_;
  };


}  // namespace syd
// --------------------------------------------------------------------

#endif
