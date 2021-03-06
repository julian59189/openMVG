IF(OpenMVG_USE_BOOST)
    ###
    # Test the localization class
    ###
    ADD_EXECUTABLE(openMVG_main_cameraLocalizer main_cameraLocalizer.cpp)
    INSTALL(TARGETS openMVG_main_cameraLocalizer DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_cameraLocalizer 
                    openMVG_localization 
                    openMVG_dataio  
                    openMVG_image 
                    openMVG_features 
                    vlsift 
                    ${BOOST_LIBRARIES})
    if(OpenMVG_USE_ALEMBIC)
        target_link_libraries(openMVG_main_cameraLocalizer openMVG_sfm ${ABC_LIBRARIES})
    endif()
    if(OpenMVG_USE_CCTAG)
        target_link_libraries(openMVG_main_cameraLocalizer CCTag::CCTag)
    endif()

    ###
    # Create the vocabulary tree
    ###
    ADD_EXECUTABLE(openMVG_main_createVoctree createVoctree.cpp)
    INSTALL(TARGETS openMVG_main_createVoctree DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_createVoctree openMVG_voctree openMVG_features ${Boost_LIBRARIES})

    ###
    # generate the pair lists
    ###
    ADD_EXECUTABLE(openMVG_main_generatePairList pairListFromVoctree.cpp)
    INSTALL(TARGETS openMVG_main_generatePairList DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_generatePairList openMVG_voctree ${Boost_LIBRARIES})

    ###
    # voctree query utility
    ###
    ADD_EXECUTABLE(openMVG_main_voctreeQueryUtility voctreeQueryUtility.cpp)
    INSTALL(TARGETS openMVG_main_voctreeQueryUtility DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_voctreeQueryUtility openMVG_voctree ${Boost_LIBRARIES})

    ###
    # Test the localization class for the rig
    ###
    ADD_EXECUTABLE(openMVG_main_rigLocalizer main_rigLocalizer.cpp)
    INSTALL(TARGETS openMVG_main_rigLocalizer DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_rigLocalizer 
                            openMVG_localization 
                            openMVG_dataio 
                            openMVG_rig 
                            openMVG_image 
                            openMVG_features 
                            vlsift 
                            ${Boost_LIBRARIES})
    if(OpenMVG_USE_ALEMBIC)
        target_link_libraries(openMVG_main_rigLocalizer openMVG_sfm ${ABC_LIBRARIES})
    endif()
    if(OpenMVG_USE_CCTAG)
        target_link_libraries(openMVG_main_rigLocalizer CCTag::CCTag)
    endif()

    ###
    # utility to convert float descriptors to char
    ###
    ADD_EXECUTABLE(openMVG_main_convertDescFloat2Uchar convertDescFloat2Uchar.cpp)
    INSTALL(TARGETS openMVG_main_convertDescFloat2Uchar DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_convertDescFloat2Uchar openMVG_localization openMVG_features ${Boost_LIBRARIES})

    # voctree statistics
    ###
    ADD_EXECUTABLE(openMVG_main_voctreeStatistics voctreeStatistics.cpp)
    INSTALL(TARGETS openMVG_main_voctreeStatistics DESTINATION bin/)
    TARGET_LINK_LIBRARIES(openMVG_main_voctreeStatistics openMVG_voctree ${Boost_LIBRARIES})

    #@TODO decide where to put the binaries

    # Installation rules
    #SET_PROPERTY(TARGET openMVG_main_createVoctree PROPERTY FOLDER OpenMVG/software)
    #INSTALL(TARGETS openMVG_main_createVoctree DESTINATION bin/)
    #SET_PROPERTY(TARGET openMVG_main_generatePairList PROPERTY FOLDER OpenMVG/software)
    #INSTALL(TARGETS openMVG_main_generatePairList DESTINATION bin/)

ENDIF(OpenMVG_USE_BOOST)



###
# Localize images in an existing SfM reconstruction
###
ADD_EXECUTABLE(openMVG_main_SfM_Localization main_SfM_Localization.cpp)
TARGET_LINK_LIBRARIES(openMVG_main_SfM_Localization
  openMVG_system
  openMVG_image
  openMVG_features
  openMVG_matching_image_collection
  openMVG_sfm
  easyexif
  vlsift
  )

# Installation rules
SET_PROPERTY(TARGET openMVG_main_SfM_Localization PROPERTY FOLDER OpenMVG/software)
INSTALL(TARGETS openMVG_main_SfM_Localization DESTINATION bin/)
