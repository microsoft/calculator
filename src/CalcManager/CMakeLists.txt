add_library(CalcManager
	CalculatorHistory.cpp
	CalculatorManager.cpp
	ExpressionCommand.cpp
	pch.cpp
	UnitConverter.cpp
)
target_include_directories(CalcManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Ratpack)
add_subdirectory(CEngine)
