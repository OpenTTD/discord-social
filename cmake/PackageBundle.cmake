string(TIMESTAMP CURRENT_YEAR "%Y")

set(CPACK_BUNDLE_NAME "OpenTTD-${PROJECT_NAME}")
set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/os/macos/openttd.icns")
set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/os/macos/splash.png")
set(CPACK_DMG_FORMAT "UDBZ")
set(CPACK_DMG_DISABLE_APPLICATIONS_SYMLINK ON)
# This seems to be a bit buggy, as CPACK_APPLE_BUNDLE_ID is only referenced once in the source:
# https://github.com/Kitware/CMake/blob/a3f76a4e4dc15997f22306b002fbc452af1259a6/Source/CPack/cmCPackBundleGenerator.cxx#L206C35-L206C56
# But if we don't set it, the codesign step will be invalid. So yeah .. weird.
set(CPACK_APPLE_BUNDLE_ID "org.openttd.${PROJECT_NAME}")

# Create a temporary Info.plist.in, where we will fill in the version via
# CPackProperties.cmake.in. This because at this point in time the version
# is not yet known.
configure_file("${CMAKE_SOURCE_DIR}/os/macos/Info.plist.in" "${CMAKE_CURRENT_BINARY_DIR}/Info.plist.in")
set(CPACK_BUNDLE_PLIST_SOURCE "${CMAKE_CURRENT_BINARY_DIR}/Info.plist.in")

get_target_property(LIBRARIES ${PROJECT_NAME} LINK_LIBRARIES)
string(REGEX REPLACE "([^;]+)" "/Contents/Resources/\\1.dylib" LIBRARIES "${LIBRARIES}")
set(CPACK_BUNDLE_APPLE_CODESIGN_FILES "/Contents/Resources/lib${PROJECT_NAME}.dylib;${LIBRARIES}")
set(CPACK_BUNDLE_APPLE_CODESIGN_PARAMETER "--deep -f --options runtime")

# Copy "Install" script to the bundle root.
install(
    CODE
    "
        file(COPY \"${CMAKE_SOURCE_DIR}/os/macos/Install\" DESTINATION \${CMAKE_INSTALL_PREFIX}/../MacOS/)
    "
    DESTINATION .
    COMPONENT Runtime)
