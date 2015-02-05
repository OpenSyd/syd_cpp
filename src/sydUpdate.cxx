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

// syd
#include "sydUpdate_ggo.h"
#include "core/sydCommon.h"
#include "sydClinicDatabase.h"
#include "sydStudyDatabase.h"

// easylogging : only once initialization (in the main)
INITIALIZE_EASYLOGGINGPP

// syd : only once initialization (in the main)
#include "sydInit.h"

using namespace syd;

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  GGO(sydUpdate, args_info);

  // Init logging option (verbose)
  syd::init_logging_verbose_options(args_info);

  // Update rawimage.patient_id
  std::string db_name = args_info.inputs[0];
  DD(db_name);
  std::shared_ptr<StudyDatabase> sdb =
    Database::OpenDatabaseType<StudyDatabase>(db_name);

  // Update patient id
  if (0) {
    std::vector<Timepoint> timepoints;
    sdb->LoadVector<Timepoint>(timepoints);
    for(auto i:timepoints) {
      Patient patient(sdb->GetPatient(i));
      std::cout << i << std::endl;
      RawImage spect(sdb->GetById<RawImage>(i.spect_image_id));
      RawImage ct(sdb->GetById<RawImage>(i.ct_image_id));
      spect.patient_id = patient.id;
      ct.patient_id = patient.id;
      sdb->Update(spect);
      sdb->Update(ct);
    }
  }

  // Update RoiMaskImage
  if (0) {
    std::vector<RoiMaskImage> rois;
    sdb->LoadVector<RoiMaskImage>(rois);
    for(auto i:rois) {
      Timepoint t(sdb->GetById<Timepoint>(i.timepoint_id));
      Patient patient(sdb->GetPatient(t));
      std::cout << i << std::endl;
      RawImage mask(sdb->GetById<RawImage>(i.mask_id));
      mask.patient_id = patient.id;
      sdb->Update(mask);
    }
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
