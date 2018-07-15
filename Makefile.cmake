all: cmakefiles

.deps:
	touch -r CMakeLists.txt .d.CMakeLists.txt

clean: distclean

distclean:
	rm -rf .d.* build xcodebuild

buildfiles: .deps
	mkdir build && cd build && cmake ..

xcodefiles: .deps
	mkdir xcodebuild && cd xcodebuild && cmake -G Xcode ..

cmakefiles: distclean buildfiles xcodefiles
