#!/bin/bash

sudo ufw allow 9090/tcp || true

if sudo docker ps -a --format '{{.Names}}' | grep -q '^prom$'; then
    sudo docker rm -f prom
fi

sudo docker run -d --name prom --network="host" prom/prometheus

echo "Prometheus started"
echo "URL: http://SERVER_IP:9090/"
