cmake_minimum_required(VERSION 2.6)

add_library(perfclient SHARED perfclient.c libperfcount/perfcount.c)
include_directories("libperfcount")
find_package(DynamoRIO)

if (NOT DynamoRIO_FOUND)
   message(FATAL_ERROR "DynamoRIO package required to build")
endif(NOT DynamoRIO_FOUND)

configure_DynamoRIO_client(perfclient)
