import sys
from time import sleep
from datetime import datetime
import urllib.request
from decimal import *
import requests
import json

# libraries for pms5003
import aqi
import serial
from adafruit_pm25.uart import PM25_UART

# libraries for sht40
import board
import adafruit_sht4x

# libraries for uptimed
import uptime

# API KEY For thingspeak
API_KEY_THINGSPEAK = 'T33R8EOP0J4YNDU0'

# Local file for output
LOG_FILE = "/var/log/weatherstation/sensorslogs.txt"
sensorhandle = None

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
    # Interval to upload to thingspeak
    post_interval = 600	# 10 minutes
    post_time = datetime(2023,1,1)

    # Interval to upload to Griffin server
    post_interval_g = 600	# 10 minutes
    post_time_g = datetime(2023,1,1)

    # Interval to print stdout
    stdout_interval = 60
    stdout_time = datetime(2023,1,1)

    # Interval to average AQI
    aqi_average_interval = 600
    aqi_average_time = datetime(2023,1,1)
    N = 0
    avg_aqi_env = 0

    # Interval to upload system uptime
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
            N = Decimal(N + 1)
            a = Decimal(1/N)
            b = Decimal(1 - a)
            avg_aqi_env = (a * myaqi_env) + (b * avg_aqi_env)

        except RuntimeError:
            print("Unable to read from sensor (pms5003), retrying...")
            continue
        except IndexError:
            print("list index out of range error")
            continue

        # Get SHT40 data
        temperature, relative_humidity = sht.measurements
        temp_fahrenheit = temperature * 1.8 + 32

        time_now = datetime.now()
        
        # stdout
        if (time_now -  stdout_time).total_seconds() > stdout_interval:
            stdout_time = time_now
# Console print disabled. So only error messages get displayed in the service status call (when run as a system service)
#            print("{:%Y-%m-%d %H:%M:%S}, AQI: {:}, {:0.1f} F, {:0.1f} %".format(datetime.now(), int(round(avg_aqi_env)), temp_fahrenheit, relative_humidity))
            sys.stdout.flush()
            if sensorhandle:
                sensorhandle.write("{:%Y-%m-%d %H:%M:%S}, AQI: {:}, {:0.1f} F, {:0.1f} %\n".format(datetime.now(), int(round(myaqi_env)), temp_fahrenheit, relative_humidity))
                sensorhandle.flush()

        # Post data to thingspeak server at specified intervals
        if (time_now -  post_time).total_seconds() > post_interval:
            post_time = time_now
            # GET https://api.thingspeak.com/update?api_key=T33R8EOP0J4YNDU0&field1=0&field2=0&field3=0
            URL = 'https://api.thingspeak.com/update?'
            KEY = 'api_key='+API_KEY_THINGSPEAK
            FIELD1 = '&field1={:}'.format(int(round(avg_aqi_env)))
            FIELD2 = '&field2={:0.1f}'.format(temp_fahrenheit)
            FIELD3 = '&field3={:0.1f}'.format(relative_humidity)
            NEW_URL = URL+KEY+FIELD1+FIELD2+FIELD3
# Posting to ThingSpeak is disabled
#            data=urllib.request.urlopen(NEW_URL)
#            print(data)

        # Post to Grafana server
        # https://library.terramisha.com
        if (time_now - post_time_g).total_seconds() > post_interval_g:
            post_time_g = time_now
            URL = 'https://api.terramisha.com/api/postWeatherParameter'
            HEADERS = {'Content-type': 'application/json', 'Accept': 'text/plain'}
            JSONDATA = {'date':datetime.utcnow().strftime("%Y-%m-%dT%H:%M:%SZ"),
                        'temperatureF': temp_fahrenheit, 
                        'aqi': int(round(avg_aqi_env)), 
                        'humidityPcntg': relative_humidity}
            r = requests.post(URL, data=json.dumps(JSONDATA), headers=HEADERS)
            print(r)

        # Reset average aqi interval
        if (time_now - aqi_average_time).total_seconds() > aqi_average_interval:
            aqi_average_time = time_now
            N = 0
            avg_aqi_env = 0

        # Check system uptime
        if (time_now - uptime_time).total_seconds() > uptime_interval:
            uptime_time = time_now
            up = uptime.get_percent_uptime()
            if up is not None:
                print("Percent Uptime:", up)
            else:
                print("Unable to retrieve percent uptime.")


def makeJsonBody():
    return ''


if __name__ == "__main__":
    pmt, sht = init()
    loop(pmt, sht)
