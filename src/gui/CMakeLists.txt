

find_package(OpenGL REQUIRED)
# find_package(glfw3 REQUIRED CONFIG)

# # if (APPLE)
# #   SET
# # else(APPLE)
pkg_check_modules(GLFW REQUIRED glfw3)
#find_package(GLFW)
DD(GLFW_STATIC_LIBRARIES)
#ndif(APPLE)

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
  sydgui.cxx
  sydguiFitOptionsWidget.cxx
  sydguiRoisSelectionWidget.cxx
  sydguiImagesSelectionWidget.cxx
  sydguiImagesListWidget.cxx
  sydguiImageWidget.cxx
  sydguiWidgets.cxx
  sydguiListFieldWidget.cxx
  )

# -----------------------------------------------------------------------------
## reference example
DD(OPENGL_LIBRARIES)
include_directories(libs/gl3w)
add_executable(imgui_opengl3_example
  main.cpp
  )
target_link_libraries(imgui_opengl3_example
  imgui
  ${OPENGL_LIBRARIES}
  ${GLFW_STATIC_LIBRARIES}
  ${SYD_STD_DB_LIBRARY})



# -----------------------------------------------------------------------------
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
  ${GLFW_STATIC_LIBRARIES}
  ${SYD_STD_DB_LIBRARY})

# -----------------------------------------------------------------------------
# gui for AUC by rois single patient, single method at a time
wrap_ggo(syd_gui_auc_by_roi_GGO_C
  syd_gui_auc_by_roi.ggo
  ../common_db/sydCommandLineDefault.ggo)
add_executable(syd_gui_auc_by_roi
  syd_gui_auc_by_roi.cxx
  ${syd_gui_auc_by_roi_GGO_C})

target_link_libraries(syd_gui_auc_by_roi
  sydgui
  ${OPENGL_LIBRARIES}
  ${GLFW_STATIC_LIBRARIES}
  ${SYD_STD_DB_LIBRARY})


# -----------------------------------------------------------------------------
# gui for AUC by rois single patient, single method at a time
wrap_ggo(syd_gui_images_GGO_C
  syd_gui_images.ggo
  ../common_db/sydCommandLineDefault.ggo)
add_executable(syd_gui_images
  syd_gui_images.cxx
  ${syd_gui_images_GGO_C})

target_link_libraries(syd_gui_images
  sydgui
  ${OPENGL_LIBRARIES}
  ${GLFW_STATIC_LIBRARIES}
  ${SYD_STD_DB_LIBRARY})
