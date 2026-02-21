// ================== PIN DEFINITIONS ==================
#define TRIG_PIN   4
#define ECHO_PIN   5
#define IR_PIN     2
#define WATER_PIN  A0
#define MOTOR_PIN  9     // PWM
#define BUZZER_PIN 10

// ================== VARIABLES ==================
long duration;
int distance;
int waterValue;
int irState;

// ================== SETUP ==================
void setup() {
  // --- Pin Modes ---
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(IR_PIN, INPUT_PULLUP);   // FIX: Prevent floating
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // --- FORCE SAFE STATE AT BOOT ---
  digitalWrite(MOTOR_PIN, LOW);    // Motor OFF
  noTone(BUZZER_PIN);              // Buzzer OFF

  Serial.begin(9600);
  Serial.println("======================================");
  Serial.println(" SMART BLIND STICK SYSTEM (STABLE) ");
  Serial.println("======================================");
}

// ================== ULTRASONIC FUNCTION ==================
int getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return 200;             // No object

  return duration * 0.034 / 2;
}

// ================== LOOP ==================
void loop() {
  // --- Read Sensors ---
  distance = getDistance();
  waterValue = analogRead(WATER_PIN);
  irState = digitalRead(IR_PIN);

  // --- SERIAL MONITOR OUTPUT ---
  Serial.println("--------------------------------------");
  Serial.print("Ultrasonic Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.print("Water Sensor Value: ");
  Serial.println(waterValue);

  Serial.print("IR Sensor State: ");
  Serial.println(irState == LOW ? "OBJECT DETECTED" : "CLEAR");

  // ================== PRIORITY LOGIC ==================

  // WATER DETECTION (HIGHEST PRIORITY)
  if (waterValue > 100) {
    Serial.println("STATUS: WATER DETECTED !!!");
    Serial.println("ACTION: CONSTANT VIBRATION + BUZZER");

    analogWrite(MOTOR_PIN, 255);
    tone(BUZZER_PIN, 2500);
    delay(100);
    return;
  }

  // IR SENSOR (VERY CLOSE OBJECT)
  if (irState == LOW) {
    Serial.println("STATUS: VERY CLOSE OBJECT (IR)");
    Serial.println("ACTION: STRONG VIBRATION + BUZZER");

    analogWrite(MOTOR_PIN, 255);
    tone(BUZZER_PIN, 2200);
    delay(100);
    return;
  }

  // ULTRASONIC DISTANCE BASED ALERTS
  if (distance > 120) {
    Serial.println("ZONE: SAFE ZONE");
    Serial.println("VIBRATION: OFF | BUZZER: OFF");

    analogWrite(MOTOR_PIN, 0);
    noTone(BUZZER_PIN);
  }

  else if (distance > 70 && distance <= 120) {
    Serial.println("ZONE: FAR OBJECT");
    Serial.println("VIBRATION: LOW | BUZZER: SLOW");

    analogWrite(MOTOR_PIN, 120);
    tone(BUZZER_PIN, 1000);
    delay(200);
    noTone(BUZZER_PIN);
    delay(400);
  }

  else if (distance > 30 && distance <= 70) {
    Serial.println("ZONE: MEDIUM DISTANCE");
    Serial.println("VIBRATION: MEDIUM | BUZZER: MEDIUM");

    analogWrite(MOTOR_PIN, 170);
    tone(BUZZER_PIN, 1500);
    delay(200);
    noTone(BUZZER_PIN);
    delay(200);
  }

  else { // distance <= 30
    Serial.println("ZONE: VERY CLOSE OBJECT (ULTRASONIC)");
    Serial.println("VIBRATION: STRONG | BUZZER: CONTINUOUS");

    analogWrite(MOTOR_PIN, 255);
    tone(BUZZER_PIN, 2200);
  }

  delay(100);
}
