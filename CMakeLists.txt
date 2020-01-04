cmake_minimum_required(VERSION 3.15)
project(simplecborrpc C)

set(CMAKE_C_STANDARD 99)

include_directories(. tests/cmocka/include tinycbor/src)

if ("${CMAKE_C_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang" OR
        "${CMAKE_CXX_COMPILER_ID}" MATCHES "(Apple)?[Cc]lang")
    string(APPEND CMAKE_C_FLAGS " -fprofile-instr-generate -fcoverage-mapping")
    string(APPEND CMAKE_CXX_FLAGS " -fprofile-instr-generate -fcoverage-mapping")
elseif(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    string(APPEND CMAKE_C_FLAGS " --coverage")
    string(APPEND CMAKE_CXX_FLAGS " --coverage")
endif()

set(TINYCBOR_FILES   tinycbor/src/cborencoder.c
                    tinycbor/src/cborerrorstrings.c
                    tinycbor/src/cborparser.c
                    tinycbor/src/cborvalidation.c)

add_executable(simplecborrpc ${TINYCBOR_FILES} simplecborrpc.c tests/main.c tests/cmocka/src/cmocka.c)