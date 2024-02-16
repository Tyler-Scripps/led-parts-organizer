import ipaddress
import wifi
import os
import socketpool

from adafruit_httpserver import Server, Request, Response, FileResponse, GET, POST

print("Connecting to WiFi")

print('SSID: ', os.getenv('WIFI_SSID'))
print('IP: ', os.getenv('IP'))

#set ipv4 if applicable
if os.getenv('IP') and len(os.getenv('IP')) > 0:
    ipv4 = ipaddress.IPv4Address(os.getenv('IP'))

#set gateway if applicable
if os.getenv('GATEWAY') and len(os.getenv('GATEWAY')) > 0:
    gateway = ipaddress.IPv4Address(os.getenv('GATEWAY'))

#set netmask if applicable
if os.getenv('NETMASK') and len(os.getenv('NETMASK')) > 0:
    netmask = ipaddress.IPv4Address(os.getenv('NETMASK'))

#if ip, gateway, and netmask are provided set the ipv4 for the radio
if os.getenv('IP') and len(os.getenv('IP')) > 0 and os.getenv('GATEWAY') and len(os.getenv('GATEWAY')) > 0 and os.getenv('NETMASK') and len(os.getenv('NETMASK')) > 0:
    wifi.radio.set_ipv4_address(ipv4=ipv4,netmask=netmask,gateway=gateway)

#connect to wifi
try:
    wifi.radio.connect(os.getenv('WIFI_SSID'), os.getenv('WIFI_PASSWORD'))
    wifiConnected = True
    print("Connected to WiFi")
except:
    wifiConnected = False
    print("WiFi network not found")

#webserver
pool = socketpool.SocketPool(wifi.radio)
server = Server(pool, "/static", debug=True)


@server.route("/", GET)
def base(request: Request):  # pylint: disable=unused-argument
    # print("got request for webpage")
    return FileResponse(request, "index.html")

@server.route("/index.js", GET)
def base(request: Request):  # pylint: disable=unused-argument
    # print("got request for webpage")
    return FileResponse(request, "index.js")