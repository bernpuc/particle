import socketserver
import http.server
import subprocess

PORT = 8000

def some_function():
    filename = '../pi/nohup.out'
    line = subprocess.check_output(['tail', '-1', filename], text=True)
    line = line.replace(",", "</H1><H1>")
    with open("index.html", "w") as fh:
        fh.write("<html><head></head><body><H1>" + line + "</H1><p><i>Conditions in Naugatuck, CT</i></p><body></html>")

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/conditions':
            some_function()
            self.path = '/index.html'
            return http.server.SimpleHTTPRequestHandler.do_GET(self)

httpd = socketserver.TCPServer(("", PORT), MyHandler)
httpd.serve_forever()
