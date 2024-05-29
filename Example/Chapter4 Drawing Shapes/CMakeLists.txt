cmake_minimum_required(VERSION 3.18)

project("DX12Project")
set(TARGET_NAME "DX12Project")

#设置链接器子系统为窗口
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
# 设置HLSL文件的存放位置
set(HLSL_DIR "HLSL")

# 设置资源文件的存放位置
set(ASSET_DIR "asset")

# 使用ImGui时设置为ON
# 需要将项目中的ImGui文件夹复制进来，保留*.h/*.cpp/CMakeLists.txt
set(USE_IMGUI ON)

# 使用Assimp时设置为ON
# 需要将解压的assimp文件夹内CMakeList.txt所在的全部文件复制到当前项目的Assimp文件夹内
set(USE_ASSIMP OFF)

# 设置C++标准为C++17
set(CMAKE_CXX_STANDARD 17)
# 设定项目编译为UNICODE标准
add_compile_definitions(UNICODE _UNICODE)

# 设置MSVC编译格式为utf-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# 将HLSL文件夹中的内容添加到build过程
file(COPY ${HLSL_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 将asset文件夹中的内容添加到build过程
file(COPY ${ASSET_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# 添加所有.hlsl文件和.h文件到指定文件夹中
file(GLOB HLSL_FILES ${HLSL_DIR}/*.hlsl)

file(COPY ${HLSL_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#设置HLSL文件属性
foreach(HLSL_FILE ${HLSL_FILES})

    get_filename_component(HLSL_FDIR ${HLSL_FILE} DIRECTORY)
    get_filename_component(HLSL_FBASENAME_WE ${HLSL_FILE} NAME_WE)
    string(CONCAT HLSL_FNAME_WE ${HLSL_FDIR} / ${HLSL_FBASENAME_WE})

    string(LENGTH ${HLSL_FBASENAME_WE} LEN_FNAME_WE)
    
    math(EXPR LEN_FNAME_WE "${LEN_FNAME_WE}-2")
    string(SUBSTRING ${HLSL_FBASENAME_WE} ${LEN_FNAME_WE} 2 ENTRY_POINT)
    string(TOLOWER ${ENTRY_POINT} SHADER_TYPE)

    if ("${SHADER_TYPE}" STREQUAL "vs") 
        set(SHADER_TYPE "Vertex")
    elseif("${SHADER_TYPE}" STREQUAL "hs") 
        set(SHADER_TYPE "Hull")
    elseif("${SHADER_TYPE}" STREQUAL "ds") 
        set(SHADER_TYPE "Domain")
    elseif("${SHADER_TYPE}" STREQUAL "gs") 
        set(SHADER_TYPE "Geometry")
    elseif("${SHADER_TYPE}" STREQUAL "ps")
        set(SHADER_TYPE "Pixel")
    elseif("${SHADER_TYPE}" STREQUAL "cs")
        set(SHADER_TYPE "Compute")
    endif()
    set_source_files_properties(${HLSL_FILE} PROPERTIES
                                VS_SHADER_OBJECT_FILE_NAME HLSL/${HLSL_FBASENAME_WE}.cso
                                VS_SHADER_TYPE ${SHADER_TYPE}
                                VS_SHADER_MODEL 5.1
                                VS_SHADER_ENTRYPOINT ${ENTRY_POINT}
                                VS_SHADER_DISABLE_OPTIMIZATIONS $<$<CONFIG:Debug>:true>
                                VS_SHADER_ENABLE_DEBUG $<$<CONFIG:Debug>:true>)
endforeach()



# 为项目附加包含目录，使其文件夹中的文件可被整个项目访问
include_directories(DX12API)
include_directories(DX12API/TextureLoader)
include_directories(DX12API/MathHelper)

# 设置文件目录

 set (HLSL_Files
 HLSL/07_basic.hlsli
  HLSL/07_color_PS.hlsl
  HLSL/07_color_VS.hlsl
 )
 #游戏进程相关
set (SOURCES
#入口
Game/DX12Project.cpp
#游戏进程
Game/GameApp.cpp
Game/GameApp.h
 )

 #DX12API
 set (DX12API
 #编译设置
DX12API/WinAPISetting.h
#DX12相关API
DX12API/DX12App.cpp
DX12API/DX12App.h
DX12API/UploadBuffer.h
 )

#程序时间
 set(GameTimer
 DX12API/DXGameTimer.cpp
 DX12API/DXGameTimer.h
 )

#DX12工具
 set(UtilityAPI
 DX12API/d3dUtil.cpp
 DX12API/d3dUtil.h
 )
 #辅助数学工具
 set(MathHelper
 DX12API/MathHelper/MathHelper.cpp
 DX12API/MathHelper/MathHelper.h
 )
 #基础几何体相关
 set(BasicGeometry
 DX12API/GeometryGenerator.cpp
 DX12API/GeometryGenerator.h
 )
 
 #帧资源处理
 set(FrameResource
 DX12API/FrameResource.cpp
 DX12API/FrameResource.h
 )
 #贴图读取
 set (TextureLoader
 DX12API/TextureLoader/DDSTextureLoader.cpp
DX12API/TextureLoader/DDSTextureLoader.h
DX12API/TextureLoader/WICTextureLoader.cpp
DX12API/TextureLoader/WICTextureLoader.h
 )
 #HLSL文件分组
 source_group(HLSL FILES ${HLSL_Files})
 #Game文件分组
 source_group(Game FILES ${SOURCES})
 #API文件分组

 #新增文件时需在此处添加分组
 source_group(DX12API FILES ${DX12API})
 source_group(DX12API\\TextureLoader FILES ${TextureLoader})
 source_group(DX12API\\MathHelper FILES ${MathHelper})
 source_group(DX12API\\BasicGeometry FILES ${BasicGeometry})
 source_group(DX12API\\FrameResource FILES ${FrameResource})
 source_group(DX12API\\GameTimer FILES ${GameTimer})
 source_group(DX12API\\UtilityAPI FILES ${UtilityAPI})

 #添加可执行文件
 #新增文件时需在此处添加分组
add_executable(${TARGET_NAME} ${SOURCES}  
${DX12API}  ${GameTimer}  ${UtilityAPI} ${TextureLoader} ${MathHelper} ${BasicGeometry} ${FrameResource}
${HLSL_Files})
# 添加链接库
target_link_libraries(${TARGET_NAME}  d3d12.lib dxgi.lib dxguid.lib D3DCompiler.lib winmm.lib d2d1.lib dwrite.lib winmm.lib legacy_stdio_definitions.lib)

# 为项目调试设置输出目录
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${TARGET_NAME})
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})


if (${USE_IMGUI})
    add_subdirectory(ImGui)
    target_compile_definitions(${TARGET_NAME} PRIVATE USE_IMGUI)
    target_link_libraries(${TARGET_NAME} ImGui)
  #  file(COPY ImGui/imgui.ini DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

if (${USE_ASSIMP})
    set(ASSIMP_BUILD_ZLIB ON)
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
    set(ASSIMP_BUILD_TESTS OFF)
    set(ASSIMP_INSTALL OFF)
    set(BUILD_SHARED_LIBS OFF)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
    add_subdirectory("assimp")
    target_link_libraries(${TARGET_NAME} assimp)
endif()