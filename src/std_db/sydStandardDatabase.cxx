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
#include "sydStandardDatabase.h"
#include "sydUpdateRadionuclideFilter.h"
#include "sydDatabaseManager.h"

// --------------------------------------------------------------------
template<>
void syd::Table<syd::Image>::Sort(syd::Image::vector & v, const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return a->acquisition_date < b->acquisition_date;
            });
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
template<>
void syd::Table<syd::Injection>::Sort(syd::Injection::vector & v,
                                      const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return a->date < b->date;
            });
}
// --------------------------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::RoiMaskImage>::Sort(syd::RoiMaskImage::vector & v,
                                         const std::string & type) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->dicoms.size() == 0) return true;
              if (b->dicoms.size() == 0) return false;
              return a->acquisition_date < b->acquisition_date;
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::DicomSerie>::Sort(syd::DicomSerie::vector & v,
                                       const std::string & order) const
{
  // Sort by acquisition_date and if equal, by reconstruction_date or
  // id is equal
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->dicom_acquisition_date == b->dicom_acquisition_date) {
                if (a->dicom_reconstruction_date == b->dicom_reconstruction_date)
                  return (a->id < b->id);
                else return syd::IsDateBefore(a->dicom_reconstruction_date,
                                              b->dicom_reconstruction_date);
              }
              else return syd::IsDateBefore(a->dicom_acquisition_date,
                                            b->dicom_acquisition_date);
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::Patient>::Sort(syd::Patient::vector & v,
                                    const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) { return a->study_id < b->study_id; });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::Radionuclide>::Sort(syd::Radionuclide::vector & v,
                                         const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) { return a->atomic_number < b->atomic_number; });
}
// --------------------------------------------------


// --------------------------------------------------
/*template<>
  void syd::Table<syd::Calibration>::Sort(syd::Calibration::vector & v,
  const std::string & order) const
  {
  std::sort(begin(v), end(v),
  [v](pointer a, pointer b) {
  if (a->image->dicoms.size() == 0) return true;
  if (b->image->dicoms.size() == 0) return false;
  return a->image->acquisition_date < b->image->acquisition_date;
  });
  }*/
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::PixelUnit>::Sort(syd::PixelUnit::vector & v,
                                      const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              return a->name < b->name;
            });
}
// --------------------------------------------------


// --------------------------------------------------
template<>
void syd::Table<syd::RoiStatistic>::Sort(syd::RoiStatistic::vector & v,
                                         const std::string & order) const
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) {
              if (a->image->dicoms.size() == 0) return true;
              if (b->image->dicoms.size() == 0) return false;
              return a->image->acquisition_date < b->image->acquisition_date;
            });
}
// --------------------------------------------------


// --------------------------------------------------
syd::StandardDatabase::~StandardDatabase()
{
}
// --------------------------------------------------

// --------------------------------------------------------------------
void syd::StandardDatabase::CreateTables()
{
  //  syd::Database::CreateTables();
  AddTable<syd::RecordHistory>();
  AddTable<syd::Tag>();
  AddTable<syd::File>();

  AddTable<syd::Patient>();
  AddTable<syd::Injection>();
  AddTable<syd::Radionuclide>();

  AddTable<syd::DicomFile>();
  AddTable<syd::DicomSerie>();

  AddTable<syd::PixelUnit>();
  AddTable<syd::Image>();

  AddTable<syd::RoiType>();
  AddTable<syd::RoiMaskImage>();
  AddTable<syd::RoiStatistic>();

  AddTable<syd::Elastix>();
  // AddTable<syd::Calibration>();
  // AddTable<syd::Timepoints>();
  // AddTable<syd::FitResult>();
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Patient::pointer syd::StandardDatabase::FindPatient(const std::string & name_or_study_id) const
{
  syd::Patient::pointer patient;
  odb::query<syd::Patient> q =
    odb::query<syd::Patient>::name == name_or_study_id or
    odb::query<syd::Patient>::study_id == atoi(name_or_study_id.c_str());
  try {
    QueryOne(patient, q);
  } catch(std::exception & e) {
    EXCEPTION("No patient with name/sid = '" << name_or_study_id << "' found." << std::endl
              << "Error message is: " << e.what());
  }
  return patient;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::StandardDatabase::FindImages(const syd::Patient::pointer patient) const
{
  odb::query<syd::Image> q = odb::query<syd::Image>::patient == patient->id;
  syd::Image::vector images;
  Query(images, q);
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Image::vector syd::StandardDatabase::FindImages(const std::string & patient_name) const
{
  return FindImages(FindPatient(patient_name));
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::StandardDatabase::InsertDefaultRecords(const std::string & def)
{
  if (def == "none") return;

  // Add some radionuclide
  LOG(1) << "Adding some default radionuclides ...";
  /*
    std::vector<std::string> rads = {"C-11", "O-15", "F-18", "P-32", "P-33",
    "Cu-64", "Cu-67", "Ga-68", "Sr-89",
    "Y-90", "Zr-89", "Tc-99m", "Rh-103m",
    "In-111", "Sn-117m", "I-123", "I-124",
    "I-125", "I-131", "Sm-153", "Ho-166",
    "Er-169", "Lu-177", "Re-186", "Re-188",
    "Tl-201", "Pb-212", "Bi-212", "Bi-213",
    "At-211", "Ra-223", "Ac-225", "Th-227"};
    // "Rb-82" ?
    // create  rad
    syd::Radionuclide::vector radionuclides;
    syd::UpdateRadionuclideFilter f(this);
    for(auto rad_name:rads) {
    syd::Radionuclide::pointer r;
    New(r);
    r->name = rad_name;
    f.GetDataFromWeb(r);
    radionuclides.push_back(r);
    }
  */
  syd::Radionuclide::vector radionuclides;
  std::vector<std::vector<std::string>> args;
  args.push_back({"C-11", "0.34", "Carbon", "6", "11", "N", "0.00"});
  args.push_back({"O-15", "0.03", "Oxygen", "8", "15", "N", "0.00"});
  args.push_back({"F-18", "1.83", "Fluorine", "9", "18", "N", "0.00"});
  args.push_back({"P-32", "342.81", "Phosphorus", "15", "32", "N", "1710.66"});
  args.push_back({"P-33", "609.19", "Phosphorus", "15", "33", "N", "248.50"});
  args.push_back({"Cu-64", "12.70", "Copper", "29", "64", "N", "579.40"});
  args.push_back({"Cu-67", "63.84", "Copper", "29", "67 ", "N", "0.00"});
  args.push_back({"Ga-68", "1.13", "Gallium", "31", "68", "N", "0.00"});
  args.push_back({"Sr-89", "1213.67", "Strontium", "38", "89", "N", "1495.10"});
  args.push_back({"Y-90", "64.04", "Yttrium", "39", "90", "N", "2278.70"});
  args.push_back({"Zr-89", "78.42", "Zirconium", "40", "89", "N", "0.00"});
  args.push_back({"Tc-99m", "6.01", "Technetium", "43", "99", "Y", "436.20"});
  args.push_back({"Rh-103m", "0.94", "Rhodium", "45", "103", "Y", "0.00"});
  args.push_back({"In-111", "67.32", "Indium", "49", "111", "N", "0.00"});
  args.push_back({"Sn-117m", "326.39", "Tin", "50", "117", "Y", "0.00"});
  args.push_back({"I-123", "13.22", "Iodine", "53", "123", "N", "0.00"});
  args.push_back({"I-124", "100.22", "Iodine", "53", "124", "N", "0.00"});
  args.push_back({"I-125", "1425.31", "Iodine", "53", "125", "N", "0.00"});
  args.push_back({"I-131", "192.56", "Iodine", "53", "131", "N", "970.80"});
  args.push_back({"Sm-153", "46.29", "Samarium", "62", "153", "N", "807.60"});
  args.push_back({"Ho-166", "26.79", "Holmium", "67", "166", "N", "1854.50"});
  args.push_back({"Er-169", "225.11", "Erbium", "6", "8 169", "N", "353.00"});
  args.push_back({"Lu-177", "159.53", "Lutetium", "71", "177", "N", "498.30"});
  args.push_back({"Re-186", "89.25", "Rhenium", "75", "186", "N", "1069.50"});
  args.push_back({"Re-188", "17.00", "Rhenium", "75", "188", "N", "2120.40"});
  args.push_back({"Tl-201", "73.01", "Thallium", "81", "201", "N", "0.00"});
  args.push_back({"Pb-212", "10.64", "Lead", "82", "212", "N", "569.90"});
  args.push_back({"Bi-212", "1.01", "Bismuth", "83", "212", "N", "2252.10"});
  args.push_back({"Bi-213", "0.76", "Bismuth", "83", "213", "N", "1423.00"});
  args.push_back({"At-211", "7.22", "Astatine", "85", "211", "N", "0.00"});
  args.push_back({"Ra-223", "274.33", "Radium", "88", "223", "N", "0.00"});
  args.push_back({"Ac-225", "240.00", "Actinium", "89", "225", "N", "0.00"});
  args.push_back({"Th-227", "449.23", "Thorium ", "90", "227", "N ", "0.00"});

  for (auto a:args) {
    syd::Radionuclide::pointer r;
    New(r);
    r->Set(a);
    radionuclides.push_back(r);
  }

  Insert(radionuclides);
  LOG(1) << radionuclides.size() << " radionuclides have been added.";

  // Add some default tags
  syd::Tag::vector tags;
  tags.push_back(NewTag("ct", "CT image"));
  tags.push_back(NewTag("spect", "SPECT image"));
  tags.push_back(NewTag("pet", "PET image"));
  tags.push_back(NewTag("init", "Initial data"));
  tags.push_back(NewTag("register", "Registered and warped image"));
  tags.push_back(NewTag("stitch", "Image computed by stitching 2 images"));
  tags.push_back(NewTag("dose", "Dose distribution image"));
  tags.push_back(NewTag("edep", "Edep distribution image"));
  tags.push_back(NewTag("dose_squared", "Squared dose distribution image (for MC simulations)"));
  tags.push_back(NewTag("edep_squared", "Squared edep distribution image (for MC simulations)"));
  tags.push_back(NewTag("dose_uncertainty", "Dose relative uncertainty distribution image (for MC simulations)"));
  tags.push_back(NewTag("edep_uncertainty", "Edep relative uncertainty distribution image (for MC simulations)"));
  tags.push_back(NewTag("spect_dc", "Decay corrected activity image"));
  tags.push_back(NewTag("activity", "Activity image (calibrated)"));
  tags.push_back(NewTag("S-matrix", "S-matrix image"));
  tags.push_back(NewTag("dose_rate", "Dose rate image"));
  tags.push_back(NewTag("tia", "Time Integrated Activity"));
  tags.push_back(NewTag("mask", "Mask image"));

  tags.push_back(NewTag("fit_r2", "Fit R2 result"));
  tags.push_back(NewTag("fit_best_model", "Fit nb of best model result"));
  tags.push_back(NewTag("fit_nb_iterations", "Fit nb of iterations"));
  tags.push_back(NewTag("fit_success", "Fit binary success/fail"));
  tags.push_back(NewTag("fit_initial_mask", "Fit binary initial mask"));
  tags.push_back(NewTag("fit_params", "Fit 4D parameters"));

  for(auto r:radionuclides) {
    tags.push_back(NewTag(r->name, "Radionuclide " + r->name));
  }
  Insert(tags);
  LOG(1) << tags.size() << " tags have been added.";

  // Add some default PixelUnit
  syd::PixelUnit::vector units;
  units.push_back(NewPixelUnit("no_unit", "Default fake unit when unset."));
  units.push_back(NewPixelUnit("%", "Percentage (such as relative uncertainty)"));
  units.push_back(NewPixelUnit("HU", "Hounsfield Units"));
  units.push_back(NewPixelUnit("counts", "Number of counts (by pixel)"));
  units.push_back(NewPixelUnit("label", "Mask image label"));
  units.push_back(NewPixelUnit("Gy", "Absorbed dose in Gy"));
  units.push_back(NewPixelUnit("cGy", "Absorbed dose in cGy")); // FIXME
  units.push_back(NewPixelUnit("MeV", "Deposited energy in MeV"));

  units.push_back(NewPixelUnit("Bq", "Activity in Bq"));
  units.push_back(NewPixelUnit("Bq_by_IA[MBq]", "Activity in Bq by injected activity in MBq"));
  units.push_back(NewPixelUnit("kBq_by_IA[MBq]°", "Activity in kBq by injected activity in MBq"));
  units.push_back(NewPixelUnit("MBq_by_IA[MBq]°", "Activity in MBq by injected activity in MBq"));

  units.push_back(NewPixelUnit("Bq.h_by_IA[MBq]°", "Time Integrated Activity in Bq by injected activity in MBq"));
  units.push_back(NewPixelUnit("kBq.h_by_IA[MBq]°", "Time Integrated Activity in kBq by injected activity in MBq"));
  units.push_back(NewPixelUnit("MBq.h_by_IA[MBq]°", "Time Integrated Activity in MBq by injected activity in MBq"));

  units.push_back(NewPixelUnit("mGy/Bq.sec", "Dose by cumulated activity"));
  units.push_back(NewPixelUnit("cGy/h/IA[MBq]", "Dose rate in cGy by hour by injected activity"));
  units.push_back(NewPixelUnit("cGy/IA[MBq]", "Dose in cGy by injected activity"));
  units.push_back(NewPixelUnit("cGy/kBq.h/IA[MBq]", "Dose in cGy by tia kBq.h by injected activity (for S matrix)"));

  Insert(units);
  LOG(1) << units.size() << " PixelUnit have been added.";

  // Add some RoiType
  syd::RoiType::vector rois;
  rois.push_back(NewRoiType("body", "Contour of the patient"));
  rois.push_back(NewRoiType("liver", "Contour of the liver"));
  rois.push_back(NewRoiType("spleen", "Contour of the spleen"));
  rois.push_back(NewRoiType("heart", "Contour of the cardiac region"));
  rois.push_back(NewRoiType("left_kidney", "Contour of the left kidney"));
  rois.push_back(NewRoiType("right_kidney", "Contour of the right kidney"));
  rois.push_back(NewRoiType("left_lung", "Contour of the left lung"));
  rois.push_back(NewRoiType("right_lung", "Contour of the right lung"));
  rois.push_back(NewRoiType("lung", "Contour of both lungs"));
  rois.push_back(NewRoiType("bone_marrow", "Contour of the L2-L4 lumbar vertebrae"));
  rois.push_back(NewRoiType("lesion", "Lesion"));
  for(auto i=1; i<30; i++) { // start at 1
    std::ostringstream oss;
    if (i < 10) oss << "lesion0" << i;
    else oss << "lesion" << i;
    rois.push_back(NewRoiType(oss.str(), "Lesion "+std::to_string(i)));
  }
  Insert(rois);
  LOG(1) << rois.size() << " RoiType have been added.";

}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Tag::pointer
syd::StandardDatabase::NewTag(const std::string & name,
                              const std::string & description)
{
  syd::Tag::pointer tag;
  New(tag);
  tag->label = name;
  tag->description = description;
  return tag;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::PixelUnit::pointer
syd::StandardDatabase::NewPixelUnit(const std::string & name,
                                    const std::string & description)
{
  syd::PixelUnit::pointer v;
  New(v);
  v->name = name;
  v->description = description;
  return v;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiType::pointer
syd::StandardDatabase::NewRoiType(const std::string & name,
                                  const std::string & description)
{
  syd::RoiType::pointer v;
  New(v);
  v->name = name;
  v->description = description;
  return v;
}
// --------------------------------------------------------------------
