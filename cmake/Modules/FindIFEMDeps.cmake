INCLUDE(IFEMOptions)

# IFEM includes Fortran code
ENABLE_LANGUAGE(Fortran)
IF(CMAKE_Fortran_COMPILER MATCHES ifort)
  SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} -lifcore)
ENDIF(CMAKE_Fortran_COMPILER MATCHES ifort)

# And c++ code
ENABLE_LANGUAGE(CXX)
IF(CMAKE_CXX_COMPILER MATCHES icpc)
  SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DUSE_MKL -mkl=sequential")
  SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DUSE_MKL -mkl=sequential")
ELSE(CMAKE_CXX_COMPILER MATCHES icpc)
  SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DUSE_CBLAS")
  SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DUSE_CBLAS")
  FIND_PACKAGE(LAPACK REQUIRED)
ENDIF(CMAKE_CXX_COMPILER MATCHES icpc)

# Required dependences
FIND_PACKAGE(GoTools REQUIRED)
FIND_PACKAGE(GoTrivariate REQUIRED)
FIND_PACKAGE(ARPACK REQUIRED)

# Mimimum GoTools version
IF(GoTools_VERSION_MAJOR LESS 3 OR NOT GoTools_VERSION_MAJOR)
  MESSAGE(FATAL_ERROR "GoTools >= 3.0.0 required. bailing")
ENDIF(GoTools_VERSION_MAJOR LESS 3 OR NOT GoTools_VERSION_MAJOR)

SET(IFEM_DEPLIBS ${IFEM_DEPLIBS}
                 ${GoTrivariate_LIBRARIES}
                 ${GoTools_LIBRARIES}
                 ${ARPACK_LIBRARIES}
                 ${LAPACK_LIBRARIES})

SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES}
                     ${GoTools_INCLUDE_DIRS}
                     ${GoTrivariate_INCLUDE_DIRS})

SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} ${GoTools_CXX_FLAGS}")
SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} ${GoTools_CXX_FLAGS}")

# HDF5
IF(IFEM_USE_HDF5)
  FIND_PACKAGE(HDF5)
  IF(HDF5_LIBRARIES AND HDF5_INCLUDE_DIR)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${HDF5_LIBRARIES})
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${HDF5_INCLUDE_DIR})
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_HDF5=1")
  ENDIF(HDF5_LIBRARIES AND HDF5_INCLUDE_DIR)
ENDIF(IFEM_USE_HDF5)

# TinyXML
FIND_PACKAGE(TinyXML)
IF(TINYXML_INCLUDE_DIR AND TINYXML_LIBRARIES)
  SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${TINYXML_INCLUDE_DIR})
  SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${TINYXML_LIBRARIES})
  SET(IFEM_USE_SYSTEM_TINYXML 1)
ENDIF(TINYXML_INCLUDE_DIR AND TINYXML_LIBRARIES)

# SuperLU
IF(IFEM_USE_SUPERLU)
  FIND_PACKAGE(SuperLU)
  IF(SuperLU_MT_LIBRARIES AND SuperLU_MT_INCLUDES AND IFEM_USE_SUPERLU_MT)
    FIND_PACKAGE(Threads REQUIRED)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${SuperLU_MT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${SuperLU_MT_INCLUDES})
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_SUPERLU_MT")
  ELSEIF(SuperLU_LIBRARIES AND SuperLU_INCLUDES AND IFEM_USE_SUPERLU_MT)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${SuperLU_LIBRARIES})
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${SuperLU_INCLUDES})
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_SUPERLU")
  ENDIF(SuperLU_MT_LIBRARIES AND SuperLU_MT_INCLUDES AND IFEM_USE_SUPERLU_MT)
ENDIF(IFEM_USE_SUPERLU)

# LR splines
IF(IFEM_USE_LRSPLINES)
  FIND_PACKAGE(LRSpline)
  IF(LRSpline_LIBRARIES AND LRSpline_INCLUDE_DIRS)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${LRSpline_LIBRARIES})
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${LRSpline_INCLUDE_DIRS})
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_LRSPLINE=1")
  ENDIF(LRSpline_LIBRARIES AND LRSpline_INCLUDE_DIRS)
ENDIF(IFEM_USE_LRSPLINES)

# PETSc
IF(IFEM_USE_PETSC)
  FIND_PACKAGE(Petsc)
  IF(PETSC_LIBRARIES AND PETSC_INCLUDES)
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${PETSC_INCLUDES})
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${PETSC_LIBRARIES})
    SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DHAS_PETSC")
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_PETSC")
    IF(IFEM_USE_PARALLEL_PETSC)
      FIND_PACKAGE(MPI REQUIRED)
      SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${MPI_INCLUDE_PATH})
      SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${MPI_LIBRARIES})
      SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DPARALLEL_PETSC")
    ENDIF(IFEM_USE_PARALLEL_PETSC)
    IF(IFEM_ENABLE_SLEPC)
      FIND_PACKAGE(SLEPc)
      MESSAGE(STATUS ${SLEPC_LIBRARIES} ${SLEPC_INCLUDES})
      IF(SLEPC_LIBRARIES AND SLEPC_INCLUDES)
        SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${SLEPC_INCLUDES})
        SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${SLEPC_LIBRARIES})
        SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_SLEPC")
      ENDIF(SLEPC_LIBRARIES AND SLEPC_INCLUDES)
    ENDIF(IFEM_ENABLE_SLEPC)
  ENDIF(PETSC_LIBRARIES AND PETSC_INCLUDES)
ENDIF(IFEM_USE_PETSC)

# SPR
IF(IFEM_USE_SPR)
  IF(SPR_LIBRARIES)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${SPR_LIBRARIES})
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_SPR -DUSE_F77SAM")
  ENDIF(SPR_LIBRARIES)
ENDIF(IFEM_USE_SPR)

# SAMG
IF(SAMG_LIBRARIES AND SAMG_INCLUDES)
  SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${SAMG_LIBRARIES})
  SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${SAMG_INCLUDES})
  SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_SAMG -DSAMG_UNIX_LINUX=1 -DSAMG_LCASE_USCORE")
ENDIF(SAMG_LIBRARIES AND SAMG_INCLUDES)

# VTFWriter
IF(IFEM_USE_VTFWRITER)
  FIND_PACKAGE(VTFWriter)
  IF(VTFWRITER_LIBRARIES AND VTFWRITER_INCLUDES)
    SET(IFEM_DEPLIBS ${IFEM_DEPLIBS} ${VTFWRITER_LIBRARIES})
    SET(IFEM_DEPINCLUDES ${IFEM_DEPINCLUDES} ${VTFWRITER_INCLUDES})
    SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} -DHAS_VTFAPI=${VTFAPI}")
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} -DHAS_VTFAPI=${VTFAPI}")
  ENDIF(VTFWRITER_LIBRARIES AND VTFWRITER_INCLUDES)
ENDIF(IFEM_USE_VTFWRITER)

# OpenMP
IF(IFEM_USE_OPENMP)
  FIND_PACKAGE(OpenMP)
  IF(OPENMP_FOUND)
    SET(IFEM_CXX_FLAGS "${IFEM_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP")
    SET(IFEM_BUILD_CXX_FLAGS "${IFEM_BUILD_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -DUSE_OPENMP")
  ENDIF(OPENMP_FOUND)
ENDIF(IFEM_USE_OPENMP)
