#First declare the rules that will form the executable tests#
#############################################################

if(hpGEM_USE_QHULL)

    include_directories(${QHULL_INCLUDES})


add_executable(rotor.out
		app1.cpp
      		)
target_link_libraries(rotor.out HPGEM::HPGEM)

endif()
