#!/bin/bash

set -e

if [[ "$PWD" == *"/tools"* ]]; then
  echo "This script must be run from the 'root' directory. use ./tools/clearResults.sh"
  return 1 2>/dev/null
fi

rm -f target/*.html

echo "Cleared all test result HTML files."