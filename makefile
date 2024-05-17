CORE_FILES := $(filter-out Core/main.cpp, $(wildcard Core/*.cpp))

task: Core/main.cpp
	g++ -o task Core/*.cpp

test: Test/Unit/main.cpp $(CORE_FILES)
	g++ -o test Test/Unit/main.cpp $(CORE_FILES) -lgtest -pthread