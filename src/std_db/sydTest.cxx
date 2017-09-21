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
#include "sydTest_ggo.h"
#include "sydPluginManager.h"
#include "sydDatabaseManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"

#include "sydTestMIRD.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init command line
  SYD_INIT_GGO(sydTest, 0);

  // Load plugin and db
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);
  // -----------------------------------------------------------------

  // test mrd icrp
  /* kidney self-dose factor of 8.03 mGy/MBq⋅s
     To be multplied 3600 s and by the organ volume of 300 mL
     (Adult Male model, assuming 1 g = 1 mL).
  */

  double activity_in_MBq = 1.0;
  std::string organ_name = "Kidneys";
  std::vector<std::string> organ_names;
  organ_names.push_back("Kidney"); // only self dose
  std::string phantom_name = "AM"; // Adult Male
  std::string rad_name = "Lu-177";

  syd::AbsorbedDoseMIRDCalculator * c = new syd::AbsorbedDoseMIRDCalculator;
  c->SetActivity(activity_in_MBq);
  c->SetSourceOrgan(organ_name);
  c->SetTargetOrgan(organ_name); // AddTargetOrgan ?
  c->SetRadionuclide(rad_name);
  c->SetPhantomName(phantom_name);
  c->Run();

  // -----------------------------------------------------------------
  DD("end");
  // This is the end, my friend.
}
// --------------------------------------------------------------------
