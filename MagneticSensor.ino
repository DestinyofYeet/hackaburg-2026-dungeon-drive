#include "TLx493D_inc.hpp"
#include <math.h> // Needed for calculating vector magnitudes

// Based on "read_iic_sensor_c_style" example since the higher level one did not work with our setup

const uint8_t POWER_PIN = 15; // XMC1100 : LED2 power pin
TLx493D_t dut;
TLx493D_SupportedSensorType_t sensorType = TLx493D_W2B6_e;

const double PRESENCE_THRESHOLD = 2.0; // Minimum mT on Z-axis to consider the magnet "nearby"
const double CENTERED_THRESHOLD = 1.5; // Maximum mT on X/Y axes to consider it "centered"

void setup() {
    Serial.begin(9600);
    delay(3000); // Wait for Serial to connect safely

    Serial.println("Starting up Magnet Tracker...");

    // Hard power reset cycle
    pinMode(POWER_PIN, OUTPUT);
    digitalWrite(POWER_PIN, LOW);
    delayMicroseconds(250000UL); // 250ms off
    digitalWrite(POWER_PIN, HIGH);
    delay(50); // Give it a moment to stabilize

    // Initialize sensor structure
    if (!tlx493d_init(&dut, sensorType)) {
        Serial.println("Failed to initialize sensor structure!");
    }

    // Initialize I2C communication
    if (!ifx::tlx493d::initCommunication(&dut, Wire, TLx493D_IIC_ADDR_A0_e, true)) {
        Serial.println("Failed to initialize I2C!");
    }

    // Set default config (1-byte read mode)
    if (!tlx493d_setDefaultConfig(&dut)) {
        Serial.println("Failed to set default config!");
    }

    Serial.println("Setup done. Ready to track.\n---------------------------");
}

void loop() {
    double x, y, z, temp;

    // 1. Fetch the latest magnetic data
    tlx493d_getMagneticFieldAndTemperature(&dut, &x, &y, &z, &temp);

    // 2. Determine if the magnet is nearby using the Z-axis (up/down)
    // We use abs() because the magnet could be facing North or South down
    if (abs(z) > PRESENCE_THRESHOLD) {
        
        Serial.print("Magnet Detected! (Strength: ");
        Serial.print(abs(z));
        Serial.print(" mT) ");
        Serial.print("");
        Serial.print("x:");
        Serial.print(x);
        Serial.print(" ");
        Serial.print("y:");
        Serial.print(y);
        Serial.print(" ");
        Serial.print("z:");
        Serial.print(z);
        Serial.print(" ");
        Serial.print(" --> ");

        // 3. Calculate how far off-center we are in the 2D plane
        // We use the Pythagorean theorem (a^2 + b^2 = c^2) to find total X/Y drift
        double xy_drift = sqrt((x * x) + (y * y));

        if (xy_drift <= CENTERED_THRESHOLD) {
            Serial.println("PERFECTLY CENTERED! [X]");
        } else {
            // Provide directions to center the sensor
            Serial.print("Move Sensor: ");
            
            if (x > CENTERED_THRESHOLD) Serial.print("LEFT ");
            else if (x < -CENTERED_THRESHOLD) Serial.print("RIGHT ");
            
            if (y > CENTERED_THRESHOLD) Serial.print("FORWARD ");
            else if (y < -CENTERED_THRESHOLD) Serial.print("BACKWARD ");
            
            Serial.println();
        }

    } else {
        Serial.println("Waiting for magnet...");
    }


    // Wait slightly before taking the next reading so the serial monitor is readable
    delay(1000); 
}
