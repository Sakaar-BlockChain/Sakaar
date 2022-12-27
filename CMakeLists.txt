cmake_minimum_required(VERSION 3.22)
project(Sakaar C)

set(CMAKE_C_STANDARD 99)

if(NOT TARGET Sakaar_Struct_lib)
    add_subdirectory(lib/Sakaar-Struct)
    link_directories(lib/Sakaar-Struct)
    MESSAGE(STATUS "Connected: Sakaar_Struct_lib")
endif()

if(NOT TARGET Sakaar_CryptoBase_lib)
    add_subdirectory(lib/Sakaar-CryptoBase)
    link_directories(lib/Sakaar-CryptoBase)
    MESSAGE(STATUS "Connected: Sakaar_CryptoBase_lib")
endif()

if(NOT TARGET Sakaar_Network_lib)
    add_subdirectory(lib/Sakaar-NetWork)
    link_directories(lib/Sakaar-NetWork)
    MESSAGE(STATUS "Connected: Sakaar_Network_lib")
endif()

FILE(GLOB SOURCE_FILES_DIR_DIR "src/*/*/*.c")
FILE(GLOB SOURCE_FILES_DIR "src/*/*.c")
FILE(GLOB SOURCE_FILES "src/*.c")
add_library(${PROJECT_NAME}_lib ${SOURCE_FILES} ${SOURCE_FILES_DIR} ${SOURCE_FILES_DIR_DIR})
target_include_directories(${PROJECT_NAME}_lib PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)


target_link_libraries(${PROJECT_NAME}_lib PUBLIC Sakaar_Memory_lib)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Sakaar_Struct_lib)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Sakaar_CryptoBase_lib)
target_link_libraries(${PROJECT_NAME}_lib PUBLIC Sakaar_Network_lib)

add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_NAME}_lib)