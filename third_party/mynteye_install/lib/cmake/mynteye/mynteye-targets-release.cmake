#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mynteye" for configuration "Release"
set_property(TARGET mynteye APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(mynteye PROPERTIES
  IMPORTED_LOCATION_RELEASE "${MYNTEYES_SDK_ROOT}/lib/libmynteye.so.2.5.0"
  IMPORTED_SONAME_RELEASE "libmynteye.so.2"
  )

list(APPEND _IMPORT_CHECK_TARGETS mynteye )
list(APPEND _IMPORT_CHECK_FILES_FOR_mynteye "${MYNTEYES_SDK_ROOT}/lib/libmynteye.so.2.5.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
