

#----------------------------------------------------------
add_library(sydPlot SHARED
  sydPyPlotBuilder.cxx
  )

#----------------------------------------------------------
# Not really a tool, for debug only
wrap_ggo(sydPlotTest_GGO_C sydPlotTest.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotTest sydPlotTest.cxx ${sydPlotTest_GGO_C})
target_link_libraries(sydPlotTest sydStandardDatabase ${PYTHON_LIBRARY})


# Plot Timepoints
wrap_ggo(sydPlotTimepoints_GGO_C sydPlotTimepoints.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotTimepoints sydPlotTimepoints.cxx ${sydPlotTimepoints_GGO_C})
target_link_libraries(sydPlotTimepoints sydStandardDatabase sydPlot ${PYTHON_LIBRARY})
