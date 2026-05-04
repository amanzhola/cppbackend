# Dashboard Queries — Four Golden Metrics

## Saturation: CPU Usage

```promql
avg(1 - rate(node_cpu_seconds_total{mode="idle"}[1m])) * 100
