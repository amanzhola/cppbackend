#!/bin/bash

set -e

VERSION="1.4.0"
ARCHIVE="node_exporter-${VERSION}.linux-amd64.tar.gz"
DIR="node_exporter-${VERSION}.linux-amd64"
URL="https://github.com/prometheus/node_exporter/releases/download/v${VERSION}/${ARCHIVE}"

cd "$HOME"

if [ ! -f "$ARCHIVE" ]; then
    wget "$URL"
fi

if [ ! -d "$DIR" ]; then
    tar xvzf "$ARCHIVE"
fi

sudo ufw allow 9100/tcp || true

echo "Node Exporter is ready in $HOME/$DIR"
echo "Run:"
echo "cd $HOME/$DIR"
echo "./node_exporter"
