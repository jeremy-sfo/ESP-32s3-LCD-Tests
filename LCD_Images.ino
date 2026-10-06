// header format: <> - search in library  "" - search in project folder
// * means values at that pointer, & means that pointer
// uint8_t is an unsigned 8 bit integer, int16_t is a 16 bit integer

#include <Arduino.h> // basic arduino actions
#include <lvgl.h> // for creating ui to put on the screen
#include "lvgl_v8_port.h" // connector between esp32 and the lcd
#include <esp_display_panel.hpp> // gives access to the board and lcd
#include <driver/i2c.h> // for mpu

#include "assets/coolio.c"
#include "assets/depressed.c"
#include "assets/gentleman.c"
#include "assets/sad.c"

// MPU address
#define MPU_ADDRESS 0x68

// display panel uses I2C host 0
#define I2C_HOST I2C_NUM_0

// gyroscope offset tuning
const int16_t GYRO_OFFSET_X = 1540;
const int16_t GYRO_OFFSET_Y = -58;
const int16_t GYRO_OFFSET_Z = 46;

int16_t yawValue; // current yaw angle
int16_t yawChange; // change in yaw angle
int prevTime = millis(); // time
int deltaTime; // change in time

// angles for test
const float FULL_LEFT_ANGLE   = -45.0;
const float SLIGHT_LEFT_ANGLE = -15.0;
const float SLIGHT_RIGHT_ANGLE = 15.0;
const float FULL_RIGHT_ANGLE  = 45.0;

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

bool mpuWriteRegister(uint8_t reg, uint8_t value){ // register, write values
    
    uint8_t data[2] = {reg, value}; // an array to store write info

    esp_err_t result = i2c_master_write_to_device( // write at I2C_HOST at MPU_ADDRESS for this data that is sizeof(data) large and then wait 100ms
        I2C_HOST,
        MPU_ADDRESS,
        data, 
        sizeof(data), 
        pdMS_TO_TICKS(100)
    );

    //Serial.printf("MPU write result: %s/n", esp_err_to_name(result));

    return result == ESP_OK; // return the result depending on if we were able to write at the reg
}

bool mpuReadRegisters(uint8_t reg, uint8_t *data, size_t length){ // register, address of the data, length of read

    esp_err_t result = i2c_master_write_read_device( // read device with I2C_HOST pin, at MPU_ADDRESS at the register of reg pointer, (1?) read these data pointers that is length long then wait 100ms
        I2C_HOST,
        MPU_ADDRESS,
        &reg,
        1, 
        data, 
        length, 
        pdMS_TO_TICKS(100)
    );

    //Serial.printf("MPU read result: %s/n", esp_err_to_name(result));

    return result == ESP_OK;  
}

bool readMPU(){

    uint8_t data[14]; // 14 bytes of data

    if(!mpuReadRegisters(0x3B, data, 14)){

        Serial.println("MPU READ FAILED");
        return false;
    }

    // read each measurement as high as 8 bits and as low as 8 bits then combine into 16 bit value
    int16_t aclX = (data[0] << 8) | data[1]; 
    int16_t aclY = (data[2] << 8) | data[3];
    int16_t aclZ = (data[4] << 8) | data[5];

    // data[6] and data[7] are temperature

    int16_t gyroX = (data[8] << 8) | data[9];
    int16_t gyroY = (data[10] << 8) | data[11];
    int16_t gyroZ = (data[12] << 8) | data[13];

    gyroX -= GYRO_OFFSET_X;
    gyroY -= GYRO_OFFSET_Y;
    gyroZ -= GYRO_OFFSET_Z;

    Serial.print("Acl: ");
    Serial.print(aclX);
    Serial.print(", ");
    Serial.print(aclY);
    Serial.print(", ");
    Serial.println(aclZ);

    Serial.print("Gyro: ");
    Serial.print(gyroX);
    Serial.print(", ");
    Serial.print(gyroY);
    Serial.print(", ");
    Serial.println(gyroZ);

    deltaTime = millis() - prevTime; // change in time

    yawChange = (gyroZ - gyroOffsetZ) / 131 * deltaTime; //  gyroZ needs to be changed into degrees/second so: (gyroZ - gyroOffsetZ) / 131
    
    yawValue += yawChange; // add the change in angle to current angle

    return true;
}

void updateLCD(){

    if (yawAngle < FULL_LEFT_ANGLE) Serial.println("Full left angle"); 
    
    else if (yawAngle < SLIGHT_LEFT_ANGLE) Serial.println("Slight left angle"); 
    // slight left

    else if (yawAngle < SLIGHT_RIGHT_ANGLE) Serial.println("Slight right angle"); 

    else if (yawAngle < FULL_RIGHT_ANGLE) Serial.println("Full right angle"); 

}

void setup() {

    Serial.begin(115200); // start monitor at 112500 baud
    delay(1000);

     // ********************* BOARD CREATION ****************************

    esp_panel::board::Board *board = new esp_panel::board::Board(); // declare the display board
    Serial.println("Board Object Created");

    if(!board->init()){ Serial.println("BOARD CREATION FAILED"); return; }

    Serial.println("Board Initialized!");

    // do we need this?
   // static_cast<esp_panel::drivers::BusI2C *>( board->getTouch()->getBus() )->configI2C_HostSkipInit();
    
    // check the board begin result
    bool beginResult = board->begin(); 
    if (!beginResult) { Serial.println("BOARD BEGIN FAILED!"); return; } 
    
    Serial.println("Board begun successfully");

    // get lcd and touch objects
    LCD *lcd = board->getLCD();
    Touch *touch = board->getTouch();

    Serial.println("LCD and Touch objects obtained");

    // lcd and touch null check
    if (lcd == nullptr) { Serial.println("LCD IS NULL!"); return; } 
    else if (touch == nullptr) { Serial.println("TOUCH IS NULL!"); } 
    
    Serial.println("LCD and Touch initiallized");

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
    Serial.println("Waking up MPU...");

    if(!mpuWriteRegister(0x6B, 0x00)){ Serial.println("MPU OVERSLEPT AND DIDN'T WAKE UP!"); return; }

    Serial.println("MPU woke up!");

    // ********************* DRAW AN IMAGE ********************************

    // commented out for now

    /*Serial.println("Creating Image");

    image = lv_img_create(lv_scr_act()); // set image's definition 

    Serial.println("Define Image");

    lv_img_set_src(image, &coolio); // set the image to the contents of coolio's adress

    Serial.println("Set Image to start");

    lv_obj_center(image); // center the image

    Serial.println("Centered Image");

    lastChange = millis(); // reset lastChange*/
}

void loop() {
    
  readMPU();
  updateLCD();
  delay(500);

}

