cmake_minimum_required(VERSION 3.15)
project(simplecborrpc C)

set(CMAKE_C_STANDARD 99)

include_directories(. tests/cmocka/include tinycbor/src)

set(TINYCBOR_FILES   tinycbor/src/cborencoder.c
                    tinycbor/src/cborerrorstrings.c
                    tinycbor/src/cborparser.c
                    tinycbor/src/cborvalidation.c)

add_executable(simplecborrpc ${TINYCBOR_FILES} simplecborrpc.c tests/main.c tests/cmocka/src/cmocka.c)