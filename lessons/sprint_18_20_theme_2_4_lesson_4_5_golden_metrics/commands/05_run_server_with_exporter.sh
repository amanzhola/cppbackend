#!/bin/bash

sudo ufw allow 80/tcp || true
sudo ufw allow 9200/tcp || true

sudo docker run --rm -p 80:8080 my_http_server | python3 "$HOME/web_exporter.py"
