#!/bin/bash

VERSION="1.4.0"
DIR="$HOME/node_exporter-${VERSION}.linux-amd64"

cd "$DIR"

nohup ./node_exporter > "$HOME/node_exporter.log" 2>&1 &

echo "Node Exporter started in background"
echo "Log file: $HOME/node_exporter.log"
echo "Metrics URL: http://SERVER_IP:9100/metrics"
