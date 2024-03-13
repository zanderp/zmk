#ifndef __INDICATOR_LED__
#define __INDICATOR_LED__


uint8_t zmk_indicator_led_get_brt();
uint8_t zmk_indicator_led_calc_brt_cycle();
int zmk_indicator_led_set_brt(uint8_t brightness);
#endif