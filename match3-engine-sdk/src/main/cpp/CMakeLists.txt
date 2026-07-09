cmake_minimum_required(VERSION 3.22.1)

project("match3enginesdk")

set(CMAKE_CXX_STANDARD 17)

if(NOT ANDROID)
    find_package(JNI REQUIRED)
endif()

set(SOURCE_FILES
        match3_engine.cpp
        my_jni.cpp
)

add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCE_FILES})

if(ANDROID)
    target_link_libraries(${CMAKE_PROJECT_NAME}
            android
            log
    )
else()
    target_link_libraries(${CMAKE_PROJECT_NAME} ${JNI_LIBRARIES})

    if(WIN32)
        set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES PREFIX "")
    endif()
endif()

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${JNI_INCLUDE_DIRS}
)