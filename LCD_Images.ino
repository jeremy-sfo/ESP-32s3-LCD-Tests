// header format: <> - search in library  "" - search in project folder

/*#include <Arduino.h> // basic arduino actions
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

    Wire.begin(SDA_PIN, SCL_PIN); // start mpu i2c

     // ********************* BOARD CREATION ****************************

    esp_panel::board::Board *board = new esp_panel::board::Board(); // declare the display board
    Serial.println("Board Object Created");

    if(!board->init()){
        return;
    } Serial.println("Board Initialized!");

    static_cast<esp_panel::drivers::BusI2C *>( board->getTouch()->getBus() )->configI2C_HostSkipInit();
    
    // check the board begin result
    bool beginResult = board->begin();
    if (!beginResult) { 
        Serial.println("BOARD BEGIN FAILED!");
        return;
    } Serial.println("Board begun successfully");

    // get lcd and touch objects
    LCD *lcd = board->getLCD();
    Touch *touch = board->getTouch();
    Serial.println("LCD and Touch objects obtained");

    // lcd and touch null check
    if (lcd == nullptr) {
        Serial.println("LCD IS NULL!");
        return;
    } else if (touch == nullptr) {
        Serial.println("TOUCH IS NULL!");
    } Serial.println("LCD and Touch initiallized");

    

    lvgl_port_init(lcd, touch); // register hardware to the lcd and touch objects
    Serial.println("LVGL port initialized");

    lvgl_port_lock(-1); // lock the lvgl port * prevents two pieces of code from editing the lvgl at the same time
    Serial.println("LVGL locked");

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xFF0000), LV_PART_MAIN); // red test
    Serial.println("Background color set");

    lvgl_port_unlock();

    touchInput = lv_indev_get_next(NULL); // set the touchInput to the registered touch device

    // ******************** MPU SETUP *************************************

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
    
  /* // request mpu data
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
    Serial.println(aclZ);

    Serial.print("Gyro values: ");
    Serial.print(gyroX - GYRO_OFFSET_X);
    Serial.print(", ");
    Serial.print(gyroY - GYRO_OFFSET_Y);
    Serial.print(", ");
    Serial.println(gyroZ - GYRO_OFFSET_Z);*//*
}*/

#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <driver/i2c.h>

// MPU6050
#define MPU_ADDRESS 0x68

// Display Panel uses I2C host 0
#define I2C_HOST I2C_NUM_0

const int16_t GYRO_OFFSET_X = 1540;
const int16_t GYRO_OFFSET_Y = -58;
const int16_t GYRO_OFFSET_Z = 46;

bool boardReady = false;

using namespace esp_panel::board;


// --------------------------------------------------
// Write one MPU register
// --------------------------------------------------

bool mpuWriteRegister(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};

    esp_err_t result = i2c_master_write_to_device(
        I2C_NUM_0,
        MPU_ADDRESS,
        data,
        sizeof(data),
        pdMS_TO_TICKS(100)
    );

    Serial.printf("MPU write result: %s\n", esp_err_to_name(result));

    return result == ESP_OK;
}


bool mpuReadRegisters(uint8_t reg, uint8_t *data, size_t length)
{
    esp_err_t result = i2c_master_write_read_device(
        I2C_NUM_0,
        MPU_ADDRESS,
        &reg,
        1,
        data,
        length,
        pdMS_TO_TICKS(100)
    );

    Serial.printf("MPU read result: %s\n", esp_err_to_name(result));

    return result == ESP_OK;
}

bool testMPU()
{
    Serial.println("ENTERED testMPU()");
    
    uint8_t reg = 0x75;
    uint8_t value = 0;

    Serial.println("About to perform WHO_AM_I transaction");

    esp_err_t result = i2c_master_write_read_device(
        I2C_NUM_0,
        MPU_ADDRESS,
        &reg,
        1,
        &value,
        1,
        pdMS_TO_TICKS(100)
    );

    Serial.printf("WHO_AM_I result: %s\n", esp_err_to_name(result));

    if (result == ESP_OK) {
        Serial.printf("WHO_AM_I = 0x%02X\n", value);
    }

    return result == ESP_OK;
}

// --------------------------------------------------
// Read MPU accelerometer + gyro
// --------------------------------------------------

bool readMPU()
{
    uint8_t data[14];

    if (!mpuReadRegisters(0x3B, data, 14))
    {
        Serial.println("MPU READ FAILED");
        return false;
    }

    int16_t accelX = (data[0] << 8) | data[1];
    int16_t accelY = (data[2] << 8) | data[3];
    int16_t accelZ = (data[4] << 8) | data[5];

    // data[6] and data[7] = temperature

    int16_t gyroX = (data[8] << 8) | data[9];
    int16_t gyroY = (data[10] << 8) | data[11];
    int16_t gyroZ = (data[12] << 8) | data[13];

    Serial.print("Accel: ");
    Serial.print(accelX);
    Serial.print(", ");
    Serial.print(accelY);
    Serial.print(", ");
    Serial.println(accelZ);

    Serial.print("Gyro: ");
    Serial.print(gyroX - GYRO_OFFSET_X);
    Serial.print(", ");
    Serial.print(gyroY - GYRO_OFFSET_Y);
    Serial.print(", ");
    Serial.println(gyroZ - GYRO_OFFSET_Z);

    return true;
}

bool initLegacyI2C(){
    i2c_config_t config = {};

    config.mode = I2C_MODE_MASTER;
    config.sda_io_num = GPIO_NUM_8;
    config.scl_io_num = GPIO_NUM_9;
    config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    config.master.clk_speed = 400000;

    esp_err_t result = i2c_param_config(I2C_NUM_0, &config);

    if (result != ESP_OK) {
        Serial.printf("I2C PARAM CONFIG FAILED: %s\n", esp_err_to_name(result));
        return false;
    }

    result = i2c_driver_install(
        I2C_NUM_0,
        I2C_MODE_MASTER,
        0,
        0,
        0
    );

    if (result != ESP_OK) {
        Serial.printf("I2C DRIVER INSTALL FAILED: %s\n", esp_err_to_name(result));
        return false;
    }

    Serial.println("Legacy I2C driver installed");
    return true;
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println("MPU6050 Legacy I2C Test");
    Serial.println("================================");

    // ----------------------------------------------
    // Create Display Panel board
    // ----------------------------------------------

    Board *board = new Board();

    Serial.println("Board object created");

    if (!board->init())
    {
        Serial.println("BOARD INIT FAILED");
        return;
    }

    Serial.println("Board initialized");

    // ----------------------------------------------
    // Start Display Panel
    //
    // This should initialize the legacy I2C host.
    // ----------------------------------------------

    if (!board->begin())
    {
        Serial.println("BOARD BEGIN FAILED");
        return;
    }

    Serial.println("Board begun successfully");

    Serial.println("AAAA");
    delay(100);

    Serial.println("BBBB");
    delay(100);

    testMPU();

    Serial.println("CCCC");
    delay(100);

Serial.println("Test finished");

Serial.println("Waking MPU6050...");

    if (!mpuWriteRegister(0x6B, 0x00))
    {
        Serial.println("MPU WAKE FAILED");
        return;
    }

    Serial.println("MPU wake successful");

    // ----------------------------------------------
    // Read MPU
    // ----------------------------------------------

    delay(100);

    readMPU();
}


// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop(){
    if(boardReady){
    readMPU();

    delay(500);}
}