
CXX = clang++
RM = rm -f
CXXFLAGS = -g --std=c++11 -Wall -I ./src
CPPFLAGS = -c
GTESTFLAGS = -pthread lib/libgmock.a lib/libgtest.a lib/gmock-all.o lib/gtest-all.o

CPP_FILES = $(wildcard src/*.cpp)
HEADER_FILES = $(wildcard src/*.h)
TEST_FILES = $(wildcard test/*.cpp)
MAIN = src/main.cpp
SRCS = $(filter-out $(MAIN), $(CPP_FILES))
OBJS = $(addprefix bin/,$(notdir $(SRCS:.cpp=.o)))

EXECUTABLE = bpl
TEST_EXECUTABLE = runTests

TARGETS = $(EXECUTABLE) $(OBJS) $(TEST_OBJS) $(TEST_EXECUTABLE)

all: $(TARGETS)

test: $(TEST_OBJS) $(TEST_EXECUTABLE)

bin/%.o: src/%.cpp src/%.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

test/bin/%.o: test/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GTESTFLAGS) -o $@ $<

$(EXECUTABLE): $(OBJS) $(MAIN)
	$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN) -o $(EXECUTABLE)

$(TEST_EXECUTABLE): $(OBJS) $(TEST_FILES)
	$(CXX) $(CXXFLAGS) $(GTESTFLAGS) -Ilib/gtest -Ilib/gmock $(OBJS) $(TEST_OBJS) $(TEST_FILES) -o $(TEST_EXECUTABLE)

clean:
	$(RM) $(TARGETS)
	$(RM) bin/* asm/*
