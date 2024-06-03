// Used template code from:
// https://github.com/0xxon/LVGL-PlatformIO-Example/blob/main/src/main.cpp
//
// No changes to lv_conf.h
//
// The M5 Lcd class has parent TFT_eSPI, so the calls in the display driver are to the parent class
// https://github.com/m5stack/M5Core2/blob/master/src/M5Display.h
// https://github.com/Bodmer/TFT_eSPI/blob/master/TFT_eSPI.h
// 

#include <M5Core2.h>

#define LV_HOR_RES_MAX 320
#define LV_VER_RES_MAX 240

#include <lvgl.h>

lv_display_t *disp;
lv_indev_t   *indev;


static uint32_t my_tick_function(void) {
  return millis();
}


void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lv_draw_sw_rgb565_swap(px_map, w * h);
  M5.Lcd.pushImage(area->x1, area->y1, w, h, (uint16_t *)px_map);
  lv_disp_flush_ready(disp);
}


void init_disp() {
  static lv_draw_buf_t *buf1 = (lv_draw_buf_t *)heap_caps_malloc((LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t)), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  static lv_draw_buf_t *buf2 = (lv_draw_buf_t *)heap_caps_malloc((LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t)), MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
  disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, buf2, LV_HOR_RES_MAX * LV_VER_RES_MAX * sizeof(lv_color_t), LV_DISPLAY_RENDER_MODE_FULL);
}


void my_touchpad_read(lv_indev_t *drv, lv_indev_data_t *data) {
  TouchPoint_t pos = M5.Touch.getPressPoint();
  bool touched = (pos.x == -1) ? false : true;
  if (!touched) {
    data->state = LV_INDEV_STATE_RELEASED;
  } 
  else {
    data->state = LV_INDEV_STATE_PRESSED;
    data->point.x = pos.x;
    data->point.y = pos.y;
  }
}

void init_touch() {
  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, my_touchpad_read);
}

// LVGL display elements and callbacks

static lv_obj_t *label1;
lv_obj_t *slider1;
lv_obj_t *btn1;


static void btn_event_cb(lv_event_t *e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t *btn = (lv_obj_t *) lv_event_get_target(e);
  if (code == LV_EVENT_CLICKED) {
    static uint8_t cnt = 0;
    cnt++;
    /*Get the first child of the button which is the label and change its text*/
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "Button: %d", cnt);
  }
}


static void slider_event_cb(lv_event_t *e)
{
  lv_obj_t *slider = (lv_obj_t *) lv_event_get_target(e);
  //lv_obj_t *label = lv_obj_get_child(slider, 0);
  lv_label_set_text_fmt(label1, "%"LV_PRId32, lv_slider_get_value(slider));
  lv_obj_align_to(label1, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
}

void setup() {
  M5.begin();

  lv_init();
  lv_tick_set_cb(my_tick_function);
  init_disp();
  init_touch();

  // Create screen elements

  slider1 = lv_slider_create(lv_screen_active());
  lv_slider_set_value(slider1, 0, LV_ANIM_ON);
  lv_obj_set_pos(slider1, 40, 60);                            /*Set its position*/
  lv_obj_set_size(slider1, 200, 20);                          /*Set its size*/
  lv_obj_add_event_cb(slider1, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
  
  label1 = lv_label_create(lv_screen_active());
  lv_label_set_text(label1, "0");
  lv_obj_align_to(label1, slider1, LV_ALIGN_OUT_TOP_MID, 0, -15);  


  lv_obj_t *label2 = lv_label_create(slider1);                 /*Add a label to the button*/
  lv_label_set_text(label2, "Slider");                         /*Set the labels text*/
  lv_obj_center(label2);

  btn1 = lv_button_create(lv_screen_active());                 /*Add a button the current screen*/
  lv_obj_set_pos(btn1, 40, 120);                               /*Set its position*/
  lv_obj_set_size(btn1, 100, 50);                              /*Set its size*/
  lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_ALL, NULL); /*Assign a callback to the button*/

  lv_obj_t *label3 = lv_label_create(btn1);                    /*Add a label to the button*/
  lv_label_set_text(label3, "Button");                         /*Set the labels text*/
  lv_obj_center(label3);
}

void loop() {
  M5.update();
  lv_task_handler();
}
