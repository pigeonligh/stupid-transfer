build: dir_exists
	cd build && cmake -G "Unix Makefiles" .. && make

dir_exists:
	@if [ ! -d "./build/" ]; then mkdir build; fi
	@if [ ! -d "./bin/" ]; then mkdir bin; fi

clean:
	rm -rf build/* bin/*