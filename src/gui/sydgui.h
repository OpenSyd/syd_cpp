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
#include "sydLog.h"

namespace syd {

  void error_callback(int error, const char* description);

  void sydguiInit();

  GLFWwindow * sydguiCreateWindow(int width, int height, std::string title);

  void sydguiRender(ImVec4 & color, GLFWwindow * window);

} // end namespace

#endif
