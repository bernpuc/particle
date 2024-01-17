#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#include "Fonts/FreeMono9pt7b.h"

#define DISPLAY_OFFSET 10
#include "PMS.h"
//#include <SoftwareSerial.h>
//SoftwareSerial SerialPMS(2, 3); // RX, TX
 
PMS pms(Serial2);
PMS::DATA data;

/* Global variables */
char data_buf[12];
uint64_t timeout = 0;
unsigned long timer = 0;
bool showblip = false;
bool swapdata = true;   // start true so the AQI shows up first
byte buttonpin = 8;

/* Function Declarations */
void writeHeader1(char *buf);
void writeHeader2(char *buf);
void writeHeader3(char *buf);
void writeHeader4(char *buf);
void writeData1(char *buf);
void writeData2(char *buf);
void writeData3(char *buf);
void writeData4(char *buf);
void UpdateAQI(uint16_t count);
void UpdatePM1(uint16_t count);
void UpdatePM2(uint16_t count);
void UpdatePM10(uint16_t count);
uint16_t AQICalculator(uint16_t pm25, uint16_t pm10);
void UpdateTemp(float data);
void UpdateHumidity(float data);

void setup(void)
{
  Serial.begin(115200);       // Start the Serial communication to send messages to the computer
  while (!Serial) delay(10);
  Serial.println("Starting Serial2...");
  Serial2.begin(9600);        // Communication with PMS5003 sensor
  while (!Serial2) delay(10);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  // Foreground white, Background black
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
  display.setFont(&FreeMono9pt7b);
  display.clearDisplay();

  delay(10);
  Serial.println('\n');

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("Adafruit SHT4x test");
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  Serial.print("Serial number 0x");
  Serial.println(sht4.readSerial(), HEX);

  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);

  writeHeader1("Starting...");

  pinMode(buttonpin, INPUT);
  digitalWrite(buttonpin, HIGH);
}

void loop(void)
{
  if ((millis() - timeout) > 5000)
  {
    timeout = millis();
    if (swapdata)
    {
      if (pms.readUntil(data))  // Tries to read for 1s then returns
      {
        UpdateAQI(AQICalculator(data.PM_AE_UG_2_5, data.PM_AE_UG_10_0));
        //UpdatePM1(data.PM_AE_UG_1_0);
        UpdatePM2(data.PM_AE_UG_2_5);
        UpdatePM10(data.PM_AE_UG_10_0);
      }
    }
    else
    {
      sensors_event_t humidity, temp;
      
      sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
      UpdateTemp(temp.temperature);
      UpdateHumidity(humidity.relative_humidity);
    }
    swapdata = !swapdata;
  }

  if ((millis() - timer) > 600)
  {
    timer = millis();
    
    if (showblip)
    {
      display.fillCircle(120, 60, 2, WHITE);
    }
    else
    {
      display.fillCircle(120, 60, 2, BLACK);
    }
    display.display();
    showblip = !showblip;
  }

  if (buttonPressed(buttonpin))
  {
    Serial.println("Ding dong!");
    timeout = 0;  // Force display refresh which triggers change to displayed variables
  }

}

/* Returns true when the button changes state to pressed. false otherwise */
bool buttonPressed(byte buttonpin)
{
  byte oldbutton = 1;
  byte button = digitalRead(buttonpin);
  if (button != oldbutton)
  {
    // button state changed
    oldbutton = button;
    if (!button)
    {
      // button is closed
      return true;
    }
  }
  else
  {
    // button is the same
  }
  return false;
}

void mySetCursor(uint16_t x, uint16_t y)
{
  display.setCursor(x, y + DISPLAY_OFFSET);
}

void UpdateAQI(uint16_t count)
{
  writeHeader1("AQI:");
  snprintf(data_buf, sizeof(data_buf), "%4d", count);
  writeData1(data_buf);
}
void UpdatePM1(uint16_t count)
{
  writeHeader2("PM1.0");
  snprintf(data_buf, sizeof(data_buf), "%4d", count);
  writeData2(data_buf);
}

void UpdatePM2(uint16_t count)
{
  writeHeader2("PM2.5");
  snprintf(data_buf, sizeof(data_buf), "%4d", count);
  writeData2(data_buf);
}

void UpdatePM10(uint16_t count)
{
  writeHeader3("PM10");
  snprintf(data_buf, sizeof(data_buf), "%4d", count);
  writeData3(data_buf);
}

void UpdateTemp(float data)
{
  writeHeader2("Temp ");
  display.print((char)247);
  dtostrf((data * 9/5) + 32, 5, 1, data_buf);  //Converting to fahrenheit
  writeData2(data_buf);
}

void UpdateHumidity(float data)
{
  writeHeader3("RH%");
  dtostrf(data, 5, 1, data_buf);
  writeData3(data_buf);
}

void writeData1(char *buf)
{
  display.fillRect(64, 0, 64, 14, BLACK);
  mySetCursor(64, 0);
  display.println(buf);
  display.display();
}
void writeData2(char *buf)
{
  display.fillRect(64, 16, 64, 12, BLACK);
  mySetCursor(64, 16);
  display.println(buf);
  display.display();
}
void writeData3(char *buf)
{
  display.fillRect(64, 32, 64, 12, BLACK);
  mySetCursor(64, 32);
  display.println(buf);
  display.display();
}
void writeData4(char *buf)
{
  display.fillRect(64, 48, 64, 12, BLACK);
  mySetCursor(64, 48);
  display.println(buf);
  display.display();
}

void writeHeader1(char *buf)
{
  display.fillRect(0, 0, 64, 12, BLACK);
  mySetCursor(0, 0);
  display.print(buf);
  display.display();
}
void writeHeader2(char *buf)
{
  display.fillRect(0, 16, 64, 12, BLACK);
  mySetCursor(0, 16);
  display.print(buf);
  display.display();
}
void writeHeader3(char *buf)
{
  display.fillRect(0, 32, 64, 12, BLACK);
  mySetCursor(0, 32);
  display.println(buf);
  display.display();
}
void writeHeader4(const char *buf)
{
  display.fillRect(0, 48, 64, 12, BLACK);
  mySetCursor(0, 48);
  display.println(buf);
  display.display();
}

uint16_t AQICalculator(uint16_t pm25, uint16_t pm10)
{
  const uint8_t aqi[][2] = {
    {0, 50},
    {51, 100},
    {101, 150},
    {151, 200},
    {201, 300},
    {301, 400},
    {401, 500},
  }; 

  const uint16_t pollutant_pm10_0[][2]{
    {0, 54},
    {55, 154},
    {155, 254},
    {255, 354},
    {355, 424},
    {425, 504},
    {505, 604},
  };

  const uint16_t pollutant_pm2_5[][2]{
    {0.0, 12},
    {13, 35},
    {36, 55},
    {56, 150},
    {151, 250},
    {251, 350},
    {351, 500},
  };

  uint16_t bplo = 0;
  uint16_t bphi = 0;
  uint16_t aqilo = 0;
  uint16_t aqihi = 0;
  uint16_t aqi_2_5 = 0;
  uint16_t aqi_10 = 0;

  for (int idx = 0; idx < 7; idx++)
  {
    bplo = pollutant_pm2_5[idx][0];
    bphi = pollutant_pm2_5[idx][1];
    if (pm25 >= bplo && pm25 <= bphi)
    {
      aqilo = aqi[idx][0];
      aqihi = aqi[idx][1];
      aqi_2_5 = (float)(aqihi - aqilo) / (float)(bphi - bplo) * (pm25 - bplo) + aqilo;
    }

    bplo = pollutant_pm10_0[idx][0];
    bphi = pollutant_pm10_0[idx][1];
    if (pm10 >= bplo && pm10 <= bphi)
    {
      aqilo = aqi[idx][0];
      aqihi = aqi[idx][1];
      aqi_10 = (float)(aqihi - aqilo) / (float)(bphi - bplo) * (pm10 - bplo) + aqilo;
    }
    
  }
  return max(aqi_10, aqi_2_5);
}
