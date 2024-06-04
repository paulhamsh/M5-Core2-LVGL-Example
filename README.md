# M5-Core2-LVGL-Example
## M5 Stack LVGL Example   
For v8.x   
Based on code here:   
https://github.com/bareboat-necessities/bbn-m5stack-tough   

Just to have a working version in my repo   

## For v8.4    

Install lvgl in library manager   
Open Arduino\libraries\lvgl    
Copy lv_conf_template.h to serc\lv_conf.h    
Edit the settings  

```
/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#define LV_COLOR_DEPTH 16
#define LV_COLOR_SCREEN_TRANSP 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_TICK_CUSTOM 1
```

The compile and run.   

## For v9.0

Install lvgl in library manager   
Compile and run - no changes needed to lv_conf.h   
