# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
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
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/patrick/projects/pico/sideset_test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/patrick/projects/pico/sideset_test/build

# Utility rule file for blink_blink_pio_h.

# Include any custom commands dependencies for this target.
include CMakeFiles/blink_blink_pio_h.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/blink_blink_pio_h.dir/progress.make

CMakeFiles/blink_blink_pio_h: blink.pio.h

blink.pio.h: ../blink.pio
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/patrick/projects/pico/sideset_test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating blink.pio.h"
	pioasm-install/pioasm/pioasm -o c-sdk -v 0 /home/patrick/projects/pico/sideset_test/blink.pio /home/patrick/projects/pico/sideset_test/build/blink.pio.h

blink_blink_pio_h: CMakeFiles/blink_blink_pio_h
blink_blink_pio_h: blink.pio.h
blink_blink_pio_h: CMakeFiles/blink_blink_pio_h.dir/build.make
.PHONY : blink_blink_pio_h

# Rule to build all files generated by this target.
CMakeFiles/blink_blink_pio_h.dir/build: blink_blink_pio_h
.PHONY : CMakeFiles/blink_blink_pio_h.dir/build

CMakeFiles/blink_blink_pio_h.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/blink_blink_pio_h.dir/cmake_clean.cmake
.PHONY : CMakeFiles/blink_blink_pio_h.dir/clean

CMakeFiles/blink_blink_pio_h.dir/depend:
	cd /home/patrick/projects/pico/sideset_test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/patrick/projects/pico/sideset_test /home/patrick/projects/pico/sideset_test /home/patrick/projects/pico/sideset_test/build /home/patrick/projects/pico/sideset_test/build /home/patrick/projects/pico/sideset_test/build/CMakeFiles/blink_blink_pio_h.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/blink_blink_pio_h.dir/depend

