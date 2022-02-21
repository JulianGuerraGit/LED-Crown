//#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


#define PIN 5

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "*****";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "*****";
char passw[] = "*****";

String DisplayText = "Senior          2020          Julian";
String oldDisplayText = "Senior          2020          Julian";
int r = 0;
int g = 0;
int b = 255;
int Brightness = 15;

int Mode = 0;

int ReCnctFlag;  // Reconnection Flag
int ReCnctCount = 0;

BlynkTimer timer;

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP    + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = { matrix.Color(r, g, b) };

BLYNK_WRITE(V0)
{
  switch (param.asInt()) {
    case 1: {
      Mode = 0;
      break;
    }
    case 2: {
      Mode = 1;
      break;
    }
    case 3: {
      Mode = 2;
      break;
    }
    case 4: {
      Mode = 3;
      break;
    }
  }
}

BLYNK_WRITE(V1)
{
 DisplayText = param.asStr();
}

BLYNK_WRITE(V2)
{
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();
}

BLYNK_WRITE(V3)
{
  Brightness = param.asInt();
}

int x    = matrix.width();
int pass = 0;

void Text() {
  if (DisplayText != oldDisplayText) {
    //Change text
    oldDisplayText = DisplayText;
    Serial.print(DisplayText);
  }
  
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  oldDisplayText.toUpperCase();
  matrix.print(oldDisplayText);
  int TextLen = oldDisplayText.length();
  int len = 15 + 6 * TextLen;
  if(--x < -len) {
    x = matrix.width();
  }
  delay(30);
}
int i = 0;
void PacMan () {
  
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  delay(60);
  // Pacman body and open mouth
  matrix.fillCircle(20,4, 3, matrix.Color(255,255,0)); // Body
  matrix.fillTriangle(20,4,17,1,17,7,matrix.Color(0,0,0)); // Mouth open
  
  // Ghost body, legs, and eyes
  matrix.fillCircle(28,3, 3, matrix.Color(r,g,b)); // Body
  
  matrix.fillRect(26,2,2,2,matrix.Color(255,255,255)); // Eyes
  matrix.fillRect(29,2,2,2,matrix.Color(255,255,255));
  matrix.drawLine(26,3,26,3,matrix.Color(0,0,200));
  matrix.drawLine(29,3,29,3,matrix.Color(0,0,200));
  
  matrix.drawLine(25,7,25,5,matrix.Color(r,g,b)); // Legs
  matrix.drawLine(31,7,31,5,matrix.Color(r,g,b));
  matrix.drawLine(28,7,28,7,matrix.Color(r,g,b));
  matrix.drawLine(26,6,30,6,matrix.Color(r,g,b));
  
  if (++i > 4) {
    i = 0;
  }
 
  if (i % 2 == 0) {
    matrix.drawLine(26,6,26,6,matrix.Color(0,0,0));
    matrix.drawLine(29,6,29,6,matrix.Color(0,0,0));
  }
  else if (i & 1 == 1) {
    matrix.drawLine(27,6,27,6,matrix.Color(0,0,0));
    matrix.drawLine(30,6,30,6,matrix.Color(0,0,0));
  }
  
  if (i == 2){
    matrix.fillCircle(20,4, 3, matrix.Color(255,255,0));
    matrix.fillTriangle(19,4,17,2,17,6,matrix.Color(0,0,0));
  }
  if (i == 3){
    matrix.fillCircle(20,4, 3, matrix.Color(255,255,0));
  }  
  if (i == 4){
    matrix.fillCircle(20,4, 3, matrix.Color(255,255,0));
    matrix.fillTriangle(19,4,17,2,17,6,matrix.Color(0,0,0));
  }  
  
  if (i < 3) {
    matrix.fillCircle(16 + i ,4,0, matrix.Color(255,255,255));
  }
  matrix.fillCircle(12 + i ,4,0, matrix.Color(255,255,255));
  matrix.fillCircle(8 + i ,4,0, matrix.Color(255,255,255));
  matrix.fillCircle(4 + i ,4,0, matrix.Color(255,255,255));
  matrix.fillCircle(0 + i ,4,0, matrix.Color(255,255,255));
  
}

BLYNK_CONNECTED() {
  //Blynk.syncAll();
  ReCnctCount = 0;
}

void Reconnect() {
  ReCnctFlag = 1;  // Set reconnection Flag
  Serial.println("Starting reconnection timer in 30 seconds...");
  timer.setTimeout(120000L, []() {  // Lambda Reconnection Timer Function
    ReCnctFlag = 0;  // Reset reconnection Flag
    ReCnctCount++;  // Increment reconnection Counter
    Serial.print("Attempting reconnection #");
    Serial.println(ReCnctCount);
    Blynk.connect();  // Try to reconnect to the server
  });
}

void setup() {
  //Serial.begin(9600);
  Serial.print("Started");
  
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(Brightness);
  matrix.setTextColor(matrix.Color(r, g, b));
  WiFi.begin(ssid, passw);  // Non-blocking if no WiFi available
  Blynk.config(auth);
  Blynk.connect(); 
}

void loop() {
  timer.run();
  if (Blynk.connected()) { 
    Blynk.run();
  } else if (ReCnctFlag == 0) {
    Reconnect();
  }
  
  if (Mode == 0) {
    Text();
  }
  else if (Mode == 1) {
    PacMan();
  }
  else if (Mode == 3) {
    matrix.fillScreen(matrix.Color(0,0,0));
  }
  matrix.setBrightness(Brightness);
  matrix.setTextColor(matrix.Color(r, g, b));
  matrix.show();
  delay(10);
}
