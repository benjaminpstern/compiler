
CXX = clang++
RM = rm -f
CXXFLAGS = -g --std=c++11 -Wall -I ./src
CPPFLAGS = -c
GTESTFLAGS = -isystem ${GTEST_DIR}/include -pthread -isystem ${GMOCK_DIR}/include lib/libgmock.a lib/libgtest.a lib/gmock-all.o lib/gtest-all.o

CPP_FILES = $(wildcard src/*.cpp)
TEST_FILES = $(wildcard test/*.cpp)
MAIN = src/main.cpp
TESTS = $(addprefix test/,$(notdir $(TEST_FILES:.cpp=.exe)))
SRCS = $(filter-out $(MAIN), $(CPP_FILES))
OBJS = $(addprefix bin/,$(notdir $(SRCS:.cpp=.o)))

EXECUTABLE = bpl

TARGETS = $(EXECUTABLE) $(OBJS) $(TESTS)

all: $(TARGETS)

test: $(TESTS)

bin/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

test/%.exe: test/%.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(GTESTFLAGS) -o $@ $<

$(EXECUTABLE): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(MAIN) -o $(EXECUTABLE)

clean:
	$(RM) $(OBJS) $(TESTS) $(EXECUTABLE)
