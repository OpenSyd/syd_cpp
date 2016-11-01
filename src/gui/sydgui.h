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

#ifndef SYDGUI_H
#define SYDGUI_H

// imgui
#include <imgui.h>
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

// std
#include <string>

// syd
#include "sydDD.h"
#include "sydStandardDatabase.h"

namespace sydgui {

  /// Callback for error messages
  void error_callback(int error, const char* description);

  /// Main function to init the GUI
  void InitGUI();

  /// Create the initial main window
  GLFWwindow * CreateMainWindow(int width, int height, std::string title);

  /// To call at the end of main loop
  void Render(ImVec4 & color, GLFWwindow * window);

  ///
  syd::Image::vector GetInputImages(syd::Patient::pointer patient);

  ///
  syd::RoiMaskImage::vector GetRoiMaskImages(syd::Image::pointer image);

  ///
  syd::RoiStatistic::vector GetTimeIntegratedActivity(syd::Image::vector images,
                                                      syd::RoiMaskImage::vector rois);

  ///
  void DisplayRoiStatisics(syd::RoiStatistic::vector stats);

} // end namespace

#endif
