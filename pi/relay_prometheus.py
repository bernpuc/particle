from time import sleep

# libraries for prometheus
from prometheus_client import Gauge, start_http_server

# Prometheus Gauges for temperature, humidity, and air quality
gt = Gauge('temperature', 'Temperature from sht40 sensor', ['scale'])
gt.labels('celsius')
gt.labels('fahrenheit')

def loop():

    while True:
        sleep(1)
        # TODO: Set the temperature from REST API PUT messages
        temperature = 22.2
        temp_fahrenheit = temperature * 1.8 + 32

        # Set the gauges
        gt.labels('celsius').set(temperature)
        gt.labels('fahrenheit').set(temp_fahrenheit)


if __name__ == "__main__":
    # prometheus will scrape at this address for metrics
    metrics_port = 8002
    start_http_server(metrics_port)
    loop()

