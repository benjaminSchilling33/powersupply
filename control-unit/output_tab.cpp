#include "output_tab.h"

// ---------------- States ----------------
#define OUTPUT_STATE_DISABLED 0
#define OUTPUT_STATE_ENABLED 1

static int output_state = OUTPUT_STATE_ENABLED;

// ---------------- Task callback prototypes ----------------
static lv_task_t * task;
void output_update_task_cb(lv_task_t * task);

// ---------------- GUI Objects ----------------
static lv_obj_t * gauge_voltage;
static lv_obj_t * gauge_current;
static lv_obj_t * led_status;

static lv_obj_t * label_voltage;
static lv_obj_t * label_current;

void create_output_tab(lv_obj_t * parent) {
  /*Describe the color for the needles*/
  static lv_color_t needle_color[1];
  needle_color[0] = LV_COLOR_BLUE;

  /*Create voltage gauge*/
  gauge_voltage = lv_gauge_create(parent, NULL);
  lv_gauge_set_scale(gauge_voltage, 220, 21, 6);
  lv_gauge_set_range(gauge_voltage, 0, 24);
  lv_gauge_set_needle_count(gauge_voltage, 1, needle_color);
  lv_obj_set_size(gauge_voltage, 200, 200);
  lv_obj_align(gauge_voltage, NULL, LV_ALIGN_IN_TOP_LEFT, 30, 10);
  
  /* Create voltage label */
  label_voltage = lv_label_create(parent, NULL);
  lv_obj_align(label_voltage, gauge_voltage, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

  /* Create voltage gauge label */
  lv_obj_t * label_voltage_gauge = lv_label_create(parent, NULL);
  lv_label_set_text(label_voltage_gauge, "Voltage");
  lv_obj_align(label_voltage_gauge, gauge_voltage, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  /*Create current gauge*/
  gauge_current = lv_gauge_create(parent, NULL);
  lv_gauge_set_scale(gauge_current, 220, 21, 6);
  lv_gauge_set_range(gauge_current, 0, 5);
  lv_gauge_set_needle_count(gauge_current, 1, needle_color);
  lv_obj_set_size(gauge_current, 200, 200);
  lv_obj_align(gauge_current, NULL, LV_ALIGN_IN_TOP_RIGHT, -30, 10);
  
  /* Create current label */
  label_current = lv_label_create(parent, NULL);
  lv_obj_align(label_current, gauge_current, LV_ALIGN_IN_BOTTOM_MID, 0, -10);

  /* Create current gauge label */
  lv_obj_t * label_current_gauge = lv_label_create(parent, NULL);
  lv_label_set_text(label_current_gauge, "Current");
  lv_obj_align(label_current_gauge, gauge_current, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  /* Create status LED */
  led_status  = lv_led_create(parent, NULL);
  static lv_style_t led_style;
  lv_style_init(&led_style);
  lv_style_set_bg_color(&led_style, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_obj_add_style(led_status, LV_LED_PART_MAIN, &led_style);
  lv_obj_align(led_status, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -40);

  /* Create status LED label */
  lv_obj_t * label_status = lv_label_create(parent, NULL);
  lv_label_set_text(label_status, "EN");
  lv_obj_align(label_status, led_status, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  task = lv_task_create(output_update_task_cb, 500, LV_TASK_PRIO_MID, NULL);
  lv_task_ready(task);
}

void output_update_task_cb(lv_task_t * task) {
  if (output_state == OUTPUT_STATE_ENABLED) {
    randomSeed(analogRead(0));

    // Set Voltage
    int randomVoltage = random(0,5);
    lv_gauge_set_value(gauge_voltage, 0, randomVoltage);
    String voltage = String(randomVoltage) + "V";
    lv_label_set_text(label_voltage, voltage.c_str());

    // Set Current
    int randomCurrent = random(0,5);
    lv_gauge_set_value(gauge_current, 0, randomCurrent);
    String current = String(randomCurrent) + "A";
    lv_label_set_text(label_current, current.c_str());

    // Set LED Status
    lv_led_on(led_status);
  }
}
