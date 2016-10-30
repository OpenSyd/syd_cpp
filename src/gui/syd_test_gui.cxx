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
#include "syd_test_gui_ggo.h"
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
  SYD_INIT_GGO(syd_test_gui, 0);

  // Load plugin and get the database
  syd::PluginManager::GetInstance()->Load();
  syd::DatabaseManager* m = syd::DatabaseManager::GetInstance();
  syd::StandardDatabase * db = m->Open<syd::StandardDatabase>(args_info.db_arg);

  // test
  syd::Image::vector images;
  syd::Image::vector results;
  db->Query(images);
  //for(auto p:patients) std::cout << p << std::endl;

  // Setup window
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) return 1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  GLFWwindow* window = glfwCreateWindow(1280, 720, "syd gui test", NULL, NULL);
  glfwMakeContextCurrent(window);
  gl3wInit();

  // Setup ImGui binding
  ImGui_ImplGlfwGL3_Init(window, true);

  // Load Fonts
  // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
  //ImGuiIO& io = ImGui::GetIO();
  //io.Fonts->AddFontDefault();
  //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
  //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
  //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
  //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
  //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

  bool show_test_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImColor(114, 144, 154);


  //
  char filter_input_text[256];
  *filter_input_text = 'a';
  char previous_filter_input_text[256];
  //  char * str = filter_input_text.c_str();
  std::vector<std::string> patterns;
  std::vector<std::string> exclude;

  char label_id[32];
  char label_pixel_type[64];

  // Main loop
  while (!glfwWindowShouldClose(window))
    {
      glfwPollEvents();
      ImGui_ImplGlfwGL3_NewFrame();

      // 1. Show a simple window

      // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears
      // in a window automatically called "Debug"
      {
        ImGui::Begin("Hello world");
        //static float f = 0.0f;
        ImGui::Text("Hello, world!");
        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //ImGui::Text("float value %.3f",f);

        // Filter box
        ImGui::InputText("input text", filter_input_text, 256);
        ImGui::Text("Input text filter %s", filter_input_text);
        if (strcmp(filter_input_text, previous_filter_input_text) != 0) {
          // images.clear();
          // db->Query(images);
          // make it too slow --> change to only update every x seconds.
          patterns.clear();
          results.clear();
          patterns.push_back(filter_input_text);git
          DDS(patterns);
          db->Grep(results, images, patterns, exclude);
          DD(results.size());
          strcpy(previous_filter_input_text, filter_input_text);
        }

        // trial table
        ImGui::Columns(2, "mycolumns2", false);  // 2-ways, no border
        ImGui::Separator();
        int max=10;
        for (auto image:results) {
          sprintf(label_id, "Id %i", (int)image->id);
          if (ImGui::Selectable(label_id)) {}
          //if (ImGui::Button(label, ImVec2(-1,0))) {}
          ImGui::NextColumn();
          sprintf(label_pixel_type, "%s", image->pixel_type.c_str());
          if (ImGui::Selectable(label_pixel_type)) {}
          ImGui::NextColumn();
        }
        ImGui::Columns(1); // back to 1 column
        ImGui::Separator();

        ImGui::End();
      }

      // Rendering
      int display_w, display_h;
      glfwGetFramebufferSize(window, &display_w, &display_h);
      glViewport(0, 0, display_w, display_h);
      glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
      glClear(GL_COLOR_BUFFER_BIT);
      ImGui::Render();
      glfwSwapBuffers(window);
    }

  // Cleanup
  ImGui_ImplGlfwGL3_Shutdown();
  glfwTerminate();

  // This is the end, my friend.
}
// --------------------------------------------------------------------
