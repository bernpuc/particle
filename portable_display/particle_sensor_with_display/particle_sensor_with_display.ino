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
SoftwareSerial Serial1(2, 3); // RX, TX
 
PMS pms(Serial1);
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

void setup(void){
  Serial1.begin(9600);        // Communication with PMS5003 sensor

  Serial.begin(115200);         // Start the Serial communication to send messages to the computer

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
    UpdateAQI(0);
    UpdatePM1(data.PM_AE_UG_1_0);
    UpdatePM2(data.PM_AE_UG_2_5);
    UpdatePM10(data.PM_AE_UG_10_0);
    
    delay(1000);
  }
}

void UpdateDataHeaders()
{
  writeHeader1("AQI:");
  writeHeader2("PM1.0:");
  writeHeader3("PM2.5:");
  writeHeader4("PM10:");
}

void UpdateAQI(uint16_t count)
{
  char buf[6];
  snprintf(buf, 6, "%5d", count);
  writeData1(buf);
}
void UpdatePM1(uint16_t count)
{
  char buf[6];
  snprintf(buf, 6, "%5d", count);
  writeData2(buf);
}

void UpdatePM2(uint16_t count)
{
  char buf[6];
  snprintf(buf, 6, "%5d", count);
  writeData3(buf);
}

void UpdatePM10(uint16_t count)
{
  char buf[6];
  snprintf(buf, 6, "%5d", count);
  writeData4(buf);
}

void writeData1(char *buf)
{
  display.fillRect(49, 0, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(49, 0);
  display.println(buf);
  display.display();
}
void writeData2(char *buf)
{
  display.fillRect(49, 16, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(49, 16);
  display.println(buf);
  display.display();
}
void writeData3(char *buf)
{
  display.fillRect(49, 32, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(49, 32);
  display.println(buf);
  display.display();
}
void writeData4(char *buf)
{
  display.fillRect(49, 48, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(49, 48);
  display.println(buf);
  display.display();
}

void writeHeader1(char *buf)
{
  display.fillRect(0, 0, 128, 16, SSD1306_BLACK);
  display.setTextSize(2); // Draw 2X-scale text
  display.setCursor(0, 0);
  display.println(buf);
  display.display();
}
void writeHeader2(char *buf)
{
  display.fillRect(0, 16, 128, 16, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.println(buf);
  display.display();
}
void writeHeader3(char *buf)
{
  display.fillRect(0, 32, 128, 16, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 36);
  display.println(buf);
  display.display();
}
void writeHeader4(char *buf)
{
  display.fillRect(0, 48, 128, 16, SSD1306_BLACK);
  display.setTextSize(1);
  display.setCursor(0, 52);
  display.println(buf);
  display.display();
}
