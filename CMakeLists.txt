# cmake最低版本号要求
cmake_minimum_required(VERSION 2.8.12.2)

# 项目名
set(PROJECT_NAME "MyAgent")
project(${PROJECT_NAME})


set(OUTPUT_PATH ${CMAKE_BINARY_DIR})

set(DEPENDS_HEADER_PATH)
list(APPEND DEPENDS_HEADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/netsnmp/include)

set(DEPENDS_LIB_PATH)
list(APPEND DEPENDS_LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/netsnmp/lib)

# 添加头文件
macro(include_headers)
    foreach(HEADER_PATH ${DEPENDS_HEADER_PATH})
        include_directories(${HEADER_PATH})
    endforeach()
endmacro()

# 添加连接库
macro(lib_headers)
    foreach(LIB_PATH ${DEPENDS_LIB_PATH})
        link_directories(${LIB_PATH})
    endforeach()
endmacro()



macro(compile_target MODULE)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_PATH}/bin)
    include_headers()
    lib_headers()
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} DIR_SRCS)
    add_executable(${MODULE} ${DIR_SRCS})
    target_link_libraries(${MODULE} netsnmp netsnmpagent netsnmphelpers netsnmpmibs  netsnmptrapd)
endmacro()


compile_target(${PROJECT_NAME})