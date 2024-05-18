CORE_FILES := $(filter-out Core/main.cpp, $(wildcard Core/*.cpp))

FLAGS = -lgtest -pthread

COVERAGE_PROPS = -fprofile-arcs -ftest-coverage

task: Core/main.cpp
	g++ -o task Core/*.cpp

test: Test/Unit/main.cpp $(CORE_FILES)
	g++ -o test Test/Unit/*.cpp $(CORE_FILES) $(FLAGS)