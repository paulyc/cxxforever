all: objs

clean:
	rm -rf .deps.lo .deps.o .obj.lo .obj.o

%.cpp.lo: %.cpp
	$(CXX) -c -g $(CFLAGS) $(CXXFLAGS) -fPIC -fPIE -o $@ $<

%.cpp.o: %.cpp
	$(CXX) -c -g $(CFLAGS) $(CXXFLAGS) -o $@ $<

: *.cpp
	for FILE in *.cpp; do touch .deps.lo/${FILE}; done
	rm -f .deps.lo/main.cpp

staticdeps: *.cpp
	touch 
	rm -f .deps.o/main.cpp

sharedobjs: 
	mkdir .obj.lo
	for FILE in .deps.lo/


objs: .deps/*
