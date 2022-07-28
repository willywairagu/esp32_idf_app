/*
 * rgd_led.h
 *
 *  Created on: Jul 15, 2022
 *      Author: Wilberforce
 */

#ifndef MAIN_RGB_LED_H_
#define MAIN_RGB_LED_H_

// RGB LED GPIOS
#define RGB_LED_RED_GPIO	21
#define RGB_LED_GREEN_GPIO	22
#define RGB_LED_BLUE_GPIO	23

//RGB LED Color mix channels
#define RGB_LED_CHANNEL_NUM 3

//RGB LED CONFIGURATIONS
typedef struct
{
	int channel;
	int gpio;
	int mode;
	int timer_index;
} led_c_info_t;

//creating an array with the same number of channels
led_c_info_t ledc_ch[RGB_LED_CHANNEL_NUM];

/*
 * Color to indicate WIFI application started
 */
void rgb_led_wifi_app_started(void);


/*
 * Color to indicate HTTP SERVER started
 */
void rgb_led_http_server_started(void);

/*
 * Color to indicate that the esp32 is connected to an access point
 */
void rgb_led_wifi_connected(void);

#endif /* MAIN_RGB_LED_H_ */
