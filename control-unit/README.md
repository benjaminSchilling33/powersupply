# control-unit

The control-unit consists of a WEMOS LOLIN32 board (based on the ESP32) as well as a Joy-It 3.5' touch display.

The GUI is based on the [lvgl](https://lvgl.io/) embedded graphics library.

The control unit contains three tabs:

1. Display of output voltage and current
2. Documentation of REST Interface
3. WiFi settings

## Dependencies

| Library           | Version                                  |
| ----------------- | ---------------------------------------- |
| arduino-esp32     | dcff2e9774c1a712e601fd62d08a68a5e74dbc62 |
| lvgl              | 7.7.2                                    |
| TFT_eSPI          | 2.3.4                                    |
| AsyncTCP          | ca8ac5f919d02bea07b474531981ddbfd64de97c |
| ESPAsyncWebServer | f6fff3f91ebf45b91ca4cff2460d2febd61f1e27 |
