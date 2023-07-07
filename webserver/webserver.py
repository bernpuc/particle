import socketserver
import http.server

PORT = 8000

def some_function():
    return "<http><body>some_function got called<body></http>"

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        conditions_report = "FOOBAR"
        if self.path == '/conditions':
            # Insert your code here
            conditions_report = some_function()

        self.send_response(200, conditions_report)
        self.end_headers()

httpd = socketserver.TCPServer(("", PORT), MyHandler)
httpd.serve_forever()