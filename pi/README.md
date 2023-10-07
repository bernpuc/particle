# Weather Station on the Raspberry Pi 4
## Data Handling
Install Prometheus and configure it to read the new target address used by prometheus-client below.

Install prometheus-client library

Modify pi/main_prometheus.py to store sensor readings in prometheus_client.Gauge() objects.

Install Grafana's PDC service - Private Datasource Connect service

Configure Grafana data source for Prometheus/PDC

## Helpful Links
https://opensource.com/article/21/7/home-temperature-raspberry-pi-prometheus

## Diagnosing Failure
1. Check main_prometheus.py is running
2. Check prometheus_client is exporting data at localhost:8001
3. Check prometheus server is exporting data at localhost:9090
4. Check Private Datasource Connect (PDC) service is running
5. Check IP address is still the same
