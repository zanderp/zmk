#pragma once

int zmk_indicator_on();
int zmk_indicator_off();
int zmk_indicator_toggle();
bool zmk_indicator_is_on();

int zmk_indicator_set_brt(uint8_t brightness);
uint8_t zmk_indicator_get_brt();
uint8_t zmk_indicator_calc_brt(int direction);
uint8_t zmk_indicator_calc_brt_cycle();