#include <Arduino.h>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

using namespace esp_panel::drivers;
using namespace esp_panel::board;

void setup() {

    Serial.begin(115200);
    delay(1000);

    Serial.println("1. Starting");

    Board *board = new Board();

    Serial.println("2. Board created");

    bool initResult = board->init();

    Serial.print("3. Board init result = ");
    Serial.println(initResult);

    LCD *lcd = board->getLCD();

    if (lcd == nullptr) {
        Serial.println("LCD IS NULL!");
        return;
    } 
    else {
        Serial.println("LCD EXISTS AFTER INIT!");
    }

    bool beginResult = board->begin();

    Serial.print("4. Board begin result = ");
    Serial.println(beginResult);

    if (!beginResult) {
        Serial.println("BOARD BEGIN FAILED!");
        return;
    }

    Serial.println("5. Board begun successfully");

    bool colorTestResult = lcd->colorBarTest();

    Serial.print("6. Color bar test result = ");
    Serial.println(colorTestResult);

    if (colorTestResult) {
        Serial.println("COLOR BAR TEST SENT!");
    } 
    else {
        Serial.println("COLOR BAR TEST FAILED!");
    }
}

void loop() {
    delay(1000);
    Serial.println("Running");
}