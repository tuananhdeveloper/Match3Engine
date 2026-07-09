cmake_minimum_required(VERSION 3.22.1)
project("match3engine")

set(CMAKE_CXX_STANDARD 17)

# 1. Định nghĩa đường dẫn tới thư mục chứa code của module SDK
set(ENGINE_SDK_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../../../match3-engine-sdk/src/main/cpp")

# 2. Thêm tất cả các file .cpp của SDK vào danh sách biên dịch của thư viện SHARED
if (ANDROID)
    add_library(${CMAKE_PROJECT_NAME} SHARED
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
            main.cpp
            ${ENGINE_SDK_SRC}/match3_engine.cpp
    )
else ()
    add_library(${CMAKE_PROJECT_NAME} SHARED
            ${ENGINE_SDK_SRC}/match3_engine.cpp
    )
endif ()


# 3. Giữ nguyên include đường dẫn header

if (ANDROID)
        target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
                ${ANDROID_NDK}/sources/android/native_app_glue
                ${ENGINE_SDK_SRC}
        )
else ()
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
            ${JNI_INCLUDE_DIRS}
            ${ENGINE_SDK_SRC}
    )
endif ()

if (ANDROID)
    target_link_libraries(${CMAKE_PROJECT_NAME}
            android
            log
    )
else ()
    target_link_libraries(${CMAKE_PROJECT_NAME} ${JNI_LIBRARIES})

    if(WIN32)
        set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
    endif()
endif()