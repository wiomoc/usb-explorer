cmake_minimum_required(VERSION 3.10)

project(USB VERSION 0.1)

configure_file(config.h.in "${PROJECT_BINARY_DIR}/config.h")

find_library(LIB_USB NAMES "usb-1.0"  PATH_SUFFIXES "lib" "lib32" "lib64")
find_path(LIB_USB_INCLUDE NAMES "libusb.h" PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_package(wxWidgets REQUIRED COMPONENTS core base)

set(CMAKE_BUILD_TYPE Debug)
set(SOURCES
    src/main.cpp
    src/App.cpp
    src/views/MainFrame.cpp
    src/usb/USBHotPlugController.cpp
)

add_executable(usb ${SOURCES})
set_property(TARGET usb PROPERTY CXX_STANDARD 11)
## XXX
SET(CMAKE_CXX_FLAGS "-lusb-1.0 -L/usr/local/lib/ ")
set_property(TARGET usb PROPERTY C_STANDARD 99)
target_include_directories(usb PUBLIC "${PROJECT_BINARY_DIR}" "${LIB_USB_INCLUDE}")
target_link_libraries(usb "${LIB_USB}" "${wxWidgets_LIBRARIES}")
include(${wxWidgets_USE_FILE})
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR})

install(TARGETS usb
BUNDLE DESTINATION . COMPONENT Runtime
RUNTIME DESTINATION bin COMPONENT Runtime
)
# Note Mac specific extension .app
set(APPS "${CMAKE_INSTALL_PREFIX}/usb.app")

# Directories to look for dependencies
set(DIRS "${CMAKE_BINARY_DIR}")
install(CODE "include(BundleUtilities)
    fixup_bundle(\"${APPS}\" \"\" \"${DIRS}\" \"IGNORE_ITEM\" \"${LIB_USB};${wxWidgets_LIBRARIES}\")" COMPONENT Runtime)

#set(CPACK_GENERATOR "DRAGNDROP")
include(CPack)