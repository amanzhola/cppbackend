#!/bin/bash

sudo ufw allow 3000/tcp || true

if sudo docker ps -a --format '{{.Names}}' | grep -q '^grafana$'; then
    sudo docker rm -f grafana
fi

sudo docker run -d --name=grafana --network="host" grafana/grafana

echo "Grafana started"
echo "URL: http://SERVER_IP:3000/"
echo "Default login: admin"
echo "Default password: admin"
