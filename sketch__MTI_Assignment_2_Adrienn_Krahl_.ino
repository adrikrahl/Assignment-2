/* code references: 
1. https://www.jaycar.com.au/gift-guessing-game 
2. https://create.arduino.cc/projecthub/gatoninja236/led-roulette-game- 
85ae3c?ref=tag&ref_id=fun&offset=11 
3. https://www.uio.no/studier/emner/matnat/ifi/IN1060/v21/arduino/arduino-projects- 
book.pdf - Project 11, Crystal Ball 
*/ 
#include <LiquidCrystal.h> 
#include <SparkFun_MMA8452Q.h> 
#include <TTBOUNCE.h> 
#include <Wire.h> 
const uint8_t led_num = 5; 
// <------ Pin declaration ------> 
const uint8_t led_array[led_num] = {6,8, 9, 10,13}; 
const uint8_t tilt_sensor_pin = 7; 
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 
// <------ Destinations declaration ------> 
#define ARRAYSIZE 10 
String message_list[ARRAYSIZE] = {"Havana,Cuba", "Alaska,USA", "Fiji Island", "Antarctica", 
"Morocco,Marrakech", "Cape Town,ZA", "Paris,France", "Edinburgh,Scotland", 
"Dubrovnik,Croatia", "New Orleans,USA"}; 
// <------ Departure times declaration ------> 
#define ARRAYSIZE2 10 
String message2_list[ARRAYSIZE2] = {"Leave now", "Leave tomorrow", "Leave @ midnight", 
"Leave next week", "Leave @ dawn", "Leave @ dusk", "Leave in 5 hours", "Leave @ midday", "Leave tmrw @ 9am", "Leave on Monday"}; 
const int cols = 16; 
const int rows = 2; 
// <------ LED delay settings ------> 
const int led_step_delay = 50; // delay time between each led blink 
const int led_stop_delay_step_base = 5; // move at least 5 time before led light actually stop 
const int led_stop_delay_step = 10; // randomly move 0 ~ 9 steps before led light stop 
// <------ Accelerator settings ------> 
const short shake_threshold = 1050; // setting threshold for accelerator

const uint8_t mag_history_max = 16; // window of averaging function 
int tilt_state = 0; 
int prev_tilt_state = 0; 
short magnitude_history[mag_history_max] = {0}; 
uint8_t mag_idx = 0; 
int raw_mag = 0; 
uint8_t led_idx = 0; 
unsigned long start_time; 
int measure_shake(); 
short get_average_magnitude(int); 
String print_message(); 
String print_message2(); 
void led_step(int); 
void setup() { 
// initializes serial port 
Serial.begin(9600); 
// initializes time counter calculating the relative time and display in log start_time = millis(); 
// initializes communication with I2C/TWI devices (accelerator), for uno devices: A4 (SDA), A5 (SCL) 
Wire.begin(); 
randomSeed(analogRead(0)); // ensuring random message generation as some messages seemed to appear more often than others 
pinMode(tilt_sensor_pin, INPUT); 
lcd.begin(cols, rows); 
lcd.print("Ask your"); 
lcd.setCursor(0, 1); 
lcd.print("Travel Agent !"); 
} 
void loop() { 
delay(300);

short shake = measure_shake(); 
//printing relative time and accelerator value to log 
Serial.println(String(String(time_to_string(millis() - start_time)) + " Accelerator value: " + shake)); 
//------- check shakes ---------------- 
if (shake > shake_threshold) { 

String message = print_message(); 
Serial.println(String(String(time_to_string(millis() - start_time)) + " LCD message: " + message)); 

} 
// reads value from tilt sensor and prints value with relative time 
tilt_state = digitalRead(tilt_sensor_pin); 
Serial.println(String(String(time_to_string(millis() - start_time)) + " Tilt state: " + tilt_state)); 

// move led light a step forward when tilted 
if (tilt_state == HIGH) { 
led_step(0); 
// stop led light moving when tilted to off and include delays for random effect 
} else if (tilt_state == LOW && prev_tilt_state == HIGH) { 
int step = random(led_stop_delay_step) + led_stop_delay_step_base; 
Serial.println(String(String(time_to_string(millis() - start_time)) + " LED index stop in step: " + step)); 
led_step(step); 
Serial.println(String(String(time_to_string(millis() - start_time)) + " LED index: " + led_idx)); 
} 
prev_tilt_state = tilt_state; 
} 
short get_average_magnitude(int mag) { 
magnitude_history[mag_idx++] = mag; 
mag_idx %= mag_history_max; 
short sum = 0;

for (uint8_t i = 0; i < mag_history_max; i++) { 
sum += magnitude_history[i]; 
} 
return sum / mag_history_max; 
} 
int measure_shake() { 
MMA8452Q accel; 
if (!accel.begin()) { 
Serial.println(F("Unable to start the accelerometer, check connections")); 
Serial.println(F("and restart")); 
while (1) 
; 
} 
while (!accel.available()) 
; 
raw_mag = sqrt( 
pow(accel.getX(), 2) + 
pow(accel.getY(), 2) + 
pow(accel.getZ(), 2)); 
short v = get_average_magnitude(raw_mag); 
return v; 
} 
//converting relative time into the format of [hh:MM:SS] 
char* time_to_string(unsigned long t) { 
t /= 1000; 
static char str[12]; 
long h = t / 3600; 
t = t % 3600; 
int m = t / 60; 
int s = t % 60; 
sprintf(str, "%02ld:%02d:%02d", h, m, s); 
return str; 
} 
String print_message() { 
int reply = random(ARRAYSIZE);
int reply_2 = random(ARRAYSIZE2);

lcd.clear(); 
//Message setting for first row first column 
lcd.setCursor(0, 0); 
lcd.print(message2_list[reply_2]);

//Message setting for second row first column 
lcd.setCursor(0, 1); 
lcd.print(message_list[reply]); 

return message2_list[reply_2] + " " + message_list[reply]; 
}


// Moving the LED light forward 
void led_step(int step) { 
for (int i = 0; i <= step; i++) { 
digitalWrite(led_array[led_idx], LOW); 
led_idx += 1; 
led_idx %= led_num; 
digitalWrite(led_array[led_idx], HIGH); 
delay(led_step_delay); 
} 
}
