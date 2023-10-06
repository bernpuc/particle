import sys
from time import sleep
from datetime import datetime
import urllib.request
from decimal import *
import requests
import json

# libraries for prometheus
from prometheus_client import Gauge, start_http_server

# libraries for pms5003
import aqi
import serial
from adafruit_pm25.uart import PM25_UART

# libraries for sht40
import board
import adafruit_sht4x

# libraries for uptimed
import uptime

# Local file for output
LOG_FILE = "/var/log/weatherstation/sensorslogs.txt"
sensorhandle = None

# Prometheus Gauges for temperature, humidity, and air quality
gt = Gauge('temperature', 'Temperature from sht40 sensor', ['scale'])
gt.labels('celsius')
gt.labels('fahrenheit')
gh = Gauge('humidity', 'Relative humidity from sht40 sensor')
ga = Gauge('AQI', 'Air Quality Index from pms5003 sensor')
gu = Gauge('Uptime', 'Server uptime')

def init():
    # initialize pms5003
    reset_pin = None
    uart = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=0.25)
    pm25 = PM25_UART(uart, reset_pin)

    # initialize sht40
    i2c = board.I2C()
    sht = adafruit_sht4x.SHT4x(i2c)
    print("Found SHT40 with serial number", hex(sht.serial_number))
    print("Current mode is: ", adafruit_sht4x.Mode.string[sht.mode])

    return pm25, sht



def loop(pm25, sht):
    # Interval to print stdout
    stdout_interval = 60
    stdout_time = datetime(2023,1,1)

    # Interval to update system uptime
    uptime_interval = 3600
    uptime_time = datetime(2023,1,1)

    # Open log file for sensor data output
    sensorhandle = open(LOG_FILE, "a")

    while True:

        sleep(1)

        # Get PMS5003 data
        try:
            aqdata = pm25.read()
            myaqi_env = aqi.to_aqi([
                (aqi.POLLUTANT_PM10, aqdata["pm10 env"]),
                (aqi.POLLUTANT_PM25, aqdata["pm25 env"])
            ])

        except RuntimeError:
            print("Unable to read from sensor (pms5003), retrying...")
            continue
        except IndexError:
            print("list index out of range error")
            continue

        # Get SHT40 data
        temperature, relative_humidity = sht.measurements
        temp_fahrenheit = temperature * 1.8 + 32

        # Set the gauges
        gt.labels('celsius').set(temperature)
        gt.labels('fahrenheit').set(temp_fahrenheit)
        gh.set(relative_humidity)
        ga.set(myaqi_env)

        time_now = datetime.now()
        
        # stdout
        if (time_now -  stdout_time).total_seconds() > stdout_interval:
            stdout_time = time_now
# Console print disabled. So only error messages get displayed in the service status call (when run as a system service)
#            print("{:%Y-%m-%d %H:%M:%S}, AQI: {:}, {:0.1f} F, {:0.1f} %".format(datetime.now(), int(round(myaqi_env)), temp_fahrenheit, relative_humidity))
#            sys.stdout.flush()
            if sensorhandle:
                sensorhandle.write("{:%Y-%m-%d %H:%M:%S}, AQI: {:}, {:0.1f} F, {:0.1f} % RH\n".format(datetime.now(), int(round(myaqi_env)), temp_fahrenheit, relative_humidity))
                sensorhandle.flush()

        # Check system uptime
        if (time_now - uptime_time).total_seconds() > uptime_interval:
            uptime_time = time_now
            up = uptime.get_percent_uptime()
            if up is not None:
                print("Percent Uptime:", up)
                gu.set(up)
            else:
                print("Unable to retrieve percent uptime.")


if __name__ == "__main__":
    # prometheus will scrape at this address for metrics
    metrics_port = 8001
    start_http_server(metrics_port)
    pmt, sht = init()
    loop(pmt, sht)
