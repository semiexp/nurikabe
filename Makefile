CXX = g++
CXXFLAGS = -Wall -Wextra -Wno-sign-compare -O3

SOURCELIST = main.cpp frontend/pb_frontend.cpp testing/graph_test.cpp testing/problem_test.cpp util/util.cpp core/ex_black.cpp core/ex_white.cpp core/field.cpp core/solver.cpp core/reachability.cpp
SOURCE = $(addprefix src/, $(SOURCELIST))
HEADER_P = main.h targetver.h core/graph.h core/nurikabe.h util/util.h
HEADER = $(addprefix src/, $(HEADERLIST))
OBJ = $(SOURCE:%.cpp=%.o)

all: solver

.cpp.o :
	$(CXX) $(CXXFLAGS) -c $< -o $@

solver : $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY:	solver clean

clean:
	rm -rf $(OBJ) solver