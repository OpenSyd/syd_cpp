

find_package(OpenGL REQUIRED)
# find_package(glfw3 REQUIRED CONFIG)

#ImGui lib
add_library(imgui
  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_impl_glfw_gl3.cpp
  libs/gl3w/GL/gl3w.c
  )

# syd + ImGui lib
add_library(sydgui
  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_impl_glfw_gl3.cpp
  libs/gl3w/GL/gl3w.c
  sydgui.h
  )

## reference example
include_directories(libs/gl3w)
add_executable(imgui_opengl3_example
  main.cpp
  )
target_link_libraries(imgui_opengl3_example
  imgui
  ${OPENGL_LIBRARIES}
  glfw3
  ${SYD_STD_DB_LIBRARY})

# test gui
wrap_ggo(syd_test_gui_GGO_C
  syd_test_gui.ggo
  ../common_db/sydCommandLineDefault.ggo)
add_executable(syd_test_gui
  syd_test_gui.cxx
  ${syd_test_gui_GGO_C})

target_link_libraries(syd_test_gui
  sydgui
  ${OPENGL_LIBRARIES}
  glfw3
  ${SYD_STD_DB_LIBRARY})
