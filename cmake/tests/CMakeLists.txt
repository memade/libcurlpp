project(bridge)

# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")

set(SOURCES_DIR ${SOLUTION_DIR}/src/system/bridge/)

set(SOURCES
${SOURCES_DIR}Config.h
${SOURCES_DIR}Config.cpp
${SOURCES_DIR}BridgeOrder.cpp
${SOURCES_DIR}ComponentObj.cpp
${SOURCES_DIR}Core.cpp
${SOURCES_DIR}Global.cpp
${SOURCES_DIR}ServerMessage.cpp
${SOURCES_DIR}SymbolCon.cpp
${SOURCES_DIR}Server.cpp
${SOURCES_DIR}ServiceObj.cpp
${SOURCES_DIR}Setup.cpp
${SOURCES_DIR}stdafx.cpp
${SOURCES_DIR}Timer.cpp
${SOURCES_DIR}main.cpp
${SOURCES_DIR}BridgeOrder.h
${SOURCES_DIR}ComponentObj.h
${SOURCES_DIR}Core.h
${SOURCES_DIR}Global.h
${SOURCES_DIR}resource.h
${SOURCES_DIR}SymbolCon.h
${SOURCES_DIR}Server.h
${SOURCES_DIR}ServiceObj.h
${SOURCES_DIR}Setup.h
${SOURCES_DIR}Shared.h
${SOURCES_DIR}stdafx.h
${SOURCES_DIR}Timer.h
${SOURCES_DIR}LiquidityProviderObj.h
${SOURCES_DIR}LiquidityProviderObj.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})
# 设置编译选项
target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/GL>")
# 设置链接选项
target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
# 指定专用库头文件目录
target_include_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/3rdparty/spdlog/include/)
# 依赖库目录
target_link_directories(${PROJECT_NAME} 
PRIVATE 
${SOLUTION_ROOT_DIR}/librarys/$(Platform)/
${SOLUTION_ROOT_DIR}/projects/MT5Projects/lib/$(Platform)/
)

#/$(Platform)/$(Configuration)
# 设置生成目录
set_target_properties(${PROJECT_NAME}
 PROPERTIES
 LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_LIB_DEBUG}
 LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_LIB_RELEASE}
 RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PUBLISH_BIN_DEBUG}
 RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PUBLISH_BIN_RELEASE}
)

# 设置编译中间目录


# MSVC 预编译头设置
if (MSVC)
set(MSVCCMAKE_PCH ${SOLUTION_DIR}/cmakes/)
set(STDAFX_PCH_H ${SOURCES_DIR}stdafx.h)
set(STDAFX_PCH_C ${SOURCES_DIR}stdafx.cpp)	
LIST(APPEND CMAKE_MODULE_PATH ${MSVCCMAKE_PCH})
INCLUDE(msvcpch)
USE_MSVC_PCH(${PROJECT_NAME} stdafx.h ${STDAFX_PCH_C})
endif()





