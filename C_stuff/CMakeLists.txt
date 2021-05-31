cmake_minimum_required(VERSION 3.16)
project(network_client)


set(CMAKE_CXX_STANDARD 20)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

# json
include(FetchContent)

FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()


# GG test
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
        hello_test
        Testing/md5.cc
        Testing/encode.cc
        Testing/make_message.cc
        encode_decode/encode_decode.cpp
        hash/md5.cpp
)

target_link_libraries(
        hello_test
        gtest_main
        nlohmann_json::nlohmann_json
)
#
include(GoogleTest)
gtest_discover_tests(hello_test)


add_subdirectory(cpp-dotenv)


add_executable(${PROJECT_NAME}
        main.cpp
        encode_decode/encode_decode.cpp
        hash/md5.cpp
        network/udp_recv.cpp
        network/udp_send.cpp
        network/resolve_hostname.cpp
        thread_safe_ds/shared_queue.cpp
        )


target_link_libraries(${PROJECT_NAME}
        nlohmann_json::nlohmann_json
        Threads::Threads
        cpp_dotenv
        )

#add_executable(network_client tcp_client.cpp tcp_server.cpp udp_client.cpp udp_server.cpp)