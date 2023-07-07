import socketserver
import http.server

PORT = 8000

def some_function():
    return "some_function got called"

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/conditions':
            # Insert your code here
            conditions_report = some_function()

        self.send_response(200, conditions_report)

httpd = socketserver.TCPServer(("", PORT), MyHandler)
httpd.serve_forever()