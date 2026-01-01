#!/bin/bash

echo "This is Markdown Engine."

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

BIN="$SCRIPT_DIR/builds/MarkdownEngine"

TEST_DIR="$SCRIPT_DIR/target"

shopt -s nullglob

FILES=( "$TEST_DIR"/*.md "$TEST_DIR"/*.mdx)

if [ ${#FILES[@]} -eq 0 ]; then
    echo "No .md or .mdx files found in $TEST_DIR"
    exit 1
fi

"$BIN" "${FILES[@]}"