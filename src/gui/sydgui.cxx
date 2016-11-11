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
#include "sydRoiMaskImageHelper.h"
#include "sydRoiStatisticHelper.h"

// FIXME to remove
#include "sydguiFitOptionsWidget.h"

// Boost vector of bool (allow reference)
#include <boost/container/vector.hpp>

// --------------------------------------------------------------------
void sydgui::error_callback(int error, const char* description)
{
  fprintf(stderr, "Error %d: %s\n", error, description);
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::InitGUI()
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
bool sydgui::StartNewFrame(GLFWwindow * window)
{
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
    glfwWaitEvents();
    return false;
  }

  if (!glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
    glfwWaitEvents();
    return false;
  }
  glfwPollEvents();
  ImGui_ImplGlfwGL3_NewFrame();
  return true;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
GLFWwindow* sydgui::CreateMainWindow(int width, int height, std::string title)
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
void sydgui::Render(ImVec4 & clear_color, GLFWwindow * window)
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


// --------------------------------------------------------------------
syd::Image::vector sydgui::GetInputImages(syd::Patient::pointer patient)
{
  static syd::Image::vector images;
  if (images.size() == 0) {
    syd::Image::pointer im;
    auto db = patient->GetDatabase<syd::StandardDatabase>();
    db->QueryOne(im, 331); images.push_back(im);
    db->QueryOne(im, 332); images.push_back(im);
    db->QueryOne(im, 333); images.push_back(im);
    db->QueryOne(im, 334); images.push_back(im);
    db->QueryOne(im, 335); images.push_back(im);
    DDS(images);
  }
  return images;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiMaskImage::vector sydgui::GetRoiMaskImages(syd::Image::pointer image)
{
  static syd::RoiMaskImage::vector rois;
  static boost::container::vector<bool> selected;
  syd::RoiMaskImage::vector selected_rois;
  if (rois.size() == 0) {
    rois = syd::FindAllRoiMaskImage(image);
    selected.resize(rois.size());
    for(auto i=0; i<rois.size(); i++) selected[i] = false;
    DDS(selected);
  }

  ImGui::Text("Rois");
  int i=0;
  ImGui::Columns(3);
  for(auto roi:rois) {
    ImGui::PushID(i); // require to separate every checkbox ROI
    ImGui::Checkbox("ROI", &selected[i]);
    ImGui::SameLine();
    ImGui::Text("%i Id %lu : %s %d", i, roi->id, roi->roitype->name.c_str(), selected[i]);
    if (selected[i]) selected_rois.push_back(roi);
    ++i;
    ImGui::PopID();
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
  return selected_rois;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
syd::RoiStatistic::vector
sydgui::GetRoiStatistics(const syd::Image::vector images,
                         const syd::RoiMaskImage::vector rois,
                         const syd::TimeIntegratedActivityFitOptions & options)
{
  syd::RoiStatistic::vector stats;
  if (images.size() == 0) return stats; // should not happen
  if (rois.size() == 0) return stats; // should not happen

  auto db = images[0]->GetDatabase<syd::StandardDatabase>();

  // Get FitImages
  syd::FitImages::vector fis;
  syd::FitImages::vector sfis;
  db->Query(fis);
  DD(fis.size());
  DD(options.ToString());
  for(auto fi:fis) {
    DD(fi->GetOptions().ToString());
    if (fi->images == images and
        fi->GetOptions().ToString().compare(options.ToString()) == 0)
      sfis.push_back(fi);
  }
  DD(sfis.size());

  // Get RoiStatistic
  for(auto fi:sfis) {
    for(auto r:rois) {
      auto ss = syd::FindRoiStatistic(fi->GetOutput("fit_auc"), r);
      for(auto s:ss) stats.push_back(s);
    }
  }
  return stats;
}
// --------------------------------------------------------------------


// --------------------------------------------------------------------
void sydgui::DisplayRoiStatistics(syd::RoiStatistic::vector stats)
{
  ImGui::Text("Hello, world!");

  for(auto stat:stats) {
    ImGui::Text("Id %lu : %f ", stat->id, stat->mean);
  }
}
// --------------------------------------------------------------------
