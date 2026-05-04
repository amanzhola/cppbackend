#!/bin/bash

PROJECT_DIR="/home/almusha/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_8_10_cron"
LOG_FILE="$PROJECT_DIR/logs/cron_hello.log"

CURRENT_TIME="$(date '+%Y-%m-%d %H:%M:%S')"

echo "$CURRENT_TIME: Hello from cron" >> "$LOG_FILE"
