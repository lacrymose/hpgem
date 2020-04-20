
### REQUIRED DEPENDENCIES ###
#############################

FIND_PACKAGE(BLAS REQUIRED)
if(BLAS_FOUND AND NOT TARGET BLAS::BLAS)
    # CREATING A BLAS::BLAS TARGET as FINDBLAS does not create one
    add_library(BLAS::BLAS INTERFACE IMPORTED)
    set_target_properties(BLAS::BLAS PROPERTIES INTERFACE_LINK_LIBRARIES "${BLAS_LIBRARIES}" INTERFACE_LINK_FLAGS "${BLAS_LINKER_FLAGS}" )
endif()

FIND_PACKAGE(LAPACK REQUIRED)
if(LAPACK_FOUND AND NOT TARGET LAPACK::LAPACK)
    # CREATING A LAPACK::LAPACK TARGET as FINDLAPACK does not create one
    add_library(LAPACK::LAPACK INTERFACE IMPORTED)
    set_target_properties(BLAS::BLAS PROPERTIES INTERFACE_LINK_LIBRARIES  "${LAPACK_LIBRARIES}" INTERFACE_LINK_FLAGS "${LAPACK_LINKER_FLAGS}" )
endif()



### OPTIONAL DEPENDENCIES ###
#############################

if(hpGEM_USE_MPI)
    FIND_PACKAGE(MPI REQUIRED COMPONENTS CXX)
    add_definitions(-DHPGEM_USE_MPI)
endif()

if(hpGEM_USE_METIS)
    FIND_PACKAGE(METIS REQUIRED)
    add_definitions(-DHPGEM_USE_METIS)
    include_directories(${METIS_INCLUDE_DIR})
    # Create target for easy linking
    add_library(METIS::METIS INTERFACE IMPORTED)
    set_target_properties(METIS::METIS PROPERTIES 
    INTERFACE_LINK_LIBRARIES "${METIS_LIBRARIES}"
    INTERFACE_INCLUDE_DIRECTORIES "${METIS_INCLUDE_DIR}")
endif()

if(hpGEM_USE_QHULL)
    FIND_PACKAGE(QHULL REQUIRED)
    add_definitions(-DHPGEM_USE_QHULL)
    # Create target for easy linking
    add_library(QHULL::QHULL INTERFACE IMPORTED)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set_target_properties(QHULL::QHULL PROPERTIES 
        INTERFACE_LINK_LIBRARIES "${QHULL_DEBUG_LIBRARIES}" 
        INTERFACE_INCLUDE_DIRECTORIES "${QHULL_INCLUDE_DIR}")
    else()
        set_target_properties(QHULL::QHULL PROPERTIES 
        INTERFACE_LINK_LIBRARIES "${QHULL_LIBRARIES}" 
        INTERFACE_INCLUDE_DIRECTORIES "${QHULL_INCLUDE_DIR}")
    endif()
endif()


find_package(PkgConfig REQUIRED) 

if (hpGEM_USE_PETSC OR hpGEM_USE_COMPLEX_PETSC)
    set(hpGEM_LOGLEVEL "DEFAULT" CACHE STRING "Verbosity of hpGEM. Default is recommended.")

    if (hpGEM_USE_PETSC AND hpGEM_USE_COMPLEX_PETSC)
        message(FATAL_ERROR "The options 'hpGEM_USE_PETSC' and 'hpGEM_USE_COMPLEX_PETSC' are mutually exclusive, please disable one of them")
    endif()

   # find_package via pkg config
    pkg_check_modules(PETSC_PKG REQUIRED IMPORTED_TARGET PETSc)
    # Check whether we found a petsc with complex or real numbers
    include(CheckSymbolExists)

    foreach(petsc_include_dir ${PETSC_PKG_INCLUDE_DIRS})
        set(petsc_petscconf_path "${petsc_include_dir}/petscconf.h")
        if(EXISTS "${petsc_petscconf_path}")
            check_symbol_exists(PETSC_USE_COMPLEX ${petsc_petscconf_path} PETSC_IS_COMPLEX)
            check_symbol_exists(PETSC_ARCH ${petsc_petscconf_path} PETSC_ARCH)
        endif()
    endforeach()

    set(hpGEM_USE_ANY_PETSC ON)

    # Check if this matches with what the user expects
    if (PETSC_IS_COMPLEX)
        if (hpGEM_USE_PETSC)
            message(FATAL_ERROR "Requested PETSc with real numbers, found one with complex numbers")
        endif()
        # TODO: This should be moved
        add_definitions(-DHPGEM_USE_COMPLEX_PETSC -DHPGEM_USE_ANY_PETSC)

    else()
        if (hpGEM_USE_COMPLEX_PETSC)
            message(FATAL_ERROR "Requested PETSc with complex numbers, found one with real numbers")
        endif()
        add_definitions(-DHPGEM_USE_PETSC -DHPGEM_USE_ANY_PETSC)
    endif()
    # Create target for easy linking
    if(PETSC_PKG_FOUND)
        add_library(PETSc::PETSc INTERFACE IMPORTED)

        if(PETSC_IS_COMPLEX)
            set_target_properties(PETSc::PETSc PROPERTIES 
            INTERFACE_LINK_LIBRARIES "${PETSC_PKG_LIBRARIES}" 
            INTERFACE_INCLUDE_DIRECTORIES "${PETSC_PKG_INCLUDE_DIRS}"
            INTERFACE_LINK_FLAGS "${PETSC_PKG_LDFLAGS}"
            INTERFACE_COMPILE_DEFINITIONS "${HPGEM_USE_COMPLEX_PETSC};${HPGEM_USE_ANY_PETSC}")      
        else()
            set_target_properties(PETSc::PETSc PROPERTIES 
            INTERFACE_LINK_LIBRARIES "${PETSC_PKG_LIBRARIES}" 
            INTERFACE_INCLUDE_DIRECTORIES "${PETSC_PKG_INCLUDE_DIRS}"
            INTERFACE_LINK_FLAGS "${PETSC_PKG_LDFLAGS}"
            INTERFACE_COMPILE_DEFINITIONS "${HPGEM_USE_PETSC};${HPGEM_USE_ANY_PETSC}")
        endif()

    endif()

else()
    set(hpGEM_USE_ANY_PETSC OFF)
endif() # Petsc

if(hpGEM_USE_SLEPC)


    if(NOT hpGEM_USE_ANY_PETSC)
        message(FATAL_ERROR
                "SLEPc depends on PETSc, please also enable PETSc support")
    endif()
    pkg_check_modules(SLEPC_PKG REQUIRED IMPORTED_TARGET SLEPc)

    include(CheckSymbolExists)

    foreach(slepc_include_dir ${SLEPC_PKG_INCLUDE_DIRS})
        set(slepc_slepcconf_path "${slepc_include_dir}/slepcconf.h")
        if(EXISTS "${slepc_slepcconf_path}")
            check_symbol_exists(SLEPC_PETSC_ARCH ${petsc_petscconf_path} SLEPC_PETSC_ARCH)
        endif()
    endforeach()

    if(NOT(SLEPC_PETSC_ARCH STREQUAL PETSC_ARCH))
        message(FATAL_ERROR "SLEPc ARCH and PETsc architecture differ.")
    endif()


    add_definitions(-DHPGEM_USE_SLEPC)
    # Create target for easy linking
    if(SLEPC_PKG_FOUND)
        add_library(SLEPc::SLEPc INTERFACE IMPORTED)
        set_target_properties(SLEPc::SLEPc PROPERTIES
        INTERFACE_LINK_LIBRARIES "${SLEPC_PKG_LIBRARIES}" 
        INTERFACE_INCLUDE_DIRECTORIES "${SLEPC_PKG_INCLUDE_DIRS}"
        INTERFACE_LINK_FLAGS "${SLEPC_PKG_LDFLAGS}"
        )
    endif()
endif()

if(hpGEM_USE_SUNDIALS)
    if(hpGEM_USE_MPI)
        message(FATAL_ERROR "SUNDIALS is only serial (for now)")
    endif()
    FIND_PACKAGE(SUNDIALS REQUIRED)
    add_definitions(-DHPGEM_USE_SUNDIALS)
    # Create target for easy linking
    add_library(SUNDIALS::SUNDIALS INTERFACE IMPORTED)

    set_target_properties(SUNDIALS::SUNDIALS PROPERTIES 
        INTERFACE_LINK_LIBRARIES  "${SUNDIALS_LIB_sundials_cvodes};${SUNDIALS_LIB_sundials_idas};${SUNDIALS_LIB_sundials_kinsol};${SUNDIALS_LIB_sundials_nvecserial}"
        INTERFACE_INCLUDE_DIRECTORIES "${SUNDIALS_INCLUDE_DIR}")
endif()
