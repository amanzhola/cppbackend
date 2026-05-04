#!/bin/bash

echo "Exporter metrics:"
curl -s http://localhost:9200/metrics | grep -E 'webexporter|webserver_request_duration' || true

echo
echo "Prometheus targets:"
curl -s http://localhost:9090/api/v1/targets | head -c 1000 || true
echo
