# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/CSocket.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CSocket.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CSocket.dir/flags.make

CMakeFiles/CSocket.dir/client2.c.o: CMakeFiles/CSocket.dir/flags.make
CMakeFiles/CSocket.dir/client2.c.o: ../client2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CSocket.dir/client2.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CSocket.dir/client2.c.o   -c "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/client2.c"

CMakeFiles/CSocket.dir/client2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CSocket.dir/client2.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/client2.c" > CMakeFiles/CSocket.dir/client2.c.i

CMakeFiles/CSocket.dir/client2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CSocket.dir/client2.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/client2.c" -o CMakeFiles/CSocket.dir/client2.c.s

CMakeFiles/CSocket.dir/client2.c.o.requires:

.PHONY : CMakeFiles/CSocket.dir/client2.c.o.requires

CMakeFiles/CSocket.dir/client2.c.o.provides: CMakeFiles/CSocket.dir/client2.c.o.requires
	$(MAKE) -f CMakeFiles/CSocket.dir/build.make CMakeFiles/CSocket.dir/client2.c.o.provides.build
.PHONY : CMakeFiles/CSocket.dir/client2.c.o.provides

CMakeFiles/CSocket.dir/client2.c.o.provides.build: CMakeFiles/CSocket.dir/client2.c.o


CMakeFiles/CSocket.dir/server2.c.o: CMakeFiles/CSocket.dir/flags.make
CMakeFiles/CSocket.dir/server2.c.o: ../server2.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/CSocket.dir/server2.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CSocket.dir/server2.c.o   -c "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/server2.c"

CMakeFiles/CSocket.dir/server2.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CSocket.dir/server2.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/server2.c" > CMakeFiles/CSocket.dir/server2.c.i

CMakeFiles/CSocket.dir/server2.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CSocket.dir/server2.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/server2.c" -o CMakeFiles/CSocket.dir/server2.c.s

CMakeFiles/CSocket.dir/server2.c.o.requires:

.PHONY : CMakeFiles/CSocket.dir/server2.c.o.requires

CMakeFiles/CSocket.dir/server2.c.o.provides: CMakeFiles/CSocket.dir/server2.c.o.requires
	$(MAKE) -f CMakeFiles/CSocket.dir/build.make CMakeFiles/CSocket.dir/server2.c.o.provides.build
.PHONY : CMakeFiles/CSocket.dir/server2.c.o.provides

CMakeFiles/CSocket.dir/server2.c.o.provides.build: CMakeFiles/CSocket.dir/server2.c.o


# Object files for target CSocket
CSocket_OBJECTS = \
"CMakeFiles/CSocket.dir/client2.c.o" \
"CMakeFiles/CSocket.dir/server2.c.o"

# External object files for target CSocket
CSocket_EXTERNAL_OBJECTS =

CSocket: CMakeFiles/CSocket.dir/client2.c.o
CSocket: CMakeFiles/CSocket.dir/server2.c.o
CSocket: CMakeFiles/CSocket.dir/build.make
CSocket: CMakeFiles/CSocket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable CSocket"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CSocket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CSocket.dir/build: CSocket

.PHONY : CMakeFiles/CSocket.dir/build

CMakeFiles/CSocket.dir/requires: CMakeFiles/CSocket.dir/client2.c.o.requires
CMakeFiles/CSocket.dir/requires: CMakeFiles/CSocket.dir/server2.c.o.requires

.PHONY : CMakeFiles/CSocket.dir/requires

CMakeFiles/CSocket.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CSocket.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CSocket.dir/clean

CMakeFiles/CSocket.dir/depend:
	cd "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket" "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket" "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug" "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug" "/Users/teto/Documents/Cours M1 MIAGE/reseau/CSocket/cmake-build-debug/CMakeFiles/CSocket.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/CSocket.dir/depend

