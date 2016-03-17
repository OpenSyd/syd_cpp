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
#include "sydUpdateRadionuclide_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydStandardDatabase.h"
#include "sydCommonGengetopt.h"

// syd init
SYD_STATIC_INIT

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(sydUpdateRadionuclide, 1);

  // Load plugin
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();

  // Get the database
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Get the radionuclide name
  std::string radname = args_info.inputs[0];
  // FIXME later do it with 'all'
  std::vector<std::string> radnames;
  if (radname == "all") {
    syd::Radionuclide::vector rad;
    db->Query(rad);
    for(auto r:rad) radnames.push_back(r->name);
  }
  else radnames.push_back(radname);

  for(auto radname:radnames) {

    // Already exist or not ?
    odb::query<syd::Radionuclide> q = odb::query<syd::Radionuclide>::name == radname;
    syd::Radionuclide::pointer r;
    try {
      db->QueryOne(r, q);
    } catch (std::exception & e) {
      LOG(1) << "Radionuclide '" << radname << "' not exist. We create it.";
      db->New(r);
      r->name = radname;
      db->Insert(r);
    }

    // Connect to default LNHB data site and get txt file
    std::string path = args_info.path_arg + radname + ".lara.txt";
    char const* param[] = { args_info.url_arg, path.c_str() };
    std::stringstream iss;
    int a = syd::GetPageContent(param, iss);

    if (a != 0) {
      db->Delete(r);
      LOG(FATAL) << "Error while looking for page " << path;
    }
    std::string page = iss.str();

    // Parse txt file to get info
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
    r->metastable = false;
    if (radname.back() == 'm') {
      r->metastable = true;
      radname.pop_back(); // remove 'm'
    }
    std::size_t found = radname.find_last_of("-");
    A = atoi(radname.substr(found+1).c_str());

    // Update
    r->half_life_in_hours = HL;
    r->element = element;
    r->atomic_number = Z;
    r->mass_number = A;
    r->max_beta_minus_energy_in_kev = Qminus;
    db->Update(r);
    LOG(1) << "Updating Radionuclide: " << r;
  }

  // This is the end, my friend.
}
// --------------------------------------------------------------------
