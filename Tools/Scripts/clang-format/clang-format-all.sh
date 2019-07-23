#!/bin/bash

function usage {
    echo "Usage: $0 DIR..."
    exit 1
}

# Variable that will hold the name of the clang-format command
FMT=""

# Some distros just call it clang-format. Others (e.g. Ubuntu) are insistent
# that the version number be part of the command. We prefer clang-format if
# that's present, otherwise we work backwards from highest version to lowest
# version.
for clangfmt in clang-format{,-{4,3}.{9,8,7,6,5,4,3,2,1,0}}; do
    if which "$clangfmt" &>/dev/null; then
        FMT="$clangfmt"
        break
    fi
done

# Check if we found a working clang-format
if [ -z "$FMT" ]; then
    echo "failed to find clang-format"
    exit 1
fi

SRC_PATH="$@"
if [ -z "$SRC_PATH" ]; then
    SRC_PATH="../../../src"
fi

# Check all of the arguments first to make sure they're all directories
for dir in "$SRC_PATH"; do
    if [ ! -d "${dir}" ]; then
        echo "${dir} is not a directory"
        usage
    fi
done

# Run clang-format -i on all of the things
for dir in "$SRC_PATH"; do
    pushd "${dir}" &>/dev/null
    find . \
         \( -name '*.c' \
         -o -name '*.cc' \
         -o -name '*.cpp' \
         -o -name '*.h' \
         -o -name '*.hh' \
         -o -name '*.hpp' \) \
         -exec "${FMT}" -style=file -i '{}' \;
    popd &>/dev/null
done
