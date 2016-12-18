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
#include "syd_gui_images_ggo.h"
#include "sydDatabaseManager.h"
#include "sydPluginManager.h"
#include "sydCommonGengetopt.h"
#include "sydStandardDatabase.h"

// imgui + glfw
#include "sydgui.h"
#include "sydguiImagesSelectionWidget.h"
#include "sydguiImagesListWidget.h"
#include "sydguiImageWidget.h"

// --------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Init
  SYD_INIT_GGO(syd_gui_images, 0);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // Init gui
  sydgui::InitGUI();
  auto window = sydgui::CreateMainWindow(1280, 720, "syd images selector");

  // Main loop
  ImVec4 clear_color = ImColor(114, 144, 154);
  while (!glfwWindowShouldClose(window)) {
    if (!sydgui::StartNewFrame(window)) continue;

    // Begin windows
    ImGui::Begin("syd image selector");

    // Panel1: input images selection
    static sydgui::ImagesSelectionWidget widget_images(db);
    auto changed = widget_images.NewFrame();

    ImGui::Separator();
    auto & images = widget_images.GetImages(); // reference (no copy)

    // End windows
    ImGui::End();

    // Begin windows
    /*
    ImGui::Begin("Selected images");
    //ImGui::Text("%lu images", images.size());
    static sydgui::ImagesListWidget widget_list(images);
    if (changed) widget_list.SetImages(images);
    widget_list.NewFrame();
    ImGui::End();
    */

    // Begin windows
    ImGui::Begin("Selected image");
    static sydgui::ImageWidget widget_image;
    //    if (changed) widget_image.SetImage(widget_images.GetSelectedImage());
    widget_image.NewFrame(widget_images.GetSelectedImage());
    ImGui::End();

    // Begin windows 2 TEST FIXME
    /*ImGui::Begin("Selected image 2");
    sydgui::ImageWidget2(widget_images.GetSelectedImage());
    ImGui::End();
    */

    // Rendering
    sydgui::Render(clear_color, window);
  }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
