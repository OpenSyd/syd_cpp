

#----------------------------------------------------------
add_library(sydPlot SHARED
  sydPyPlotBuilder.cxx
  )

## to declare dependencies
target_link_libraries(sydPlot sydCore sydStandardDatabase ${PYTHON_LIBRARY})

#----------------------------------------------------------
# Not really a tool, for debug only
wrap_ggo(sydPlotTest_GGO_C sydPlotTest.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotTest sydPlotTest.cxx ${sydPlotTest_GGO_C})
target_link_libraries(sydPlotTest sydPlot StandardDatabase ${PYTHON_LIBRARY})


# Plot Timepoints
wrap_ggo(sydPlotTimepoints_GGO_C sydPlotTimepoints.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotTimepoints sydPlotTimepoints.cxx ${sydPlotTimepoints_GGO_C})
target_link_libraries(sydPlotTimepoints sydPlot ${PYTHON_LIBRARY} ${SYD_STD_DB_LIBRARY})

# Plot FitResult
wrap_ggo(sydPlotFitResult_GGO_C sydPlotFitResult.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotFitResult sydPlotFitResult.cxx ${sydPlotFitResult_GGO_C})
target_link_libraries(sydPlotFitResult StandardDatabase sydPlot ${PYTHON_LIBRARY})


# Plot FitImages
wrap_ggo(sydPlotFitImages_GGO_C sydPlotFitImages.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotFitImages sydPlotFitImages.cxx ${sydPlotFitImages_GGO_C})
target_link_libraries(sydPlotFitImages sydPlot ${PYTHON_LIBRARY} ${SYD_STD_DB_LIBRARY})
