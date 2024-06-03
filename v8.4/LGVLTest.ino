// Used instructions and template code from:
// https://github.com/bareboat-necessities/bbn-m5stack-tough
// Under MIT license
//

//  Copy Arduino\libraries\lvgl\lv_conf_template.h to Arduino\libraries\lvgl\src\lv_conf.h
//
//  Changes to lv_conf.h (LVGL 8.4) needed:
//   /* clang-format off */
//   #if 1 /*Set it to "1" to enable content*/
//
//   #define LV_COLOR_DEPTH 16
//   #define LV_COLOR_SCREEN_TRANSP 1
//   #define LV_FONT_MONTSERRAT_20 1
//   #define LV_TICK_CUSTOM 1
//

// The M5 Lcd class has parent TFT_eSPI, so the calls in the display driver are to the parent class
// https://github.com/m5stack/M5Core2/blob/master/src/M5Display.h
// https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_eSPI.h
// 
// LVGL instructions on porting are here (v8.4):
// https://docs.lvgl.io/8.4/get-started/quick-overview.html#learn-the-basics
//


#include <M5Core2.h>

#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

#include <lvgl.h>

static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t      disp_drv;    // Descriptor of a display driver
static lv_indev_drv_t     indev_drv;   // Descriptor of a touch driver

static unsigned long last_touched;

M5Display *tft;

void tft_lv_initialization() {
  static lv_color_t *buf1 = (lv_color_t *)heap_caps_malloc((LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t)), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  static lv_color_t *buf2 = (lv_color_t *)heap_caps_malloc((LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t)), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  lv_disp_draw_buf_init(&draw_buf, buf1, buf2, (LV_HOR_RES_MAX * LV_VER_RES_MAX));
  tft = &M5.Lcd;
}

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  tft->startWrite();
  tft->setAddrWindow(area->x1, area->y1, w, h);
  tft->pushColors((uint16_t *)&color_p->full, w * h, true);
  tft->endWrite();

  lv_disp_flush_ready(disp);
}

void init_disp_driver() {
  lv_disp_drv_init(&disp_drv);  // Basic initialization
  disp_drv.flush_cb = my_disp_flush;  // Set your driver function
  disp_drv.draw_buf = &draw_buf;      // Assign the buffer to the display
  disp_drv.hor_res = LV_HOR_RES_MAX;  // Set the horizontal resolution of the display
  disp_drv.ver_res = LV_VER_RES_MAX;  // Set the vertical resolution of the display

  lv_disp_drv_register(&disp_drv);                   // Finally register the driver
  lv_disp_set_bg_color(NULL, lv_color_hex3(0x000));  // Set default background color to black
}


void my_touchpad_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
  TouchPoint_t pos = M5.Touch.getPressPoint();
  bool touched = (pos.x == -1) ? false : true;
  if (!touched) {
    data->state = LV_INDEV_STATE_RELEASED;
  } 
  else {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = pos.x;
    data->point.y = pos.y;
    last_touched = millis();
  }
}

void init_touch_driver() {
  lv_disp_drv_register(&disp_drv);
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv); 
  last_touched = millis();
}


static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

static lv_obj_t *label1;

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t *slider = lv_event_get_target(e);

    /*Refresh the text*/
    lv_label_set_text_fmt(label1, "%"LV_PRId32, lv_slider_get_value(slider));
    lv_obj_align_to(label1, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}

void setup() {
  M5.begin();
  M5.Lcd.setTextColor(BLUE);
  M5.Lcd.setTextSize(2);    
  M5.Lcd.setCursor(0,0);
  M5.Lcd.print("Hello");

  lv_init();
  tft_lv_initialization();
  init_disp_driver();
  init_touch_driver();


  lv_obj_t *slider1 = lv_slider_create(lv_scr_act());
  lv_slider_set_value(slider1, 0, LV_ANIM_ON);
  lv_obj_set_pos(slider1, 40, 60);                            /*Set its position*/
  lv_obj_set_size(slider1, 200, 20);                          /*Set its size*/
  lv_obj_add_event_cb(slider1, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  
  /*Create a label above the slider*/
  label1 = lv_label_create(lv_scr_act());
  lv_label_set_text(label1, "0");
  lv_obj_align_to(label1, slider1, LV_ALIGN_OUT_TOP_MID, 0, -15);  


  lv_obj_t *label2 = lv_label_create(slider1);                 /*Add a label to the button*/
  lv_label_set_text(label2, "Slider");                         /*Set the labels text*/
  lv_obj_center(label2);

  lv_obj_t * btn1 = lv_btn_create(lv_scr_act());               /*Add a button the current screen*/
  lv_obj_set_pos(btn1, 40, 120);                               /*Set its position*/
  lv_obj_set_size(btn1, 100, 50);                              /*Set its size*/
  lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t *label3 = lv_label_create(btn1);                    /*Add a label to the button*/
  lv_label_set_text(label3, "Button");                        /*Set the labels text*/
  lv_obj_center(label3);
}

void loop() {
  M5.update();
  lv_task_handler();
}
