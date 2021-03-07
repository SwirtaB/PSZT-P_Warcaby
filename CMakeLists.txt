cmake_minimum_required(VERSION 3.16)
project(PSZT-P_warcaby)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git) 
FetchContent_MakeAvailable(mahi-gui)

if (WIN32)
    find_package(Python 3.8 REQUIRED)
    find_package(PythonLibs 3.8 REQUIRED)
elseif (UNIX)
    find_package(Boost 1.71.0 REQUIRED COMPONENTS python38)
    find_package(Python 3.8 REQUIRED)
    find_package(PythonLibs 3.8 REQUIRED)
endif ()

set(Boost_USE_STATIC_LIBS ON)
set(CMAKE_SHARED_MODULE_PREFIX "")

file(GLOB TARGET_SRC "./src/*.cpp" )
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS)

set(EXECUTABLE_NAME "pszt_warcaby")
include_directories(${CMAKE_HOME_DIRECTORY}/include)
add_executable(${EXECUTABLE_NAME} ${TARGET_SRC})

if (WIN32)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} mahi::gui ${PYTHON_LIBRARIES})
elseif (UNIX)
    target_include_directories(${EXECUTABLE_NAME} PRIVATE ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} mahi::gui Boost::python38 ${PYTHON_LIBRARIES})
endif ()


#Add tests -> building program for testing with boost
set(BUILD_TESTS True) #Set to "True" for test for building tests

if(BUILD_TESTS)
    message("Building tests.")
    list(FILTER TARGET_SRC EXCLUDE REGEX /*/main.cpp)
    if (UNIX)
        find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    endif ()

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/build)

    add_executable(tests ${CMAKE_HOME_DIRECTORY}/tests/Tests.cpp ${TARGET_SRC} tests/Tests.cpp)

    if (WIN32)
        target_include_directories(tests PRIVATE ${PYTHON_INCLUDE_DIRS})
        target_link_libraries(tests mahi::gui ${PYTHON_LIBRARIES})
    elseif (UNIX)
        target_include_directories(tests PRIVATE ${PYTHON_INCLUDE_DIRS})
        target_link_libraries(tests mahi::gui Boost::python38 ${PYTHON_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    endif ()

    add_test(UnitTest tests)
    enable_testing()
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
