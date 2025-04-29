CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

TARGET = program
TESTTARGET = test
TEST1TARGET = test1
TULGUURTARGET = tulguur_shiid
YRUNHIITULGUURTARGET = yrunhii_tulguur_shiid
ONOVCHTOITARGET = onovchtoi_shiid

OBJS = main.o fraction.o
TESTOBJS = test.o fraction.o
TEST1OBJS = test1.o fraction.o
TULGUUROBJS = tulguur_shiid.o fraction.o
YRUNHIIOBJS = yrunhii_tulguur_shiid.o fraction.o
ONOVCHTOIOBJS = onovchtoi_shiid.o fraction.o

all: $(TARGET) $(TESTTARGET) $(TEST1TARGET) $(TULGUURTARGET) $(YRUNHIITULGUURTARGET) $(ONOVCHTOITARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

$(TESTTARGET): $(TESTOBJS)
	$(CXX) $(CXXFLAGS) -o $(TESTTARGET) $(TESTOBJS)

$(TEST1TARGET): $(TEST1OBJS)
	$(CXX) $(CXXFLAGS) -o $(TEST1TARGET) $(TEST1OBJS)

$(TULGUURTARGET): $(TULGUUROBJS)
	$(CXX) $(CXXFLAGS) -o $(TULGUURTARGET) $(TULGUUROBJS)

$(YRUNHIITULGUURTARGET): $(YRUNHIIOBJS)
	$(CXX) $(CXXFLAGS) -o $(YRUNHIITULGUURTARGET) $(YRUNHIIOBJS)

$(ONOVCHTOITARGET): $(ONOVCHTOIOBJS)
	$(CXX) $(CXXFLAGS) -o $(ONOVCHTOITARGET) $(ONOVCHTOIOBJS)

main.o: main.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c main.cpp

test.o: test.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c test.cpp

test1.o: test1.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c test1.cpp

fraction.o: fraction.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c fraction.cpp

tulguur_shiid.o: tulguur_shiid.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c tulguur_shiid.cpp

yrunhii_tulguur_shiid.o: yrunhii_tulguur_shiid.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c yrunhii_tulguur_shiid.cpp

onovchtoi_shiid.o: onovchtoi_shiid.cpp fraction.h
	$(CXX) $(CXXFLAGS) -c onovchtoi_shiid.cpp

clean:
	rm -f $(OBJS) $(TESTOBJS) $(TEST1OBJS) $(TULGUUROBJS) $(YRUNHIIOBJS) $(TARGET) $(TESTTARGET) $(TEST1TARGET) $(TULGUURTARGET) $(YRUNHIITULGUURTARGET) $(ONOVCHTOITARGET) $(ONOVCHTOITARGET)
