#!/bin/bash

# This script takes the base name of a C++ file (without the .cpp extension) as an argument.

# Check if an argument is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <base name of C++ file>"
    exit 1
fi

# The base name of the file
BASE_NAME=$1

# Flags used

# Debug mode
DEBUG="-ggdb"
# Release mode
RELEASE="-O2 -DNDEBUG"

# Disable compiler extensions
NOEXT="-pedantic-errors"

# Show all the warnings
WARNINGS="-Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion"

# Treat warnings as errors
ERRONWARN="-Werror"

# What language standard to use
STANDARD="-std=c++2a"

# Compile the file with GCC, including the .cpp extension for the input file and using the base name for the output
# g++ -Wall -Werror -std=c++17 "${BASE_NAME}.cpp" -o "${BASE_NAME}"

g++ "${BASE_NAME}.cpp" -o "${BASE_NAME}" $DEBUG $NOEXT $WARNINGS $ERRONWARN $STANDARD

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Output file is ${BASE_NAME}"
else
    echo "Compilation failed."
fi

