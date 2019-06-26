cmake_minimum_required(VERSION 3.13)
project(calculator CXX)
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER MATCHES ".*clang")
    # Clang disagress with libstdc++ about constexpr-ness of wstring_view
    # See https://github.com/Microsoft/calculator/pull/321
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(CalcManager)
