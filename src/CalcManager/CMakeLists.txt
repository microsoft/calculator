add_library(CEngine
	CEngine/calc.cpp
	CEngine/CalcInput.cpp
	CEngine/CalcUtils.cpp
	CEngine/History.cpp
	CEngine/Number.cpp
	CEngine/Rational.cpp
	CEngine/RationalMath.cpp
	CEngine/scicomm.cpp
	CEngine/scidisp.cpp
	CEngine/scifunc.cpp
	CEngine/scioper.cpp
	CEngine/sciset.cpp
)
target_include_directories(CEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
