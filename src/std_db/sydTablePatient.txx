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


// --------------------------------------------------------------------
template<class Patient>
void FindPatient(Patient & p, const syd::Database * db, const std::string & arg) {
  odb::query<Patient> q =
    odb::query<Patient>::name == arg or
    odb::query<Patient>::study_id == atoi(arg.c_str());
  try {
    p = db->QueryOne(q);
  }
  catch(std::exception & e) {
    LOG(FATAL) << "Could not find patient with name or study_id equal to '" << arg << "'."
               << std::endl << e.what();
  }
}
// --------------------------------------------------------------------



// --------------------------------------------------------------------
template<class Patient>
void FindPatients(std::vector<Patient> & patients, const syd::Database * db, const std::string & arg)
{
  if (arg == "all") return db->Query(patients);
  std::vector<std::string> n;
  syd::GetWords(arg, n);
  for(auto a:n) {
    Patient p;
    syd::FindPatient(p, db, arg);
    patients.push_back(p);
  }
}
// --------------------------------------------------------------------
