// header format: <> - search in library  "" - search in project folder

#include <Arduino.h> // basic arduino actions
#include <lvgl.h> // for creating ui to put on the screen
#include "lvgl_v8_port.h" // connector between esp32 and the lcd
#include <esp_display_panel.hpp> // gives access to the board and lcd

using namespace esp_panel::drivers;
using namespace esp_panel::board;

lv_indev_t *touchInput; // for reading the touch screen device

lv_obj_t *rectangle; // rectangle

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

    // get lcd and touch objects
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

    touchInput = lv_indev_get_next(NULL); // set the touchInput to the registered touch device

    // ********************* DRAW A LABEL ********************************

    lv_obj_t *text = lv_label_create(lv_scr_act()); // create a lable to draw on
    Serial.println("Text created");

    lv_label_set_text(text, "This is a rectangle: "); 
    Serial.println("Text set");

    lv_obj_set_style_text_font(text, &lv_font_montserrat_32, LV_PART_MAIN); // draw the lable on the main screen at certain font
    Serial.println("Font set");

    lv_obj_set_style_text_color(text, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // change the lable's text to a white color
    Serial.println("Text color set");

    lv_obj_align(text, LV_ALIGN_TOP_LEFT, 10, 20); // put the text at top left 10px right 20px down
    Serial.println("Text aligned");

    // ********************* DRAW A RECTANGLE ********************************

    rectangle = lv_obj_create(lv_scr_act()); // define the rectangle as an object
    Serial.println("Rectangle created");

    lv_obj_set_size(rectangle, 200, 100); // set the rectangle to 200x100 px
    Serial.println("Rectangle size set");

    lv_obj_set_style_bg_color(rectangle, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // make the rectangle white-ish
    Serial.println("Rectangle color set");

    lv_obj_set_style_radius(rectangle, 0, LV_PART_MAIN); // make the rectangle be 0% round
    Serial.println("Rectangle radius set");

    //lv_obj_align(rectangle, LV_ALIGN_CENTER, 0, 0);
    Serial.println("Rectangle aligned");

    lvgl_port_unlock();

    Serial.println("LVGL unlocked");
    Serial.println("Text test finished");
}

void loop() {
    lv_point_t point; // make a variable for the point of touch

    lv_indev_get_point(touchInput, &point); // read the touch point; if any
    
    // output reading if the finger is pressing the screen
    if (lv_indev_get_type(touchInput) == LV_INDEV_STATE_PRESSED) {
       /*Serial.print("Touch: ");
        Serial.print(point.x);
        Serial.print(", ");
        Serial.print(point.y);

        Serial.print(" | Rectangle: ");
        Serial.print(lv_obj_get_x(rectangle));
        Serial.print(", ");
        Serial.println(lv_obj_get_y(rectangle));*/

        lvgl_port_lock(-1);

        lv_obj_set_pos(rectangle, point.x - 100, point.y - 50);

        lvgl_port_unlock();

        delay(15);
    }
}