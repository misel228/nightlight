/*
* Arduino Nano with Atmega 328 (use old boot loader)
*/

#include <FastLED.h>
#define NUM_LEDS 8
#define DATA_PIN 7
#define CLOCK_PIN 5
#define PIR_PIN 12
#define BUZZER_PIN 2
#define LED_PIN 13
#define PHOTO_PIN A7

#define PHOTO_THRESHOLD 150
#define BUFFER_LENGTH 4

int pir = 0;
int ldr = 0;
int white_bright = 255;  // default brightness for white (0-255)
int buzz_disable = 0;    // change to 1 to switch off initial buzzer
int incomingByte = 0;    // for incoming serial data
int pir_was_read = false;

CRGBArray<NUM_LEDS> leds;

typedef struct
{
  int r;
  int g;
  int b;
} Color;

Color colors[NUM_LEDS];
int current_color = 0;

uint32_t nextTime;
uint32_t lastTime;

void setup() {
  Serial.begin(9600);

  Serial.println("Basic colors");
  colors[0].r = 1;
  colors[0].g = 0;
  colors[0].b = 0;

  colors[1].r = 1;
  colors[1].g = 0;
  colors[1].b = 0;

  colors[2].r = 1;
  colors[2].g = 0;
  colors[2].b = 0;

  colors[3].r = 1;
  colors[3].g = 0;
  colors[3].b = 0;

  colors[4].r = 1;
  colors[4].g = 0;
  colors[4].b = 0;

  colors[5].r = 1;
  colors[5].g = 0;
  colors[5].b = 0;

  colors[6].r = 1;
  colors[6].g = 0;
  colors[6].b = 0;

  colors[7].r = 1;
  colors[7].g = 0;
  colors[7].b = 0;


  Serial.println("Init FastLEDs");
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
  FastLED.setBrightness(white_bright);  // Global Brightness setting max 255

  Serial.println("Init Ports");
  pinMode(PIR_PIN, INPUT);      // PIR motion sensor read port
  pinMode(BUZZER_PIN, OUTPUT);  // buzzer port
  pinMode(LED_PIN, OUTPUT);     // LED port

  Serial.println("Buzz (hold your ears)");
  if (buzz_disable == 0) {
    beep(10);
  }

  digitalWrite(LED_PIN, HIGH);  // PIR LED ON
  delay(100);
  digitalWrite(LED_PIN, LOW);  // PIR LED OFF
}


void loop() {
  if (Serial.available() > 0) {
    int color = read_integer_from_serial();
    colors[current_color].r = color;

    color = read_integer_from_serial();
    colors[current_color].g = color;

    color = read_integer_from_serial();
    colors[current_color].b = color;
    if (++current_color > 7) {
      current_color = 0;
    }
  }

  FastLED.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(colors[i].r, colors[i].g, colors[i].b);
  }

  // read the PIR only every half a second
  // unfortunately, Arduino Nano supports Interrupts only on Pin 1 and 2, but the PIR is connected to Pin12
  uint32_t currTime = millis();
  if (currTime > nextTime) {
    nextTime += 500;
    uint32_t deltaTime = currTime - lastTime;
    lastTime = currTime;
    pir = digitalRead(PIR_PIN);  // check PIR
    pir_was_read = true;

    ldr = analogRead(PHOTO_PIN);  // read photo resistance
    Serial.print("Brightness: ");
    Serial.println(ldr);
  }

  if (pir_was_read && pir == HIGH) {  // If motion detected
    Serial.println("Motion detected");
    digitalWrite(LED_PIN, HIGH);  // PIR LED ON
    delay(100);                   // Delay so when dark the LED will flash on for 0.1s
  }
  digitalWrite(LED_PIN, LOW);  // PIR LED OFF

  if (pir_was_read && pir == HIGH && ldr <= PHOTO_THRESHOLD) {  // if someone there AND dark(ish) with case
    flash_white();
  }
  pir_was_read = false;
  FastLED.show();
  delay(50);
}

void flash_white() {
  Serial.println("... and it's dark");
  digitalWrite(LED_PIN, LOW);  // Don't need PIR LED on now
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].setRGB(white_bright, white_bright, white_bright);
  }
  FastLED.show();
  Serial.println("Show LEDs for 18 seconds");
  delay(18000);  // lights on for about 18 seconds

  for (int j = white_bright; j > -1; j--) {
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i].setRGB(j, j, j);  // fading out the white over ~2s
    }
    FastLED.show();
    delay(10);  // wait 10ms
  }

  FastLED.clear();  // reset LEDs to 0,0,0
  FastLED.show();   // lights off
}

void beep(int milliseconds) {
  digitalWrite(BUZZER_PIN, HIGH);  // buzzer ON, if not disabled
  delay(milliseconds);
  digitalWrite(BUZZER_PIN, LOW);  // buzzer OFF
}

int read_integer_from_serial() {

  char read_buffer[BUFFER_LENGTH];  // 123,
  int buffer_pointer = 0;
  bool continue_reading = true;

  for (int i = buffer_pointer; i < BUFFER_LENGTH; i++) {
    read_buffer[i] = 0;
  }


  while (continue_reading) {
    incomingByte = Serial.read();
    switch (incomingByte) {
      case 48:  // '0'
      case 49:  // '1'
      case 50:  // '2'
      case 51:  // '3'
      case 52:  // '4'
      case 53:  // '5'
      case 54:  // '6'
      case 55:  // '7'
      case 56:  // '8'
      case 57:  // '9'
        read_buffer[buffer_pointer] = incomingByte;
        buffer_pointer++;
        if (buffer_pointer > (BUFFER_LENGTH - 1)) {
          continue_reading = false;
        }
        break;
      default:  // ignore everything
        continue_reading = false;
        break;
    }
  }
  int result = String(read_buffer).toInt();
  if (result > 255) {
    return 255;
  }
  if (result < 0) {
    return 0;
  }
  return result;
}
