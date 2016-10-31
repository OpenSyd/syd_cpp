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

#include "sydgui.h"

// --------------------------------------------------------------------
void syd::error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydguiInit()
{
  DDF();
  glfwSetErrorCallback(error_callback);
  if (!glfwInit()) {
    using namespace sydlog;
    DDF();
    LOG(FATAL) << "Cannot init glfw gui.";
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
GLFWwindow* syd::sydguiCreateWindow(int width, int height, std::string title)
{
  DDF();
  GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
  glfwMakeContextCurrent(window);
  gl3wInit();
  // Setup ImGui binding
  ImGui_ImplGlfwGL3_Init(window, true);
  return window;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void syd::sydguiRender(ImVec4 & clear_color, GLFWwindow * window)
{
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui::Render();
  glfwSwapBuffers(window);
}
// --------------------------------------------------------------------
