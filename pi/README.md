# Weather Station on the Raspberry Pi 4
## Data Handling
Install Prometheus and configure it to read the new target address used by prometheus-client below.

Install prometheus-client library

Modify pi/main_prometheus.py to store sensor readings in prometheus_client.Gauge() objects.

Install Grafana's PDC service - Private Datasource Connect service

Configure Grafana data source for Prometheus/PDC

## Helpful Links
https://opensource.com/article/21/7/home-temperature-raspberry-pi-prometheus
