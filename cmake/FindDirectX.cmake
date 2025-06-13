# FindDirectX.cmake
# Finds the DirectX library using Windows SDK

# This will define the following variables
#
#   DirectX_FOUND        - True if the system has DirectX
#   DirectX_INCLUDE_DIRS - DirectX include directory
#   DirectX_LIBRARIES    - DirectX libraries

include(FindPackageHandleStandardArgs)

# Find DirectX in Windows SDK
if(WIN32)
    # Windows SDK include directory
    get_filename_component(WINDOWS_SDK_ROOT "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" ABSOLUTE CACHE)
    
    if(NOT WINDOWS_SDK_ROOT)
        # Try to find Windows SDK in standard locations
        set(WINDOWS_SDK_ROOT "C:/Program Files (x86)/Windows Kits/10")
    endif()

    # Find latest Windows SDK version
    file(GLOB WINDOWS_SDK_VERSIONS LIST_DIRECTORIES true "${WINDOWS_SDK_ROOT}/Include/*")
    list(SORT WINDOWS_SDK_VERSIONS ORDER DESCENDING)
    list(GET WINDOWS_SDK_VERSIONS 0 LATEST_SDK_VERSION)
    get_filename_component(LATEST_SDK_VERSION ${LATEST_SDK_VERSION} NAME)

    # Set include directory
    set(DirectX_INCLUDE_DIR "${WINDOWS_SDK_ROOT}/Include/${LATEST_SDK_VERSION}/um")

    # Set library directory
    set(DirectX_LIB_DIR "${WINDOWS_SDK_ROOT}/Lib/${LATEST_SDK_VERSION}/um/x64")

    # Find DirectX libraries
    set(DirectX_LIBRARIES
        "${DirectX_LIB_DIR}/xbox360.lib"
        "${DirectX_LIB_DIR}/xinput.lib"
    )

    set(DirectX_FOUND TRUE)
    set(DirectX_INCLUDE_DIRS ${DirectX_INCLUDE_DIR})
else()
    set(DirectX_FOUND FALSE)
endif()

find_package_handle_standard_args(DirectX
    REQUIRED_VARS DirectX_INCLUDE_DIR DirectX_LIBRARIES
)

mark_as_advanced(
    DirectX_INCLUDE_DIR
    DirectX_LIB_DIR
    DirectX_LIBRARIES
)
