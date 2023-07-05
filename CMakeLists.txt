cmake_minimum_required(VERSION 3.20.0)
project (
    shinobi
    DESCRIPTION "Simple, fun, memory safe language."
    VERSION 0.0.1
    LANGUAGES CXX
)

set(shinobi_sources
    src/ast.cc
    src/lexer.cc
    src/parser.cc
)

include(CMakeDependentOption)
cmake_dependent_option(SHINOBI_BUILD_TESTS
  "Build the unit tests when shinobi is the root project" ON
  "CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR" OFF)

# Configure LLVM.
# The following command is useful for determining configuration:
#   `llvm-config --cxxflags --ldflags --system-libs --libs core`
enable_language(C) # See https://github.com/llvm/llvm-project/issues/53950.
set (CMAKE_CXX_COMPILER "clang++")
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set (CMAKE_EXE_LINKER_FLAGS "-Wl,-headerpad_max_install_names")

if(APPLE)
  set (llvm_libs "LLVM")
else()
  set (llvm_libs "LLVMCore")
endif()

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

# ASAN Support
option(ASAN "Enable code asan" OFF)
if(ASAN)
    set (CMAKE_CXX_FLAGS_DEBUG
         "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS_DEBUG
         "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

# Executable target.
add_executable(shinobi driver.cc ${shinobi_sources})
target_include_directories(shinobi PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(shinobi ${llvm_libs})

# Static library target.
add_library(shinobi_a STATIC ${shinobi_sources})
target_include_directories(shinobi_a PRIVATE ${PROJECT_SOURCE_DIR}/include)
# TODO(cjihrig): Implement code coverage.
#if(CODE_COVERAGE)
#    target_link_libraries(shinobi_a PUBLIC ${llvm_libs} coverage_config)
#else()
    target_link_libraries(shinobi_a PRIVATE ${llvm_libs})
#endif()

# Test targets.
if(SHINOBI_BUILD_TESTS)
    enable_testing()
    file(GLOB test_files "test/test-*.cc")
    foreach(file ${test_files})
        get_filename_component(test_name ${file} NAME_WE)
        add_executable(${test_name} ${file})
        # Create the test under Testing/, which is already git ignored.
        set_target_properties(${test_name}
                              PROPERTIES
                              RUNTIME_OUTPUT_DIRECTORY
                              "${CMAKE_BINARY_DIR}/Testing")
        add_test(NAME ${test_name}
                 COMMAND ${test_name}
                 # Set the working directory so that paths to fixtures work.
                 WORKING_DIRECTORY "test")
        target_include_directories(${test_name}
                                    PRIVATE
                                    ${PROJECT_SOURCE_DIR}/include)
        target_link_libraries(${test_name} PRIVATE ${llvm_libs} shinobi_a)
        list(APPEND test_list ${test_name})
    endforeach()

    add_custom_target(check
        COMMAND ctest -VV -C Debug -R test-
        DEPENDS ${test_list}
    )
endif()

message(STATUS "Shinobi Build Options:

    Build tests:            ${SHINOBI_BUILD_TESTS}
    ASAN:                   ${ASAN}
    Install prefix:         ${CMAKE_INSTALL_PREFIX}
    Target system:          ${CMAKE_SYSTEM_NAME}
    Target architecture:    ${CMAKE_HOST_SYSTEM_PROCESSOR}
    CMake module path:      ${CMAKE_MODULE_PATH}
    Compiler:
      C++ compiler:         ${CMAKE_CXX_COMPILER}
      C++ language version: ${CMAKE_CXX_STANDARD}
      C++ flags:            ${CMAKE_CXX_FLAGS}
    Linker:
      Flags:                ${CMAKE_EXE_LINKER_FLAGS}
    LLVM:
      Version:              ${LLVM_PACKAGE_VERSION}
      LLVMConfig.cmake in:  ${LLVM_DIR}
      Include dirs:         ${LLVM_INCLUDE_DIRS}
      Library dirs:         ${LLVM_LIBRARY_DIRS}
      Link libraries:       ${llvm_libs}
      Definitions:          ${LLVM_DEFINITIONS_LIST}
")
