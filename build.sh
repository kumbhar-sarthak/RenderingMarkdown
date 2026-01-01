#!/bin/bash
set -e

if [ ! -d "builds" ]; then
    mkdir builds
fi

cd "$(dirname "$0")/builds"

printf "\nCompiling Project...\n\n"

cmake -S ..

printf "\nCompiled successfully. Now building the project...\n\n"

cmake --build .

cd ..
