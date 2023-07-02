#include <Arduino.h>
#include <Ultrasonic.h>
#include <Servo.h>
#include <ESP8266_ISR_Servo.h>

#define TRIG_PIN_1 D3
#define ECHO_PIN_1 D4
// #define PIN_SERVO_1 D7
#define TRIG_PIN_2 D5
#define ECHO_PIN_2 D6
// #define PIN_SERVO_2 D8

#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS      800  //544
#define MAX_MICROS      2450

Ultrasonic Ugate1(TRIG_PIN_1, ECHO_PIN_1);
Ultrasonic Ugate2(TRIG_PIN_2, ECHO_PIN_2);

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

#define NUM_SERVOS 2

ISR_servo_t ISR_servo[NUM_SERVOS] =
{
  { -1, D7 }, { -1, D8 }
};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting ESP8266_MultipleServos on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP8266_ISR_SERVO_VERSION);
  
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    ISR_servo[index].servoIndex = ISR_Servo.setupServo(ISR_servo[index].servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo[index].servoIndex != -1)
    {
      Serial.print(F("Setup OK Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
    else
    {
      Serial.print(F("Setup Failed Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
  }
}
void loop()
{
  int position;      // position in degrees

  // Read distance from ultrasonic sensors
  int distance1 = Ugate1.read();
  int distance2 = Ugate2.read();
  Serial.print(distance1);
  Serial.println();
  Serial.print(distance2);
  Serial.println();

  // Control servo motors based on distance readings
  // if (distance1 < 5 || distance2 < 5)
  // {
  //   // Open gate 1
  //   ISR_Servo.setPosition(ISR_servo[0].servoIndex, 0);
  // }
  if (distance1 < 5)
  {
    // Open gate 1
    ISR_Servo.setPosition(ISR_servo[0].servoIndex, 0);
  }
  else
  {
    // Close gate 1
    ISR_Servo.setPosition(ISR_servo[0].servoIndex, 180);
  }

  if (distance2 < 5)
  {
    // Open gate 2
    ISR_Servo.setPosition(ISR_servo[1].servoIndex, 0);
  }
  else
  {
    // Close gate 2
    ISR_Servo.setPosition(ISR_servo[1].servoIndex, 180);
  }

  delay(1000);
}