# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/test12.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test12.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test12.dir/flags.make

CMakeFiles/test12.dir/test.c.o: CMakeFiles/test12.dir/flags.make
CMakeFiles/test12.dir/test.c.o: ../test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/test12.dir/test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/test12.dir/test.c.o   -c "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/test.c"

CMakeFiles/test12.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/test12.dir/test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/test.c" > CMakeFiles/test12.dir/test.c.i

CMakeFiles/test12.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/test12.dir/test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/test.c" -o CMakeFiles/test12.dir/test.c.s

CMakeFiles/test12.dir/test.c.o.requires:

.PHONY : CMakeFiles/test12.dir/test.c.o.requires

CMakeFiles/test12.dir/test.c.o.provides: CMakeFiles/test12.dir/test.c.o.requires
	$(MAKE) -f CMakeFiles/test12.dir/build.make CMakeFiles/test12.dir/test.c.o.provides.build
.PHONY : CMakeFiles/test12.dir/test.c.o.provides

CMakeFiles/test12.dir/test.c.o.provides.build: CMakeFiles/test12.dir/test.c.o


# Object files for target test12
test12_OBJECTS = \
"CMakeFiles/test12.dir/test.c.o"

# External object files for target test12
test12_EXTERNAL_OBJECTS =

test12: CMakeFiles/test12.dir/test.c.o
test12: CMakeFiles/test12.dir/build.make
test12: CMakeFiles/test12.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable test12"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test12.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test12.dir/build: test12

.PHONY : CMakeFiles/test12.dir/build

CMakeFiles/test12.dir/requires: CMakeFiles/test12.dir/test.c.o.requires

.PHONY : CMakeFiles/test12.dir/requires

CMakeFiles/test12.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test12.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test12.dir/clean

CMakeFiles/test12.dir/depend:
	cd "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles/test12.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/test12.dir/depend
