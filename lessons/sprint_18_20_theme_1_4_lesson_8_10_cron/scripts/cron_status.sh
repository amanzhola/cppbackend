#!/bin/bash

PROJECT_DIR="/home/almusha/cppbackend/lessons/sprint_18_20_theme_1_4_lesson_8_10_cron"
LOG_FILE="$PROJECT_DIR/logs/cron_status.log"

CURRENT_TIME="$(date '+%Y-%m-%d %H:%M:%S')"
LOAD_AVERAGE="$(cat /proc/loadavg)"
DISK_USAGE="$(df -h / | tail -1)"

{
    echo "[$CURRENT_TIME] status check"
    echo "load average: $LOAD_AVERAGE"
    echo "disk usage: $DISK_USAGE"
    echo "----"
} >> "$LOG_FILE"
