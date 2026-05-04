#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

sudo ufw allow 9200/tcp || true

cat "$PROJECT_DIR/exporter/test_logs.txt" | python3 "$PROJECT_DIR/exporter/web_exporter.py"
