import socketserver
import http.server
import subprocess
from datetime import datetime

PORT = 8000

# Local file for output
LOG_FILE = "/var/log/weatherstation/sensorslogs.txt"

def some_function():
    # Pull last line from file (most current reading)
    # Convert timestamp to human readable format
    # Output new html file
    filename = LOG_FILE
    
    line = subprocess.check_output(['tail', '-1', filename], text=True)
    rawdata = line.split(',')
    rawdata[0] = datetime.strptime(rawdata[0],'%Y-%m-%d %H:%M:%S').strftime('%a %B %d %Y, %I:%M%p')
    with open("index.html", "w") as fh:
        fh.write("<html><head></head><body>\n")
        for item in rawdata:
            fh.write("<H1>" + item + "</H1>\n")
#        fh.write("<p><i>Conditions in Naugatuck, CT</i></p>\n")
        fh.write("<p><i>Conditions in Bern's Office</i></p>\n")
        fh.write("<body></html>\n")

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/conditions':
            some_function()
            self.path = '/index.html'
            return http.server.SimpleHTTPRequestHandler.do_GET(self)

httpd = socketserver.TCPServer(("", PORT), MyHandler)
httpd.serve_forever()
