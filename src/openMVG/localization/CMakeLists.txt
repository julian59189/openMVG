
set(localization_files_headers
  LocalizationResult.hpp
  VoctreeLocalizer.hpp
  optimization.hpp
  reconstructed_regions.hpp
  ILocalizer.hpp
  rigResection.hpp
)
set(localization_files_sources
  LocalizationResult.cpp
  VoctreeLocalizer.cpp
  optimization.cpp
  rigResection.cpp
)

if (OpenMVG_USE_CCTAG)
  list(APPEND localization_files_headers 
        CCTagLocalizer.hpp)
  list(APPEND localization_files_sources 
        CCTagLocalizer.cpp)
endif()

ADD_LIBRARY(openMVG_localization
  ${localization_files_headers}
  ${localization_files_sources})
TARGET_LINK_LIBRARIES(openMVG_localization 
      PUBLIC openMVG_sfm 
             openMVG_voctree 
             openMVG_numeric 
      PRIVATE openMVG_system 
              stlplus 
              ${Boost_LIBRARIES} 
              ${LOG_LIB})
IF(OpenMVG_USE_CCTAG)
  TARGET_LINK_LIBRARIES(openMVG_localization PRIVATE CCTag::CCTag)
ENDIF(OpenMVG_USE_CCTAG)
IF(OpenMVG_USE_OPENGV)
  TARGET_LINK_LIBRARIES(openMVG_localization PRIVATE ${OPENGV_LIBRARY})
ENDIF(OpenMVG_USE_OPENGV)

SET_TARGET_PROPERTIES(openMVG_localization PROPERTIES SOVERSION ${OPENMVG_VERSION_MAJOR} VERSION "${OPENMVG_VERSION_MAJOR}.${OPENMVG_VERSION_MINOR}")
SET_PROPERTY(TARGET openMVG_localization PROPERTY FOLDER OpenMVG)
INSTALL(TARGETS openMVG_localization DESTINATION lib EXPORT openMVG-targets)

UNIT_TEST(openMVG LocalizationResult
  "openMVG_localization")
IF(OpenMVG_USE_OPENGV)
  UNIT_TEST(openMVG rigResection
    "openMVG_localization")
ENDIF(OpenMVG_USE_OPENGV)
