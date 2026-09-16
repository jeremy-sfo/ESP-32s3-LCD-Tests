// header format: <> - search in library  "" - search in project folder

#include <Arduino.h> // basic arduino actions
#include <lvgl.h> // for creating ui to put on the screen
#include "lvgl_v8_port.h" // connector between esp32 and the lcd
#include <esp_display_panel.hpp> // gives access to the board and lcd

using namespace esp_panel::drivers;
using namespace esp_panel::board;

void setup() {

    Serial.begin(115200); // start monitor at 112500 baud
    delay(1000);

    Serial.println("Starting LVGL Text Test");

    Board *board = new Board(); // declare the display board
    Serial.println("Board object created");

    if (!board->init()) { // if we can't init, return
        Serial.println("Board init failed!");
        return;
    }

    Serial.println("Board Initialized!");

    LCD *lcd = board->getLCD();
    Touch *touch = board->getTouch();

    Serial.println("LCD and Touch objects obtained");

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

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x101820), LV_PART_MAIN ); // set the background's color * (set screen active, set the color, paint the main screen)

    Serial.println("Background color set");

    lv_obj_t *label = lv_label_create(lv_scr_act());

    Serial.println("Label created");

    lv_label_set_text(label, "HELLO WORLD!");

    Serial.println("Label text set");

    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    Serial.println("Label aligned");

    lv_obj_set_style_text_font(label, &lv_font_montserrat_32, LV_PART_MAIN); // set the main screen's font to montserrat at size 32

    Serial.println("Font set");

    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // change the main screen's text to a dark grey color

    Serial.println("Text color set");

    lvgl_port_unlock();

    Serial.println("LVGL unlocked");
    Serial.println("Text test finished");
}

void loop() {
    delay(1000);
    Serial.println("Running");
}