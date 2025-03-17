CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = program
TESTTARGET = test
TULGUURTARGET = tulguur_shiid

OBJS = main.o fraction.o
TESTOBJS = test.o fraction.o
TULGUUROBJS = tulguur_shiid.o fraction.o

all: $(TARGET) $(TESTTARGET) $(TULGUURTARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(TESTTARGET): $(TESTOBJS)
	$(CXX) $(CXXFLAGS) -o $(TESTTARGET) $(TESTOBJS)

$(TULGUURTARGET): $(TULGUUROBJS)
	$(CXX) $(CXXFLAGS) -o $(TULGUURTARGET) $(TULGUUROBJS)

main.o: main.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c main.cpp

test.o: test.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c test.cpp

fraction.o: fraction.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c fraction.cpp

tulguur_shiid.o: tulguur_shiid.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c tulguur_shiid.cpp

clean:
	rm -f $(OBJS) $(TESTOBJS) $(TULGUUROBJS) $(TARGET) $(TESTTARGET) $(TULGUURTARGET)
