add_library(CalcManager)
target_include_directories(CalcManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Ratpack)
add_subdirectory(CEngine)
