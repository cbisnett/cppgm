# CPPGM Grading Starter Kit

## Grading Process

The grading system works by making a clean checkout of your `dev.git` repository and executing:

    $ make

This will build the first rule it finds in the `Makefile`.

It will then look for compiled applications produced in the root directory matching the programming assignment codenames.

For each one it finds, it will copy it out into an external testing environment for further analysis.

The initial commit of your `dev.git` repository contains a starter `Makefile` and some stub applications (`pptoken` and `posttoken`) and stub classes (`ExampleClass1`, `ExampleClass2`).

## Starter Makefile Description

The starter Makefile is as follows:

	apps = \
	    	pptoken \
	        posttoken

The `apps` variable contains a list of the applications you wish to build.  Each application `x` must have an associated `x.cpp` file that contains a `main` function.
	
	units = \
	        ExampleClass1 \
	        ExampleClass2

The `units` variable contains a list of the shared cpp files you wish to build.
	
	all: $(apps)

The `all` target is the default target as it appears first in the Makefile.  It depends upon all the applications listed in the apps variable, so they will all be built by this target
	
	CXXFLAGS = -MD -g -O2 -std=gnu++11

This is the list of compiler flags that will be passed to gcc when it creates `.o` files from `.cpp` files.  The `-MD` flag will cause gcc to automatically generated `.d` files that contain extra rules that tell make which `.o` files depend on which `.h` files based on their `#includes`.  `-g` is to generate debug information for `gdb`.  `-O2` is optimization level 2. `-std=gnu++11` tells GCC to switch on C++11 features.
	
	clean:
	        -rm $(apps) *.o *.d

The `clean` target deletes intermediate files.  You should always do this before you commit.  Intermediate files must _never_ be committed or pushed.
	
	$(apps): %: %.o $(units:=.o)
	        g++ -g -O2 -std=gnu++11 $^ -o $@

This is the linking rule for each app.
	
	-include $(units:=.d) $(apps:=.d)

This line includes the previously mentioned `.d` files generated from the `-MD` flag.	

## Adding New Source Files

To add a new application `x`

1. Create a new file `x.cpp` with the application main
2. Add `x` to the `apps` variable in the Makefile

To add a new shared source file `y.cpp`

1. Create `y.cpp` and `y.h`
2. Add `y` to the `units` variable in the Makefile

To add a new header file `z.h`

1. Create and use the header file `z.h`
2. No changes to the Makefile are required

## Restrictions

- All files you commit to `dev.git` (apart from provided starter code) must be your own original hand-written work
- Try not to commit superflous files, in particular do not commit any test scripts or test files to `dev.git`.  The files you commit should be only the ones used in building
- Your build must not depend on any tool/library outside of `build-essential`, which is essentially `make`, `gcc` and `libstdc++` of the appropriate versions

