from time import sleep
# libraries for webserver
import http.server
import socketserver
import re

# libraries for prometheus
from prometheus_client import Gauge, start_http_server

# Prometheus Gauges for temperature, humidity, and air quality
gt = Gauge('temperature', 'Temperature from sht40 sensor', ['scale'])
gt.labels('celsius')
gt.labels('fahrenheit')

def loop():

    while True:
        sleep(1)


class MyRequestHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if re.search('/update*', self.path):
            self.send_response(200)

            #Parse the temperature value from the path
            regex = r"=\d+\.\d+"
            match = re.search(regex, self.path)
            if match:
                value = match.group(0)[1:]
                gt.labels('fahrenheit').set(value)
        else:
            self.send_response(403)
        self.end_headers()
        sleep(1)

# Define a function to start the web server
def start_web_server():
    PORT = 8010
    DIRECTORY = "."

    Handler = MyRequestHandler
    httpd = socketserver.TCPServer(("", PORT), Handler)

    print(f"Serving at http://localhost:{PORT}")

    # TODO: Set the temperature from REST API PUT messages
    temperature = 22.2
    temp_fahrenheit = temperature * 1.8 + 32

    # Set the gauges
    gt.labels('celsius').set(temperature)
    gt.labels('fahrenheit').set(temp_fahrenheit)
    httpd.serve_forever()


if __name__ == "__main__":
    # prometheus will scrape at this address for metrics
    metrics_port = 8002
    start_http_server(metrics_port)

    # Create a new process for the web server
    start_web_server()
#    loop()

