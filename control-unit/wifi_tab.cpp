#include "wifi_tab.h"

// ---------------- States ----------------
#define WIFI_STATE_DISCONNECTED 0
#define WIFI_STATE_CONNECTED 1
#define WIFI_STATE_CONNECTING 2
static int connection_state = WIFI_STATE_DISCONNECTED;
static String selected_wifi = "";

static int tries = 10;

#define GUI_STATE_DEFAULT 0
#define GUI_STATE_ENTER_PASSWORD 1
static int gui_state = GUI_STATE_DEFAULT;

// ---------------- Function protypes ----------------
static void update_gui();
static void scan_networks();

// ---------------- Event handler prototypes ----------------
static void ta_event_cb(lv_obj_t * ta, lv_event_t event);
static void event_handler_connect(lv_obj_t * obj, lv_event_t event);
static void event_handler_disconnect(lv_obj_t * obj, lv_event_t event);
static void event_handler_scan(lv_obj_t * obj, lv_event_t event);
static void event_handler_ssid_roller(lv_obj_t * obj, lv_event_t event);
static void event_handler_close(lv_obj_t * obj, lv_event_t event);

// ---------------- Task callback prototypes ----------------
static lv_task_t * task;
void connect_wifi_task_cb(lv_task_t * task);

// ---------------- GUI objects ----------------

// ---------------- password ----------------
static lv_obj_t * pwd_ta;
static lv_obj_t * pwd_label;
// ---------------- ssid ----------------
static lv_obj_t * ssid_roller;
static lv_obj_t * ssid_label;

// ---------------- buttons ----------------
static lv_obj_t * btn_connect;
static lv_obj_t * button_label_connect;
static lv_obj_t * btn_disconnect;
static lv_obj_t * button_label_disconnect;
static lv_obj_t * btn_scan;
static lv_obj_t * button_label_scan;
static lv_obj_t * btn_close;
static lv_obj_t * button_label_close;

// ---------------- WiFi Info ----------------

static lv_obj_t * label_connected_ssid;
static lv_obj_t * label_connection_strength;
static lv_obj_t * label_ip_address;

// ---------------- keyboard ----------------
static lv_obj_t * kb;

// ---------------- spinner ----------------
static lv_obj_t * preload;

void create_wifi_tab(lv_obj_t * parent) {
  /* Create the password box */
  pwd_ta = lv_textarea_create(parent, NULL);
  lv_textarea_set_text(pwd_ta, "");
  lv_textarea_set_pwd_mode(pwd_ta, true);
  lv_textarea_set_one_line(pwd_ta, true);
  lv_textarea_set_cursor_hidden(pwd_ta, true);
  lv_obj_align(pwd_ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 20);
  lv_obj_set_width(pwd_ta, LV_HOR_RES / 2 - 20);
  lv_obj_set_event_cb(pwd_ta, ta_event_cb);

  /* Create a label and position it above the text box */
  pwd_label = lv_label_create(parent, NULL);
  lv_label_set_text(pwd_label, "Password:");
  lv_obj_align(pwd_label, pwd_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  /* Create the SSID roller */
  ssid_roller = lv_roller_create(parent, NULL);
  lv_roller_set_visible_row_count(ssid_roller, 2);
  //lv_obj_set_size(ssid_roller,  LV_HOR_RES / 2 - 20, 40);
  lv_obj_align(ssid_roller, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 20);
  lv_obj_set_event_cb(ssid_roller, event_handler_ssid_roller);

  /* Create a label and position it above the text box */
  ssid_label = lv_label_create(parent, NULL);
  lv_label_set_text(ssid_label, "SSID:");
  lv_obj_align(ssid_label, ssid_roller, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

  // Initialize buttons
  // Connect button
  btn_connect = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_connect, event_handler_connect);
  lv_obj_set_size(btn_connect,  LV_HOR_RES / 2.5, 40);
  lv_obj_align(btn_connect, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

  button_label_connect = lv_label_create(btn_connect, NULL);
  lv_label_set_text(button_label_connect, "Connect");

  // Disconnect button
  btn_disconnect = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_disconnect, event_handler_disconnect);
  lv_obj_set_size(btn_disconnect,  LV_HOR_RES / 2.5, 40);
  lv_obj_align(btn_disconnect, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -30);

  button_label_disconnect = lv_label_create(btn_disconnect, NULL);
  lv_label_set_text(button_label_disconnect, "Disconnect");

  // Scan button
  btn_scan = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_scan, event_handler_scan);
  lv_obj_set_size(btn_scan,  LV_HOR_RES / 2.5, 30);
  lv_obj_align(btn_scan, ssid_roller, LV_ALIGN_OUT_BOTTOM_MID, 60, 10);

  button_label_scan = lv_label_create(btn_scan, NULL);
  lv_label_set_text(button_label_scan, "Scan");

  // Close button
  btn_close = lv_btn_create(parent, NULL);
  lv_obj_set_event_cb(btn_close, event_handler_close);
  lv_obj_set_size(btn_close,  50, 50);
  lv_obj_align(btn_close, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);

  button_label_close = lv_label_create(btn_close, NULL);
  lv_label_set_text(button_label_close, "X");

  /* Create the keyboard */
  kb = lv_keyboard_create(parent, NULL);
  lv_obj_set_size(kb,  LV_HOR_RES - 50, LV_VER_RES / 2);
  lv_obj_align(kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, -90);

  lv_keyboard_set_textarea(kb, pwd_ta); /* Focus it on one of the text areas to start */
  lv_keyboard_set_cursor_manage(kb, true); /* Automatically show/hide cursors on text areas */

  preload = lv_spinner_create(parent, NULL);
  lv_obj_set_size(preload, 100, 100);
  lv_obj_align(preload, NULL, LV_ALIGN_CENTER, 0, 0);

  /* Connection info labels*/
  label_connected_ssid = lv_label_create(parent, NULL);
  lv_label_set_text(label_connected_ssid, "SSID: ");
  lv_obj_align(label_connected_ssid, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

  label_connection_strength = lv_label_create(parent, NULL);
  lv_label_set_text(label_connection_strength, "RSSI: ");
  lv_obj_align(label_connection_strength, NULL, LV_ALIGN_IN_TOP_MID, 0, 40);

  label_ip_address = lv_label_create(parent, NULL);
  lv_label_set_text(label_ip_address, "IP Address: ");
  lv_obj_align(label_ip_address, NULL, LV_ALIGN_IN_TOP_MID, 0, 60);

  scan_networks();
  update_gui();
}

static void update_gui() {
  if (gui_state == GUI_STATE_ENTER_PASSWORD) {
    // show keyboard and close button
    lv_obj_set_hidden(kb, false);
    lv_obj_set_hidden(btn_close, false);

    // move password text area to top mid
    lv_obj_align(pwd_ta, NULL, LV_ALIGN_IN_TOP_MID, -10, 20);
    lv_obj_align(pwd_label, pwd_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    //hide all other objects
    lv_obj_set_hidden(ssid_roller, true);
    lv_obj_set_hidden(ssid_label, true);
    lv_obj_set_hidden(btn_connect, true);
    lv_obj_set_hidden(button_label_connect, true);
    lv_obj_set_hidden(btn_scan, true);
    lv_obj_set_hidden(button_label_scan, true);

  } else if (gui_state == GUI_STATE_DEFAULT) {
    if (connection_state == WIFI_STATE_DISCONNECTED) {
      // Hide keyboard and close button
      lv_obj_set_hidden(kb, true);
      lv_obj_set_hidden(btn_close, true);
      // Hide spinner
      lv_obj_set_hidden(preload, true);

      // Hide connected objects
      lv_obj_set_hidden(btn_disconnect, true);
      lv_obj_set_hidden(button_label_disconnect, true);
      lv_obj_set_hidden(label_connected_ssid, true);
      lv_obj_set_hidden(label_connection_strength, true);
      lv_obj_set_hidden(label_ip_address, true);

      //show all other objects
      lv_obj_set_hidden(pwd_ta, false);
      lv_obj_set_hidden(pwd_label, false);
      lv_obj_set_hidden(ssid_roller, false);
      lv_obj_set_hidden(ssid_label, false);
      lv_obj_set_hidden(btn_connect, false);
      lv_obj_set_hidden(button_label_connect, false);
      lv_obj_set_hidden(btn_scan, false);
      lv_obj_set_hidden(button_label_scan, false);


      // move password text area to default position
      lv_obj_align(pwd_ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -10, 20);
      lv_obj_align(pwd_label, pwd_ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    } else if (connection_state == WIFI_STATE_CONNECTED) {
      // Hide spinner
      lv_obj_set_hidden(preload, true);
      // Delete possibly running task
      lv_task_del(task);
      // Show connected Wifi SSID
      lv_obj_set_hidden(label_connected_ssid, false);
      String ssid = "SSID: " + String(WiFi.SSID());
      lv_label_set_text(label_connected_ssid, ssid.c_str());
      // Show signal strength
      lv_obj_set_hidden(label_connection_strength, false);
      String rssi = "RSSI: " + String(WiFi.RSSI());
      lv_label_set_text(label_connection_strength, rssi.c_str());
      // Show IP address
      lv_obj_set_hidden(label_ip_address, false);
      String ipAddress = "IP Address: " + WiFi.localIP().toString();
      lv_label_set_text(label_ip_address, ipAddress.c_str());
      // show disconnect button
      lv_obj_set_hidden(btn_disconnect, false);
      lv_obj_set_hidden(button_label_disconnect, false);
    } else if (connection_state == WIFI_STATE_CONNECTING) {
      // Hide all objects
      lv_obj_set_hidden(pwd_ta, true);
      lv_obj_set_hidden(pwd_label, true);
      lv_obj_set_hidden(ssid_roller, true);
      lv_obj_set_hidden(ssid_label, true);
      lv_obj_set_hidden(btn_connect, true);
      lv_obj_set_hidden(button_label_connect, true);
      lv_obj_set_hidden(btn_scan, true);
      lv_obj_set_hidden(button_label_scan, true);
      lv_obj_set_hidden(btn_disconnect, true);
      lv_obj_set_hidden(button_label_disconnect, true);
      lv_obj_set_hidden(label_connected_ssid, true);
      lv_obj_set_hidden(label_connection_strength, true);
      lv_obj_set_hidden(label_ip_address, true);

      // Show circular progress indicator
      lv_obj_set_hidden(preload, false);

      // Start connecting to wifi
      String password = lv_textarea_get_text(pwd_ta);
      WiFi.begin(selected_wifi.c_str(), password.c_str());
      Serial.println(password);
      Serial.println(selected_wifi);
      tries = 20;
      task = lv_task_create(connect_wifi_task_cb, 500, LV_TASK_PRIO_MID, NULL);
      lv_task_set_repeat_count(task, 21);
      lv_task_ready(task);
    } else {
      Serial.println("Undefined Wifi Connection state");
    }
  } else {
    Serial.println("Undefined GUI State");
  }

}

void connect_wifi_task_cb(lv_task_t * task) {
  // If connection established done do anything
  if(connection_state == WIFI_STATE_CONNECTED){
    return;
  }
  if (WiFi.status() != WL_CONNECTED && tries > 0) {
    // Continue trying to connect
    Serial.print("Tries: ");
    Serial.println(tries);
    tries--;
  } else if (WiFi.status() != WL_CONNECTED && tries == 0) {
    // If connection failed after 10 seconds (20 * 500ms)
    Serial.println("Connection failed");
    connection_state = WIFI_STATE_DISCONNECTED;
    update_gui();
  } else if (WiFi.status() == WL_CONNECTED) {
    // If connection is established
    Serial.println("Connection successfull");
    connection_state = WIFI_STATE_CONNECTED;
    update_gui();
  }
}

static void scan_networks() {

  int numberOfNetworks = WiFi.scanNetworks();

  String ssids = "";

  for (int i = 0; i < numberOfNetworks; i++) {
    if (i + 1 < numberOfNetworks) {
      ssids = ssids + WiFi.SSID(i) + "\n";
    } else {
      ssids = ssids + WiFi.SSID(i);
    }
  }

  lv_roller_set_options(ssid_roller,
                        ssids.c_str(),
                        LV_ROLLER_MODE_NORMAL);
}

static void ta_event_cb(lv_obj_t * ta, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    /* Focus on the clicked text area */
    if (kb != NULL)
      lv_keyboard_set_textarea(kb, ta);
    gui_state = GUI_STATE_ENTER_PASSWORD;
    update_gui();
  }
}

static void event_handler_ssid_roller(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_VALUE_CHANGED) {
    char buf[32];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    selected_wifi = String(buf);
  }
}

static void event_handler_close(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    gui_state = GUI_STATE_DEFAULT;
    update_gui();
  }
}

static void event_handler_connect(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    if (connection_state == WIFI_STATE_DISCONNECTED) {
      connection_state = WIFI_STATE_CONNECTING;
      update_gui();
    }
  }
}

static void event_handler_disconnect(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    if (connection_state == WIFI_STATE_CONNECTED) {
      WiFi.disconnect();
      connection_state = WIFI_STATE_DISCONNECTED;
      update_gui();
    }
  }
}

static void event_handler_scan(lv_obj_t * obj, lv_event_t event)
{
  if (event == LV_EVENT_CLICKED) {
    scan_networks();
    update_gui();
  }
}
