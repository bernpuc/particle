'''
This script starts two servers. First the prometheus_client server is started on a background thread.
This is the metrics port which provides the prometheus scraper with metrics data.
The second server is a simple REST API to receive data from an external sensor via an HTTP GET.
URI string format is /update?temperature=XX.XX. We use a regex to extract the temperature data from the string.
'''
import re

# libraries for webserver
import http.server
import socketserver

# libraries for prometheus
from prometheus_client import Gauge, start_http_server

# Prometheus Gauge for temperature
gt = Gauge('temperature', 'Temperature from sht40 sensor', ['scale'])
gt.labels('fahrenheit')


class MyRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        pattern = re.compile(r'/update\?temperature=([\d.]+)&id=([\w%]+)&units=([\w]+)')
        match = pattern.search(self.path)
        if match:
            temperature = match.group(1)
            id_value = match.group(2)
            units = match.group(3)

            self.send_response(200)
            gt.labels(units.lower()).set(temperature)
        else:
            self.send_response(404)
        self.end_headers()

# Define a function to start the web server
def start_web_server(PORT):

    Handler = MyRequestHandler
    httpd = socketserver.TCPServer(("", PORT), Handler)

    print(f"Serving at http://localhost:{PORT}")
    httpd.serve_forever()


if __name__ == "__main__":
    # started on a separate thread
    # prometheus will scrape at this address for metrics
    metrics_port = 8002
    start_http_server(metrics_port)

    # Create a new process for the web server
    rest_port = 8010
    start_web_server(rest_port)

