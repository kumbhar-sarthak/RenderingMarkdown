#!/bin/bash

echo "This is Markdown Engine Starter"

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

BIN="$SCRIPT_DIR/builds/MarkdownEngine"

FILE="${1:-$SCRIPT_DIR/tests/test.md}"
FILE2="${1:-$SCRIPT_DIR/tests/test-2.md}"

"$BIN" "$FILE" "$FILE2"