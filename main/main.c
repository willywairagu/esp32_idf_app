/*
 * main.c
 * Application Entry Point.
 *
 * Created on: Jul 15, 2022
 * Author: Wilberforce
 */
#include "nvs_flash.h"
#include "wifi_app.h"

void app_main()
{
//	 Initialise NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	//
	wifi_app_start();
}
