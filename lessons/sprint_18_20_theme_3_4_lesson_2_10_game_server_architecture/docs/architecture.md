# Architecture Notes

## Main idea

The game server should be divided into modules.

```text
http_server → request_handler → app → model
