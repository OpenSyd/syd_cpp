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
#include "sydUpdateRadionuclideFilter.h"

// --------------------------------------------------------------------
syd::UpdateRadionuclideFilter::UpdateRadionuclideFilter(syd::StandardDatabase * db)
  :DatabaseFilter(db)
{
  SetURL("www.nucleide.org", "/DDEP_WG/Nuclides/");
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Radionuclide::pointer
syd::UpdateRadionuclideFilter::Update(const std::string & rad_name)
{
  auto r = GetOrCreate(rad_name);
  GetDataFromWeb(r);
  db_->Update(r);
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Radionuclide::vector
syd::UpdateRadionuclideFilter::Update(const std::vector<std::string> & rad_names)
{
  syd::Radionuclide::vector rads;
  for(auto radname:rad_names)
    rads.push_back(Update(radname));
  return rads;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::Radionuclide::pointer syd::UpdateRadionuclideFilter::GetOrCreate(const std::string & rad_name)
{
  syd::Radionuclide::pointer r;
  odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == rad_name;
  try {
    db_->QueryOne(r, q);
  } catch (std::exception & e) {
    LOG(1) << "Radionuclide '" << rad_name << "' not exist. We create it.";
    db_->New(r);
    r->name = rad_name;
    db_->Insert(r);
  }
  return r;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::UpdateRadionuclideFilter::SetURL(const std::string & url, const std::string & path)
{
  url_ = url;
  path_ = path;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::UpdateRadionuclideFilter::GetDataFromWeb(syd::Radionuclide::pointer rad)
{
  // Connect to default LNHB data site and get txt file
  std::string p = path_ + rad->name + ".lara.txt";
  char const* param[] = { url_.c_str(), p.c_str() };
  std::stringstream iss;
  int a = syd::GetPageContent(param, iss);

  if (a != 0) {
    db_->Delete(rad);
    LOG(FATAL) << "Error while looking for page " << path_;
  }
  std::string page = iss.str();

  // Parse txt file to get info
  std::string rad_name = rad->name;
  std::string line;
  double Z;
  double HL;
  double A;
  std::string element;
  double Qminus=0.0;
  while(std::getline(iss, line)) {
    std::string w1;
    std::vector<std::string> w;
    syd::GetWords(w, line);

    int i=0;
    while (i<w.size()) {
      w1 = w[i]; // first word

      // Z
      if (w1 == "Z") Z = atoi(w[i+2].c_str());

      // HL
      if (w1 == "Half-life") {
        std::string unity = w[i+1];
        HL = atof(w[i+3].c_str());
        if (unity == "(d)") HL*=24.0;
        else { if (unity == "(s)") HL/=3600;
          else { if (unity == "(min)") HL /= 60;
            else { if (unity != "(h)") {
                LOG(FATAL) << "I dont know the unity : " << unity << " for the Half-life."
                           << std::endl << page;
              }
            }
          }
        }
      }

      if (w1 == "Element") element = w[i+2];
      if (w1 == "Q-") Qminus = atof(w[i+2].c_str());
      i++;
    }
  }

  // Get 'A'
  rad->metastable = false;
  if (rad_name.back() == 'm') {
    rad->metastable = true;
    rad_name.pop_back(); // remove 'm'
  }
  std::size_t found = rad_name.find_last_of("-");
  A = atoi(rad_name.substr(found+1).c_str());

  // Update
  rad->half_life_in_hours = HL;
  rad->element = element;
  rad->atomic_number = Z;
  rad->mass_number = A;
  rad->max_beta_minus_energy_in_kev = Qminus;
  // db_->Update(rad);
  // LOG(1) << "Updating Radionuclide: " << rad;
}
// --------------------------------------------------------------------
