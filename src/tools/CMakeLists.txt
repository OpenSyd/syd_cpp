


#----------------------------------------------------------
wrap_ggo(sydCreateDatabase sydCreateDatabase.ggo)
add_executable(sydCreateDatabase sydCreateDatabase.cxx sydCreateDatabase_ggo.c)
target_link_libraries(sydCreateDatabase ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydDump sydDump.ggo)
add_executable(sydDump sydDump.cxx sydDump_ggo.c)
target_link_libraries(sydDump ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------


#----------------------------------------------------------
wrap_ggo(sydInsert sydInsert.ggo)
add_executable(sydInsert sydInsert.cxx sydInsert_ggo.c)
target_link_libraries(sydInsert ${SYD_CORE_LIBRARIES})
#----------------------------------------------------------
