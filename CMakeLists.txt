cmake_minimum_required(VERSION 3.25.1)

project(libcurlpp)

set(CMAKE_CXX_STANDARD 23)
# 添加预定义宏
add_definitions(-DUNICODE -D_UNICODE)

set(PLATFORM "x86")
if (CMAKE_CL_64)
set(PLATFORM x64)
else(CMAKE_CL_64)
set(PLATFORM x86)
endif(CMAKE_CL_64)

# 设置默认编译类型
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;" CACHE STRING "info" FORCE)

set(SOLUTION_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../)

set(CURRENT_SOLUTION_DIR ${CMAKE_CURRENT_SOURCE_DIR})
#cmake modules path
set(GLOBAL_CMAKE_MODULES_PATH ${SOLUTION_ROOT_DIR}/cmake/modules/)

LIST(APPEND CMAKE_MODULE_PATH ${GLOBAL_CMAKE_MODULES_PATH})
include(msvc_methods)
include(OutputPathConfig)

MEMADE_SET_CMAKETARGETS_FOLDER()

OutputPathConfig("bin" "bin" "lib")
add_subdirectory(${CURRENT_SOLUTION_DIR}/cmake/core/)
#set_target_properties(win PROPERTIES FOLDER "shared")

add_subdirectory(${CURRENT_SOLUTION_DIR}/cmake/tests/libcurlpp_test_core/)
set_target_properties(libcurlpp_test_core PROPERTIES FOLDER "tests")
OutputPathConfig("")


#openssl 3 dir or 
#--- "${LOCAL_COMMON_INCLUDE_PATH}/openssl/1.0.2u/"
#--- "${LOCAL_COMMON_INCLUDE_PATH}/openssl/3.0.5/"
#--- "${LOCAL_COMMON_INCLUDE_PATH}/openssl/3.1.0-beta1/"
#set(OPENSSL_INCLUDE_DIR ${LOCAL_COMMON_INCLUDE_PATH}/openssl/3.1.0-beta1/)
#set(OPENSSL_LIBRARY_DIR ${LOCAL_LIBRARYS_PATH}/openssl/3.1.0-beta1/)

#Project generic header definition
#include_directories(${LOCAL_SRC_PATH})
#include_directories(${LOCAL_COMMON_INCLUDE_PATH})
#include_directories(${LCOAL_PROJECTS_3RDPARTY_PATH})


#[[
OutputPathConfig("bin" "bin" "librarys/shared/")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/win)
set_target_properties(win PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/zip++)
set_target_properties(zip++ PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/encryption)
set_target_properties(encryption PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/iconv)
set_target_properties(iconv PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/ntobj)
set_target_properties(ntobj PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/openssl)
set_target_properties(openssl PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/reflective)
set_target_properties(reflective PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "shared")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/shared/wxskin)
set_target_properties(wxskin PROPERTIES FOLDER "shared")
OutputPathConfig("")


OutputPathConfig("tools/bin" "tools/bin" "librarys" ON)
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/compress)
set_target_properties(compress PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/cryption)
set_target_properties(cryption PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/encode)
set_target_properties(encode PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/packet)
set_target_properties(packet PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/respak)
set_target_properties(respak PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/update)
set_target_properties(update PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/fileedit)
set_target_properties(fileedit PROPERTIES FOLDER "tools")
add_subdirectory(${SOLUTION_ROOT_DIR}/compile/cmakes/tools/cppshellcode)
set_target_properties(cppshellcode PROPERTIES FOLDER "tools")
OutputPathConfig("")

add_subdirectory(${SOLUTION_ROOT_DIR}/projects)
]]
#set_target_properties(respak PROPERTIES FOLDER "projects")


#message(STATUS ${LOCAL_SRC_PATH})
#message(STATUS ${LOCAL_COMMON_INCLUDE_PATH})
#message(STATUS ${LCOAL_PROJECTS_3RDPARTY_PATH})
#message(STATUS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE})