add_library(epsilon INTERFACE)
target_compile_features(epsilon INTERFACE cxx_std_23)
target_include_directories(epsilon INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
