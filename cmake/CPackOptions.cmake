# Per-generator CPack settings, read by cpack at packaging time.
# The portable archive unpacks to WeirdMath-<ver>-Linux/bin/WeirdMath
# instead of .../opt/WeirdMath/bin/WeirdMath.
if(CPACK_GENERATOR STREQUAL "TGZ")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/")
endif()
