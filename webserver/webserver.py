import socketserver
import http.server

PORT = 8000

#Handler = http.server.SimpleHTTPRequestHandler

#with socketserver.TCPServer(("", PORT), Handler) as httpd:
#    print("serving at port", PORT)
#    httpd.serve_forever()


def some_function():
    print("some_function got called")

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/captureImage':
            # Insert your code here
            some_function()

        self.send_response(200)

httpd = socketserver.TCPServer(("", 8080), MyHandler)
httpd.serve_forever()