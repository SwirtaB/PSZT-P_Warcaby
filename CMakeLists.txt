cmake_minimum_required(VERSION 3.16)
project(PSZT-P_warcaby)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git) 
FetchContent_MakeAvailable(mahi-gui)

file(GLOB TARGET_SRC "./src/*.cpp" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(EXECUTABLE_NAME "pszt_warcaby")
include_directories(${CMAKE_HOME_DIRECTORY}/include)
add_executable(${EXECUTABLE_NAME} ${TARGET_SRC})

target_link_libraries(${EXECUTABLE_NAME} mahi::gui)

if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /W4)
else()
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -pedantic -Ofast)
endif()

set (BUILD_DOCS True) #set TRUE if you want to build docs

if(BUILD_DOCS)
    find_package(Doxygen)
    option(BUILD_DOCUMENTATION "Create and install the HTML based API
    documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    message("Doxygen build started.")

    add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile_in}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
endif()
