#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

sudo docker cp "$PROJECT_DIR/configs/prometheus.yml" prom:/etc/prometheus/prometheus.yml
sudo docker restart prom

echo "Prometheus config updated"
