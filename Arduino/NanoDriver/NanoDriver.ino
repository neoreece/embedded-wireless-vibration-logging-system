/**
 * NanoDriver for Vibration Logging Project.
 * EE433 Intro to Embedded Systems
 *
 * The NanoDriver provides the functionality to the Arduino Nano vibration
 * data collection system. This establishes a software serial connection
 * with an HC-05 bluetooth module on pins 2 (rx) and 3 (tx). An MPU6050
 * is the intended accelerometer for this application and is accessed using
 * the I2C interface of the arduino nano. Due to the accessing of the MPU6050's
 * specific addresses, this program will be incompatible with other
 * accelerometers.
 *
 * | ============================================================= |
 * | Wiring Diagram                                                |
 * | ------------------------------------------------------------- |
 * | Arduino Nano | HC-05 Bluetooth Module | MPU6050 Accelerometer |
 * | ------------ | ---------------------- | --------------------- |
 * | D3 (TX)      | TX                     |                       |
 * | ------------ | ---------------------- | --------------------- |
 * | D2 (RX)      | RX                     |                       |
 * | ------------ | ---------------------- | --------------------- |
 * | 5V           | VCC                    |                       |
 * | ------------ | ---------------------- | --------------------- |
 * | A4 (SDA)     |                        | SDA                   |
 * | ------------ | ---------------------- | --------------------- |
 * | A5 (SCL)     |                        | SCL                   |
 * | ------------ | ---------------------- | --------------------- |
 * | 3V3          |                        | VCC                   |
 * | ------------ | ---------------------- | --------------------- |
 * | GND          | GND                    | GND                   |
 * | ============================================================= |
 *
 * @author Reece Younger
 * @since 2019 10 30
 */

#include <SoftwareSerial.h>
#include <Wire.h>

/** Allow serial communication with HC-05 bluetooth device. */
#define BLUETOOTH_RX_PIN 2        // Arduino Nano pin used for bluetooth reception
#define BLUETOOTH_TX_PIN 3        // Arduino Nano pin used for bluetooth transmission
#define BLUETOOTH_BAUD_RATE 9600  // Serial communication rate
SoftwareSerial BluetoothModule(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);

/**
 * MPU6050 relevant register addresses for reading accelerometer data, power
 * configuration, and accessing the MPU6050.
 */
/* MPU6050 Relevant Register Map
 * | ------------------------------------------------------------------------ |                     
 * | Power Management 1 :: PWR_MGMT_1 --------------------------------------- |                     
 * | ------------------------------------------------------------------------ |                     
 * | Register | Bit 7        | Bit 6 | Bit 5 | Bit 4 |   Bit 3  |  Bit[2:0]   |
 * | ------------------------------------------------------------------------ |
 * | 0x6B     | DEVICE_RESET | SLEEP | CYCLE |   -   | TEMP_DIS | CLKSEL[2:0] | 
 * | ------------------------------------------------------------------------ |
 */

// MPU6050 accelerometer slave address
#define MPU6050                       0x68   
#define PWR_MGMT_1   ((unsigned char) 0x6B)  
#define WAKE_UP      ((unsigned char) 0x00)
// Sends a stop signal to release I2C bus 
#define STOP                          true  
// Sends a repeated start signal to hold I2C bus
#define REPEATED_START                false  

#define ACCEL_XOUT_H ((unsigned char) 0x3B)
#define ACCEL_XOUT_L ((unsigned char) 0x3C)
#define ACCEL_YOUT_H ((unsigned char) 0x3D)
#define ACCEL_YOUT_L ((unsigned char) 0x3E)
#define ACCEL_ZOUT_H ((unsigned char) 0x3F)
#define ACCEL_ZOUT_L ((unsigned char) 0x40)


char payload_data[8] = {'S', 'T', 'R', 'R', 'R', 'R', 'R', 'R'};
#define PAYLOAD_SIZE 8

/**
 * Establishes serial comminiation with bluetooth module and wakes up the
 * MPU6050 accelerometer.
 */
void setup() {
    initializeBluetoothModule();
    initializeAccelerometer();
    /* DEBUG */ Serial.begin(9600);
}

/**
 * Read the accelerometer X,Y,and Z values. Transmit these values via the
 * bluetooth module.
 */
void loop() {
    readAccelerometer();          // Read accelerometer X, Y, Z registers
    transmitAccelerometerData();  // Transmit data via bluetooth
    delay(100);
}

/**
 * Wake up the MPU6050 accelerometer and have it operate on its internal clock.
 */
void initializeAccelerometer() {
    Wire.begin();
    Wire.beginTransmission(MPU6050); // Prepare to send data to MPU6050
    Wire.write(PWR_MGMT_1);          // Access the power management register
    Wire.write(WAKE_UP);             // Wake up the set the internal clock
    Wire.endTransmission();          // Send the bytes to MPU6050
}

/**
 * Reads the accelerometer's X, Y, and Z registers and stores them in acceleromterData[]
 */
void readAccelerometer() {
    Wire.beginTransmission(MPU6050);     // Establish connection with MPU6050
    Wire.write(ACCEL_XOUT_H);            // Request bytes startinf from this register.
    Wire.endTransmission(REPEATED_START);// Send request
    Wire.requestFrom(MPU6050, 6);

    payload_data[2] = Wire.read();
    payload_data[3] = Wire.read();
    payload_data[4] = Wire.read();
    payload_data[5] = Wire.read();
    payload_data[6] = Wire.read();
    payload_data[7] = Wire.read();
}

/**
 * Sets the recieve pin to input mode, sets the transmit pin to output mode
 * Starts serial communication at the baud rate specified by BLUETOOTH_BAUD_RATE
 */
void initializeBluetoothModule() {
    pinMode(BLUETOOTH_RX_PIN, INPUT);                 // Configure recieving pin to accept input
    pinMode(BLUETOOTH_TX_PIN, OUTPUT);                // Configure transmission pin to accept output
    BluetoothModule.begin(BLUETOOTH_BAUD_RATE);  // Establish serial communication at 9600 baud
}

/**
 * Sends the X, Y, and Z acclerometer data via bluetooth serial connection.
 */
void transmitAccelerometerData() {
    BluetoothModule.write(payload_data, PAYLOAD_SIZE);  // Send the accelerometerData array, whose size is 6 bytes
    int timeout = 200;
    while (BluetoothModule.read() != 'O' && BluetoothModule.read() != 'P') {
        timeout--;
        Serial.println(timeout);
        if (timeout <= 0) {
            break;
        }
    }
}
