// header format: <> - search in library  "" - search in project folder

#include <Arduino.h> // basic arduino actions
#include <lvgl.h> // for creating ui to put on the screen
#include "lvgl_v8_port.h" // connector between esp32 and the lcd
#include <esp_display_panel.hpp> // gives access to the board and lcd
#include <Wire.h> // for mpu

#include "assets/coolio.c"
#include "assets/depressed.c"
#include "assets/gentleman.c"
#include "assets/sad.c"

// MPU pins and address
#define SDA_PIN 8
#define SCL_PIN 9
#define MPU_ADDRESS 0x68


// gyroscope offset tuning
const int16_t GYRO_OFFSET_X = 1540;
const int16_t GYRO_OFFSET_Y = -58;
const int16_t GYRO_OFFSET_Z = 46;

using namespace esp_panel::drivers;
using namespace esp_panel::board;


lv_indev_t *touchInput; // for reading the touch screen device

LV_IMG_DECLARE(coolio);
LV_IMG_DECLARE(depressed);
LV_IMG_DECLARE(gentleman);

lv_obj_t *image; // create an obj called image

const lv_img_dsc_t *pictures[] = { // make an array for the images
    &coolio, 
    &depressed,
    &gentleman
};

int curPicture = 0;
int lastChange = 0;

void setup() {

    Serial.begin(115200); // start monitor at 112500 baud
    delay(1000);

    // ********************* BOARD CREATION ****************************

    Serial.println("Starting LVGL Text Test");

    Board *board = new Board(); // declare the display board
    Serial.println("Board object created");

    if (!board->init()) { // if we can't init, return
        Serial.println("Board init failed!");
        return;
    }

    Serial.println("Board Initialized!");

    // get lcd and touch objects
    LCD *lcd = board->getLCD();
    Touch *touch = board->getTouch();

    Serial.println("LCD and Touch objects obtained");

    // lcd null check
    if (lcd == nullptr) {
        Serial.println("LCD IS NULL!");
        return;
    }
    else {
        Serial.println("LCD EXISTS AFTER INIT!");
    }

    bool beginResult = board->begin(); // begin the board

    Serial.print("Board begin result = ");
    Serial.println(beginResult);

    if (!beginResult) { // exit if the board didn't begin
        Serial.println("BOARD BEGIN FAILED!");
        return;
    }

    Serial.println("Board begun successfully");

    lvgl_port_init(lcd, touch); // register hardware to the lcd and touch objects
    Serial.println("LVGL port initialized");

    lvgl_port_lock(-1); // lock the lvgl port * prevents two pieces of code from editing the lvgl at the same time
    Serial.println("LVGL locked");

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFF0000), LV_PART_MAIN); // red test
    Serial.println("Background color set");

    lvgl_port_unlock();

    touchInput = lv_indev_get_next(NULL); // set the touchInput to the registered touch device

    // ******************** MPU SETUP *************************************

    Wire.begin(SDA_PIN, SCL_PIN); // start mpu i2c

    // wake up the MPU
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x6B);
    Wire.write(0x00);
    Wire.endTransmission();

    // ********************* DRAW AN IMAGE ********************************

    Serial.println("Creating Image");

    image = lv_img_create(lv_scr_act()); // set image's definition 

    Serial.println("Define Image");

    lv_img_set_src(image, &coolio); // set the image to the contents of coolio's adress

    Serial.println("Set Image to start");

    lv_obj_center(image); // center the image

    Serial.println("Centered Image");

    lastChange = millis(); // reset lastChange
}

void loop() {
    
    // request mpu data
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDRESS, 14, true); // request 14 bytes of data (acl, skip temp, gyro)

    // read 8 bytes from acl (1 bit at a time)
    int16_t aclX = Wire.read() << 8 | Wire.read();
    int16_t aclY = Wire.read() << 8 | Wire.read();
    int16_t aclZ = Wire.read() << 8 | Wire.read();

    // skip temp
    Wire.read();
    Wire.read();

    // read the 6 bytes from gyro
    int16_t gyroX = Wire.read() << 8 | Wire.read();
    int16_t gyroY = Wire.read() << 8 | Wire.read();
    int16_t gyroZ = Wire.read() << 8 | Wire.read();

    Serial.print("Accel values: ");
    Serial.print(aclX);
    Serial.print(", ");
    Serial.print(aclY);
    Serial.print(", ");
    Serial.println(aclY);

    Serial.print("Gyro values: ");
    Serial.print(gyroX - GYRO_OFFSET_X);
    Serial.print(", ");
    Serial.print(gyroY - GYRO_OFFSET_Y);
    Serial.print(", ");
    Serial.println(gyroZ - GYRO_OFFSET_Z);
}