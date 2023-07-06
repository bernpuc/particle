from time import sleep
from datetime import datetime
import urllib.request
from decimal import *


def init():
    # libraries for pms5003
    import aqi
    import serial
    from adafruit_pm25.uart import PM25_UART
    reset_pin = None
    uart = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=0.25)
    pm25 = PM25_UART(uart, reset_pin)

    # libraries for sht40
    import board
    import adafruit_sht4x
    i2c = board.I2C()
    sht = adafruit_sht4x.SHT4x(i2c)
    print("Found SHT40 with serial number", hex(sht.serial_number))
    print("Current mode is: ", adafruit_sht4x.Mode.string[sht.mode])
    return pm25, sht


def loop(pm25, sht):
    # Interval to upload to thingspeak
    post_interval = 600	# 10 minutes
    post_time = datetime(2023,1,1)

    # Interval to average AQI
    aqi_average_interval = 600
    aqi_average_time = datetime.now()
    N = 0
    avg_aqi_env = 0

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
            print("Unable to read from sensor, retrying...")
            continue
        except IndexError:
            print("list index out of range error")
            continue

        # Get SHT40 data
        temperature, relative_humidity = sht.measurements
        temp_fahrenheit = temperature * 1.8 + 32

        # stdout
        print("{:%Y-%m-%d %H:%M:%S}, AQI: {:0.1f}, {:0.1f} F, {:0.1f} %".format(datetime.now(), avg_aqi_env, temp_fahrenheit, relative_humidity))

        # Post data to cloud server at specified intervals
        if (datetime.now() -  post_time).total_seconds() > post_interval:
            print("Post data")
            post_time = datetime.now()
            # GET https://api.thingspeak.com/update?api_key=T33R8EOP0J4YNDU0&field1=0&field2=0&field3=0
            URL = 'https://api.thingspeak.com/update?'
            KEY = 'api_key=T33R8EOP0J4YNDU0'
            HEADER = '&field1={:0.1f}&field2={:0.1f}&field3={:0.1f}'.format(avg_aqi_env,temp_fahrenheit,relative_humidity)
            NEW_URL = URL+KEY+HEADER
            data=urllib.request.urlopen(NEW_URL)
            print(data)

        # Reset average aqi interval
        if (datetime.now() - aqi_average_time).total_seconds() > aqi_average_interval:
            aqi_average_time = datetime.now()
            N = 0
            avg_aqi_env = 0

if __name__ == "__main__":
    pmt, sht = init()
    loop(pmt, sht)
