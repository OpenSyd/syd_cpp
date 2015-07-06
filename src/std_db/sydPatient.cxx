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
#include "sydPatient.h"
#include "sydInjection.h"
#include "sydStandardDatabase.h"

// --------------------------------------------------
syd::Patient::Patient():syd::Record("")
{
  // default value
  name = "unset"; // must be unique
  study_id = 0; // must be unique
  weight_in_kg = 0;
  dicom_patientid = "unset";
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Patient::ToString() const
{
  std::stringstream ss ;
  ss << id << " "
     << study_id << " "
     << name << " "
     << weight_in_kg << " "
     << dicom_patientid;
  return ss.str();
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::CopyFrom(const pointer p)
{
  syd::Record::CopyFrom(p);
  name = p->name;
  study_id = p->study_id;
  weight_in_kg = p->weight_in_kg;
  dicom_patientid = p->dicom_patientid;
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Patient::IsEqual(const pointer p) const
{
  return (syd::Record::IsEqual(p) and name == p->name and
          study_id == p->study_id and dicom_patientid == p->dicom_patientid);
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(const syd::Database * db, const std::vector<std::string> & arg)
{
  if (arg.size() < 2) {
    LOG(FATAL) << "To insert patient, please set <name> <study_id> [<weight_in_kg> <dicom_patientid>]";
  }
  name = arg[0];
  study_id = atoi(arg[1].c_str());
  if (arg.size() > 2) weight_in_kg = atof(arg[2].c_str());
  if (arg.size() > 3) dicom_patientid = arg[3];
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Set(const syd::Database * db,
                       const std::string & pname,
                       const IdType & pstudy_id,
                       const double pweight_in_kg,
                       const std::string pdicom_patientid)
{
  name = pname;
  study_id = pstudy_id;
  weight_in_kg = pweight_in_kg;
  dicom_patientid = pdicom_patientid;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::InitPrintTable(const syd::Database * db, syd::PrintTable & ta, const std::string & format) const
{
  if (format == "help") {
    std::cout << "Available formats for table 'Patient': " << std::endl
              << "\tdefault: id name study_id weight dicom" << std::endl
              << "\tinjection: add a column with the number of associated injections" << std::endl;
    return;
  }
  ta.AddColumn("#id");
  ta.AddColumn("name", 8);
  ta.AddColumn("sid", 5);
  ta.AddColumn("w(kg)", 10);
  ta.AddColumn("dicom", 20);
  if (format == "injection")
    ta.AddColumn("nb_inj", 10); // advanced dump format, compute the nb of injections
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::DumpInTable(const syd::Database * d, syd::PrintTable & ta, const std::string & format) const
{
  ta << id << name << study_id << weight_in_kg << dicom_patientid;
  if (format == "injection") {
    syd::StandardDatabase* db = (syd::StandardDatabase*)(d);
    syd::Injection::vector injections;
    odb::query<syd::Injection> q = odb::query<syd::Injection>::patient == id;
    db->Query(injections, q);
    ta << injections.size();
  }
}
// --------------------------------------------------


// --------------------------------------------------
bool syd::Patient::CheckIdentity(std::string vdicom_patientid, std::string vdicom_name) const {
  if (dicom_patientid != vdicom_patientid) return false;
  // Try to guess initials. Consider the first letter and the first after the symbol '^'
  int n = vdicom_name.find("^");
  if (n != std::string::npos) {
    std::string initials = vdicom_name[0] + vdicom_name.substr(n+1,1);
    std::transform(initials.begin(), initials.end(), initials.begin(), ::tolower);
    // Check only 2 first letters
    if (initials[0] == name[0] and initials[1] == name[1]) return true;
    return false;
  }
  return false; // do not consider ok with a patient name not having a '^' inside
}
// --------------------------------------------------


// --------------------------------------------------
std::string syd::Patient::ComputeRelativeFolder() const
{
  return name;
}
// --------------------------------------------------


// --------------------------------------------------
void syd::Patient::Sort(syd::Patient::vector & v, const std::string & type)
{
  std::sort(begin(v), end(v),
            [v](pointer a, pointer b) { return a->study_id < b->study_id; });
}
// --------------------------------------------------
