// FAN COUNTER - count the speed (in Hz) of a computer fan
// Jason Friedman, August 2025
// write.to.jason@gmail.com

// Wiring - the fan should be connected to an external power supply
// From the fan - do not connect the positive (+) wire (only connect to the fan)
// the ground (-) should be connected to a ground on the device
// The third wire should be connected to pin 26
// If there are 4 wires, you can ignore the fourth one (it is for using PWM to control the fan speed)

// If you are using a DS18B20 temperature sensor - uncomment #define MEASURETEMPERATURE on line 21
// Connect the red wire to 5V
// Connect the black wire to Ground
// Connect the yellow wire to pin 27
// Connect a resistor (4.7kOhm) from pin 27 to 5V

#include "driver/pcnt.h"

#define PCNT_UNIT PCNT_UNIT_0 // Use PCNT unit 0
#define PULSE_INPUT_PIN 26    // GPIO pin for pulse input
#define PULSES_PER_REVOLUTION 2

//#define MEASURETEMPERATURE
//#define DEBUGGING
#ifdef MEASURETEMPERATURE
const int temperatureSensorPin = 27; //15;
#include <OneWire.h>
#include <DallasTemperature.h>
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(temperatureSensorPin);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);
#endif

#include <Chrono.h>         // new version of Metro
#include <BleKeyboard.h>  // BLE keyboard library

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// pinouts from https://github.com/Xinyuan-LilyGO/TTGO-T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

BleKeyboard bleKeyboard("CHANGEOFENERGY", "LilyGO", 100);

int16_t long lastCount = 0UL;
unsigned long lastTime = 0UL;

#define BUTTON_A_PIN 35
#define BUTTON_DEBOUNCE_MS 200

unsigned long lastButtonPressTime = 0;
bool lastButtonState = HIGH;

Chrono timer;

int getHz() {
  int16_t count;
  esp_err_t err_msg = pcnt_get_counter_value(PCNT_UNIT, &count);
  if (err_msg == ESP_OK) {
    unsigned long currentTime = millis();
    unsigned long deltaCount = count - lastCount;
    unsigned long deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    lastCount = count;
    int speedHz = 1000.0 * (float)deltaCount / (float)deltaTime / PULSES_PER_REVOLUTION;
    //int speedRPM = (float) (1000.0 * 60.0) * deltaCount / (float)deltaTime / PULSES_PER_REVOLUTION;

    #ifdef DEBUGGING
    Serial.print("delta Time: ");
    Serial.println(deltaTime);
    Serial.print("delta count: ");
    Serial.println(deltaCount);
    Serial.print("Current Hz: ");
    Serial.println(speedHz);
    #endif

    return speedHz;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_A_PIN, INPUT);

  #ifdef MEASURETEMPERATURE
  // Start up the library for the one wire (temperature sensor)
  sensors.begin();
  #endif

  // Configure PCNT unit
  pcnt_config_t pcnt_config = {
    .pulse_gpio_num = PULSE_INPUT_PIN,
    .ctrl_gpio_num = -1, // No control signal for simple pulse counting
    .lctrl_mode = PCNT_MODE_KEEP,
    .hctrl_mode = PCNT_MODE_KEEP,
    .pos_mode = PCNT_COUNT_INC, // Increment on positive edge
    .neg_mode = PCNT_COUNT_DIS, // Do not count on negative edge
    .counter_h_lim = 32767,     // Max count for 16-bit signed counter
    .counter_l_lim = -32768,    // Min count for 16-bit signed counter
    .unit = PCNT_UNIT,
    .channel = PCNT_CHANNEL_0,
  };

  // Initialize PCNT unit
  pcnt_unit_config(&pcnt_config);

  // Set filter value (for debouncing) - adjust as needed
  pcnt_set_filter_value(PCNT_UNIT, 100); // 100 clock cycles filter
  pcnt_filter_enable(PCNT_UNIT);

  // Clear and start the counter
  pcnt_counter_clear(PCNT_UNIT);
  pcnt_counter_resume(PCNT_UNIT);

  // Start display
  pinMode(TFT_BL, OUTPUT);      // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);   // T-Display turn on Backlight
  tft.init(135, 240);           // Initialize ST7789 240x135
  Serial.println(F("Initialized screen"));
  tft.fillScreen(ST77XX_BLACK);
  tft.setRotation(3);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);

  bleKeyboard.begin();
}

void loop() {
  bool buttonState = digitalRead(BUTTON_A_PIN);
  unsigned long now = millis();

  if (buttonState == LOW && lastButtonState == HIGH && (now - lastButtonPressTime > BUTTON_DEBOUNCE_MS)) {
    // Button A was just pressed
    lastButtonPressTime = now;
    if (bleKeyboard.isConnected()) {
      int rpm = (int)getHz();
      String msg = String(rpm) + " Hz\n";
      bleKeyboard.print(msg);  // Types the RPM and Enter on the host
    }
  }
  lastButtonState = buttonState;
  
   if (timer.hasPassed(1000)) {
    int hz = getHz();
    char str[16];
    tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
    if (hz>9999) // avoid overflow
      hz = 0;
    sprintf(str,"%4d Hz",(int)hz);

    #ifdef MEASURETEMPERATURE
    tft.setCursor(30,20);
    tft.print(str);

    sensors.requestTemperatures(); 
    float temperature = sensors.getTempCByIndex(0);
    tft.setCursor(30,80);
    sprintf(str,"%3.2f%cC",temperature,(char)247,temperature);
    tft.print(str);
    #else
    tft.setCursor(30,50);
    tft.print(str);
    #endif

    timer.restart();
   }
}
