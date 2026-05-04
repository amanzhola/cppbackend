#!/bin/bash

PROJECT_DIR="/home/almusha/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_8_10_cron"
LOG_FILE="$PROJECT_DIR/logs/cron_cleanup.log"
TMP_DIR="$PROJECT_DIR/logs/tmp"

mkdir -p "$TMP_DIR"

CURRENT_TIME="$(date '+%Y-%m-%d %H:%M:%S')"

find "$TMP_DIR" -type f -mmin +10 -print -delete >> "$LOG_FILE" 2>&1

echo "$CURRENT_TIME: cleanup completed" >> "$LOG_FILE"
