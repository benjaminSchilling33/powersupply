#include <lvgl.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "output_tab.h"
#include "rest_interface_tab.h"
#include "wifi_tab.h"

#define screenWidth 480
#define screenHeight 320
#define LVGL_TICK_PERIOD 20

// GUI Objects
TFT_eSPI tft = TFT_eSPI(); /* TFT instance */
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
Ticker   lvgl_tick;             /* timer for lvgl */

// REST Interface
AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

/* Display flushing */
void disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, w, h);
  tft.pushColors(&color_p->full, w * h, true);
  tft.endWrite();

  lv_disp_flush_ready(disp);
}

/*Read the touchpad*/
bool touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  uint16_t touchX, touchY;

  bool touched = tft.getTouch(&touchX, &touchY, 600);

  if (!touched)
  {
    data->state = LV_INDEV_STATE_REL;
    return false;
  }
  else
  {
    data->state = LV_INDEV_STATE_PR;
  }

  if (touchX > screenWidth || touchY > screenHeight)
  {
    Serial.println("Y or y outside of expected parameters..");
  }
  else
  {
    /*Set the coordinates*/
    if (3 == tft.getRotation()) {
      //Shift coordinates
      data->point.x = 480 - touchX;
      data->point.y = 320 - touchY;
    } else if (1 == tft.getRotation()) {
      data->point.x = touchX;
      data->point.y = touchY;
    }
  }

  return false; /*Return `false` because we are not buffering and no more data to read*/
}

void lv_tick_handler() {
  lv_tick_inc(LVGL_TICK_PERIOD);
}

void create_tab_view() {
  /*Create a Tab view object*/
  lv_obj_t *tabview;
  tabview = lv_tabview_create(lv_scr_act(), NULL);

  /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
  lv_obj_t *tab_output = lv_tabview_add_tab(tabview, "Output");
  lv_obj_t *tab_rest_interface = lv_tabview_add_tab(tabview, "REST\nInterface");
  lv_obj_t *tab_wifi = lv_tabview_add_tab(tabview, "Wifi");

  lv_page_set_scroll_propagation(tab_output, false);
  lv_page_set_scroll_propagation(tab_rest_interface, false);
  lv_page_set_scroll_propagation(tab_wifi, false);

  /*Add content to the tabs*/
  create_output_tab(tab_output);
  create_rest_interface_tab(tab_rest_interface);
  create_wifi_tab(tab_wifi);
}

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */
  EEPROM.begin(512);

  // ================= GUI =================
  lv_init();

  tft.begin(); /* TFT init */
  tft.setRotation(3); /* Landscape orientation */

  uint16_t calibrationData[5] = { 305, 3542, 245, 3529, 3  };
  tft.setTouch(calibrationData);

  lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

  /*Initialize the display*/
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = 480;
  disp_drv.ver_res = 320;
  disp_drv.flush_cb = disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /*Initialize the (dummy) input device driver*/
  lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read;
  lv_indev_drv_register(&indev_drv);

  lvgl_tick.attach_ms(LVGL_TICK_PERIOD, lv_tick_handler);

  //Create tab view gui
  create_tab_view();

  // ================= REST Interface =================
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", "Hello, world");
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
}


void loop()
{
  /* let the GUI do its work */
  lv_task_handler();
  //delay(5);
}
