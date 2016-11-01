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
#include "syd_gui_auc_by_roi_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"

// imgui + glfw
#include "sydgui.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_gui_auc_by_roi, 0);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // test
  syd::Image::vector images;
  syd::Image::vector results;
  db->Query(images);
  //for(auto p:patients) std::cout << p << std::endl;

  // Init gui
  sydgui::InitGUI();
  auto window = sydgui::CreateMainWindow(1280, 720, "syd gui test");

  //
  char filter_input_text[256];
  *filter_input_text = 'a';
  char previous_filter_input_text[256];
  //  char * str = filter_input_text.c_str();
  std::vector<std::string> patterns;
  std::vector<std::string> exclude;

  char label_id[32];
  char label_pixel_type[64];

  // data
  // syd::Images::vector images;
  syd::RoiMaskImage::vector rois;
  // syd::RoiStatistic::vector stats;
  syd::Patient::pointer patient;
  db->QueryOne(patient, 6);
  DD(patient);

  // Main loop
  ImVec4 clear_color = ImColor(114, 144, 154);
  while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();
      ImGui_ImplGlfwGL3_NewFrame();

      ImGui::Begin("TIA");

      // Panel1: input images selection
      auto images = sydgui::GetInputImages(patient);

      // Panel2: rois selection
      if (images.size() > 0) {
        if (ImGui::CollapsingHeader("ROI selection"))
          rois = sydgui::GetRoiMaskImages(images[0]);
      }

      /// Panel3: 
      auto stats = sydgui::GetTimeIntegratedActivity(images, rois);

      // Panel4: results
      sydgui::DisplayRoiStatisics(stats);

      ImGui::End();

      // Rendering
      sydgui::Render(clear_color, window);
    }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
