#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mynteye" for configuration "Release"
set_property(TARGET mynteye APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mynteye PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${MYNTEYES_SDK_ROOT}/lib/libmynteye.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS mynteye )
list(APPEND _IMPORT_CHECK_FILES_FOR_mynteye "${MYNTEYES_SDK_ROOT}/lib/libmynteye.a" )

# Import target "camera_models" for configuration "Release"
set_property(TARGET camera_models APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(camera_models PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
  IMPORTED_LOCATION_RELEASE "${MYNTEYES_SDK_ROOT}/lib/libcamera_models.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS camera_models )
list(APPEND _IMPORT_CHECK_FILES_FOR_camera_models "${MYNTEYES_SDK_ROOT}/lib/libcamera_models.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
