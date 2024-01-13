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

#include "PMS.h"
#include <SoftwareSerial.h>
SoftwareSerial SerialPMS(2, 3); // RX, TX
 
PMS pms(SerialPMS);
PMS::DATA data;

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

void setup(void){
  SerialPMS.begin(9600);        // Communication with PMS5003 sensor

  Serial.begin(115200);       // Start the Serial communication to send messages to the computer

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  // Foreground white, Background black
  display.setTextColor(SSD1306_WHITE, SSD1306_BLACK); 

  delay(10);
  Serial.println('\n');

  display.clearDisplay();
  UpdateDataHeaders();
  
}

void loop(void){
  if (pms.read(data))
  {
    UpdateAQI(AQICalculator(data.PM_AE_UG_2_5, data.PM_AE_UG_10_0));
    UpdatePM1(data.PM_AE_UG_1_0);
    UpdatePM2(data.PM_AE_UG_2_5);
    UpdatePM10(data.PM_AE_UG_10_0);
    
    delay(1000);
  }
}

void UpdateDataHeaders()
{
  writeHeader1("AQI:");
  writeHeader2("PM1.0");
  writeHeader3("PM2.5");
  writeHeader4("PM10");
}

void UpdateAQI(uint16_t count)
{
  char buf[5];
  snprintf(buf, 5, "%4d", count);
  writeData1(buf);
}
void UpdatePM1(uint16_t count)
{
  char buf[5];
  snprintf(buf, 5, "%4d", count);
  writeData2(buf);
}

void UpdatePM2(uint16_t count)
{
  char buf[5];
  snprintf(buf, 5, "%4d", count);
  writeData3(buf);
}

void UpdatePM10(uint16_t count)
{
  char buf[5];
  snprintf(buf, 5, "%4d", count);
  writeData4(buf);
}

void writeData1(char *buf)
{
  display.fillRect(64, 0, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(64, 0);
  display.println(buf);
  display.display();
}
void writeData2(char *buf)
{
  display.fillRect(64, 16, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(64, 16);
  display.println(buf);
  display.display();
}
void writeData3(char *buf)
{
  display.fillRect(64, 32, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(64, 32);
  display.println(buf);
  display.display();
}
void writeData4(char *buf)
{
  display.fillRect(64, 48, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(64, 48);
  display.println(buf);
  display.display();
}

void writeHeader1(const char *buf)
{
  display.fillRect(0, 0, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(0, 0);
  display.println(buf);
  display.display();
}
void writeHeader2(const char *buf)
{
  display.fillRect(0, 16, 128, 16, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(0, 16);
  display.println(buf);
  display.display();
}
void writeHeader3(const char *buf)
{
  display.fillRect(0, 32, 128, 16, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(0, 32);
  display.println(buf);
  display.display();
}
void writeHeader4(const char *buf)
{
  display.fillRect(0, 48, 128, 16, SSD1306_BLACK);
  display.setTextSize(2);
  display.setCursor(0, 48);
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
