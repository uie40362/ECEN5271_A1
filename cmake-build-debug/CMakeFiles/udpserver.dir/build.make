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
include CMakeFiles/udpserver.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/udpserver.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/udpserver.dir/flags.make

CMakeFiles/udpserver.dir/Server/udp_server.c.o: CMakeFiles/udpserver.dir/flags.make
CMakeFiles/udpserver.dir/Server/udp_server.c.o: ../Server/udp_server.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/udpserver.dir/Server/udp_server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/udpserver.dir/Server/udp_server.c.o   -c "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/Server/udp_server.c"

CMakeFiles/udpserver.dir/Server/udp_server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/udpserver.dir/Server/udp_server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/Server/udp_server.c" > CMakeFiles/udpserver.dir/Server/udp_server.c.i

CMakeFiles/udpserver.dir/Server/udp_server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/udpserver.dir/Server/udp_server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/Server/udp_server.c" -o CMakeFiles/udpserver.dir/Server/udp_server.c.s

CMakeFiles/udpserver.dir/Server/udp_server.c.o.requires:

.PHONY : CMakeFiles/udpserver.dir/Server/udp_server.c.o.requires

CMakeFiles/udpserver.dir/Server/udp_server.c.o.provides: CMakeFiles/udpserver.dir/Server/udp_server.c.o.requires
	$(MAKE) -f CMakeFiles/udpserver.dir/build.make CMakeFiles/udpserver.dir/Server/udp_server.c.o.provides.build
.PHONY : CMakeFiles/udpserver.dir/Server/udp_server.c.o.provides

CMakeFiles/udpserver.dir/Server/udp_server.c.o.provides.build: CMakeFiles/udpserver.dir/Server/udp_server.c.o


# Object files for target udpserver
udpserver_OBJECTS = \
"CMakeFiles/udpserver.dir/Server/udp_server.c.o"

# External object files for target udpserver
udpserver_EXTERNAL_OBJECTS =

udpserver: CMakeFiles/udpserver.dir/Server/udp_server.c.o
udpserver: CMakeFiles/udpserver.dir/build.make
udpserver: CMakeFiles/udpserver.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable udpserver"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/udpserver.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/udpserver.dir/build: udpserver

.PHONY : CMakeFiles/udpserver.dir/build

CMakeFiles/udpserver.dir/requires: CMakeFiles/udpserver.dir/Server/udp_server.c.o.requires

.PHONY : CMakeFiles/udpserver.dir/requires

CMakeFiles/udpserver.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/udpserver.dir/cmake_clean.cmake
.PHONY : CMakeFiles/udpserver.dir/clean

CMakeFiles/udpserver.dir/depend:
	cd "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug" "/mnt/c/Users/goshe/Personal/NUS Matters/Y3S2/ECEN5273/Assignment/Assignment 1/udp/cmake-build-debug/CMakeFiles/udpserver.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/udpserver.dir/depend

