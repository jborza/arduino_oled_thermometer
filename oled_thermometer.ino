#include "U8glib.h"
#include "DHT.h"

// setup u8g object, please remove comment from one of the following constructor calls
U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

#define DHTPIN 10     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

#define DEGREE '\xB0'

char buffer_line1[16];
char buffer_line2[40];

float temp_celsius, humidity_percent;
float temp_index_celsius;
bool last_result;
long startupMillis = 0;

boolean read_temperature() {
  humidity_percent = dht.readHumidity();
  temp_celsius = dht.readTemperature();
  if (isnan(humidity_percent) || isnan(temp_celsius)) {
    // Check if any reads failed and exit early (to try again).
    return false;
  }
  const bool isFahrenheit = false;
  temp_index_celsius = dht.computeHeatIndex(temp_celsius , humidity_percent, isFahrenheit);
  return true;
}

void formatLine1() {
  char tempBuf[5];
  //format temperature with one decimal
  dtostrf(temp_celsius, 2, 1, tempBuf);
  sprintf(buffer_line1, "%s %cC", tempBuf, DEGREE);
}

void formatLine2() {
  String humidity = String(humidity_percent);
  String temp_idx = String(temp_index_celsius);
  sprintf(buffer_line2, "RH: %s%% HI: %s ""\xB0""C", humidity.c_str(), temp_idx.c_str());
}

void prepare_display() {
  last_result = read_temperature();
  if (last_result) {
    formatLine1();
    formatLine2();
  }
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
//  u8g.setFont(u8g_font_profont29);
u8g.setFont(u8g_font_helvR18);

  if (!last_result) {
    u8g.drawStr(0, 24, "NO DATA");
    return;
  }

  u8g.drawStr(10, 24, buffer_line1);
  u8g.setFont(u8g_font_4x6);
  u8g.drawStr(10, 32, buffer_line2);
}

void setup(void) {
  // assign default color value
  u8g.setColorIndex(1);

  //set up DHT and give it some time to stabilize
  dht.begin();
  delay(1000);
}

void loop(void) {
  prepare_display();

  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  // rebuild the picture after some delay
  delay(2500);
}
