#搜索制定目录下的源码文件，并添加到 变量 SRC_LIST中
#aux_source_directory (./echo SRC_LIST)


include_directories (../inc)
 

 

# 设置源文件目录
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# 获取 echo 目录下的所有 .cpp 文件
file(GLOB CPP_FILES "${SOURCE_DIR}/*.cpp")

# 遍历每个 .cpp 文件，为其创建一个目标
foreach(CPP_FILE ${CPP_FILES})
  # 获取文件名（不带路径和扩展名）
  get_filename_component(TARGET_NAME ${CPP_FILE} NAME_WE)

  # 添加目标
  add_executable(${TARGET_NAME} ${CPP_FILE})
  
  set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

endforeach()
  include_directories(
    ../../spdlog/include/
  )
