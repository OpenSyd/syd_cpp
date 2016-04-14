

#----------------------------------------------------------


#----------------------------------------------------------
# Not really a tool, for debug only
wrap_ggo(sydPlotTest_GGO_C sydPlotTest.ggo ../std_db/sydCommandLineDefault.ggo)
add_executable(sydPlotTest sydPlotTest.cxx ${sydPlotTest_GGO_C})
target_link_libraries(sydPlotTest sydStandardDatabase ${PYTHON_LIBRARY})
