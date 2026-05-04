#!/bin/bash

echo "Docker containers:"
sudo docker container ls

echo
echo "Node Exporter process:"
pgrep -a node_exporter || true

echo
echo "Listening ports:"
ss -lntp | grep -E '(:9090|:9100)' || true

echo
echo "Node Exporter metrics sample:"
curl -s http://localhost:9100/metrics | head -20 || true
