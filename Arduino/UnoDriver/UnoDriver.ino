/**
 * UnoDriver for Vibration Logging Project.
 * EE433 Intro to Embedded Systems
 *
 * The UnoDriver provides the funcionality of recieving data via bluetoot,
 * formatting it, and writing it to an SD card.
 * 
 * @author Reece Younger
 * @since 2019 10 30
 */
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

/** Allow serial communication with HC-05 bluetooth device. */
#define BLUETOOTH_RX_PIN 2        // Arduino Nano pin used for bluetooth reception
#define BLUETOOTH_TX_PIN 3        // Arduino Nano pin used for bluetooth transmission
#define BLUETOOTH_BAUD_RATE 9600  // Serial communication rate
SoftwareSerial BluetoothModule(BLUETOOTH_RX_PIN, BLUETOOTH_TX_PIN);

const char ACK[2] = {'O', 'P'};
short int acceleration_x;
short int acceleration_y;
short int acceleration_z;

#define PLOT "X: " + ((String)acceleration_x) + " Y: " + ((String)acceleration_y) + " Z: " + ((String)acceleration_z)

/**
 * Establish serial connection with the bluetooth module and SD card.
 */
void setup() {
    initializeBluetoothModule();
    BluetoothModule.listen(); // watch the bluetooth module for any incoming data
    SD.begin(10);             // start SPI communication with SD card
    /* DEBUG */ Serial.begin(9600);
}

/**
 * Read the bluetooth module for incoming data. If it's header contains the
 * characters 'S' followed by 'T', read the next 6 bytes and convert them to
 * 16 bit signed integers.
 */
void loop() {
    if (BluetoothModule.available() > 9 && BluetoothModule.read() == 'S' && BluetoothModule.read() == 'T') {
        acceleration_x = BluetoothModule.read()<<8|BluetoothModule.read();
        acceleration_y = BluetoothModule.read()<<8|BluetoothModule.read();
        acceleration_z = BluetoothModule.read()<<8|BluetoothModule.read();

        Serial.println(PLOT); // Display plot in serial monitor

        File dataLog = SD.open("datalog.txt", FILE_WRITE);
        if (dataLog) {
            dataLog.println(PLOT);
            dataLog.close();
        }

        BluetoothModule.write(ACK, 2); // Acknowledge data was recieved, allow another transmission
    }
}

/**
 * Sets the recieve pin to input mode, sets the transmit pin to output mode
 * Starts serial communication at the baud rate specified by BLUETOOTH_BAUD_RATE
 */
void initializeBluetoothModule() {
    pinMode(BLUETOOTH_RX_PIN, INPUT);              // Configure recieving pin to accept input
    pinMode(BLUETOOTH_TX_PIN, OUTPUT);             // Configure transmission pin to accept output
    BluetoothModule.begin(BLUETOOTH_BAUD_RATE);  // Establish serial communication at 9600 baud
}

