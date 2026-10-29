#include <Adafruit_NeoPixel.h>
#define BUTTON 3
#define LED 2
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);
TaskHandle_t TaskHandle_buttonRead;
TaskHandle_t TaskHandle_Led;

int buttonLastState = HIGH;
int buttonCurrentState;
// Shared between the two tasks, so always read it fresh from memory.
volatile int buttonCount = 1;

int high = 255;
int mid = 128;
int low = 0;

void buttonRead(void *pvParameters)
{
  while (true)
  {
    buttonCurrentState = digitalRead(BUTTON);
    if (buttonLastState == LOW && buttonCurrentState == HIGH)
    {
      Serial.println("Button Pressed!");
      // Cycle through colours 1 to 6.
      buttonCount = buttonCount % 6 + 1;
    }
    buttonLastState = buttonCurrentState;
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void changeLedState(void *pvParameters)
{
  while (true)
  {
    pixels.clear();
    switch (buttonCount)
    {
    case 1:
      pixels.setPixelColor(0, pixels.Color(high, low, low));
      break;
    case 2:
      pixels.setPixelColor(0, pixels.Color(high, mid, mid));
      break;
    case 3:
      pixels.setPixelColor(0, pixels.Color(low, high, low));
      break;
    case 4:
      pixels.setPixelColor(0, pixels.Color(mid, high, mid));
      break;
    case 5:
      pixels.setPixelColor(0, pixels.Color(low, low, high));
      break;
    case 6:
      pixels.setPixelColor(0, pixels.Color(mid, mid, high));
      break;
    default:
      break;
    }
    pixels.show();
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void setup()
{
  Serial.begin(115200);
  pixels.begin();
  pinMode(BUTTON, INPUT_PULLUP);

  // FreeRTOS priorities on the ESP32 go up to 24. Both tasks sit above the
  // Arduino loop task (priority 1).
  xTaskCreate(buttonRead, "buttonRead", 2048, nullptr, 2, &TaskHandle_buttonRead);
  xTaskCreate(changeLedState, "changeLedState", 2048, nullptr, 2, &TaskHandle_Led);
}

void loop()
{
  // All the work happens in the tasks above, so the loop task isn't needed.
  vTaskDelete(nullptr);
}