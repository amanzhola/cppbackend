#!/bin/bash

echo "Docker containers:"
sudo docker container ls

echo
echo "Listening monitoring ports:"
ss -lntp | grep -E '(:3000|:9090|:9100)' || true

echo
echo "Node Exporter sample:"
curl -s http://localhost:9100/metrics | head -5 || true

echo
echo "Prometheus readiness:"
curl -s http://localhost:9090/-/ready || true

echo
echo
echo "Grafana health:"
curl -s http://localhost:3000/api/health || true
echo
