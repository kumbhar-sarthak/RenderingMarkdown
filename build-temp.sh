#!/bin/bash
set -e

cd "$(dirname "$0")/builds"

printf "\nBuilding Markdown Engine...\n\n"

cmake -S ..

printf "\nCompiled successfully. Now building the project...\n\n"

cmake --build .

cd ..
