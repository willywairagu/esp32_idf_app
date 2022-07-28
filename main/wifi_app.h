/*
 * wifi_app.h
 *
 *  Created on: Jul 22, 2022
 *      Author: Wilberforce
 */

#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include "esp_netif.h"

// WIFI application settings
#define WIFI_AP_SSID				"ESP32_AP"
#define WIFI_AP_PASSWORD			"password"
#define WIFI_AP_CHANNEL 			1					// channel, WIFI has 14, 5Mhz spaced channels under the 2.4ghz band
#define WIFI_AP_SSID_HIDDEN 		0					// AP visibility
#define WIFI_AP_MAX_CONNECTIONS		5					// AP maximum clients
#define WIFI_AP_BEACON_INTERVAL		100					// AP beacon interval, 100ms recommended
#define WIFI_AP_IP					"192.168.0.1"  		// AP default IP that we assigng to the SoftAP
#define WIFI_AP_GATEWAY				"192.168.0.1" 		//AP default Gateway (Should be same as ip address
#define WIFI_AP_NETMASK				"255.255.255.0"  	//AP netmask
#define  WIFI_AP_BANDWIDTH			WIFI_BW_HT20 		// AP BW is 20Mhz
#define WIFI_STA_POWER_SAVE			WIFI_PS_NONE		// Power save not used
#define MAX_SSID_LENGTH				32					// IEEE standard maximum
#define MAX_PASSWORD_LENGTH			64					// IEEE standard maximum
#define MAX_CONNECTION_RETRIES		5					// Retry number on disconnect

// network interface objects for the station and access point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

/*
 * Message IDs for the WIFI application task
 * can be expanded based on application requirements
 */
typedef enum wifi_app_message
{
	WIFI_APP_MSG_START_HTTP_SERVER = 0,
	WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
	WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
} wifi_app_message_e;

/*
 * structure for the message queue
 * can also be expanded based on application eg addng another parameter
 */
typedef struct wifi_app_queue_message
{
	wifi_app_message_e msgID;
} wifi_app_queue_message_t;

/**
 * prototype to send a message to the queue
 * @params msgID message ID from wifi_app_message_e enum
 * @return pdTrue if an item was successfully sent to the queue, otherwise pdFalse
 * #note Expand the parameter list based on your requirements eg how you've expanded the wifi_app_queue_message_t
 */

BaseType_t wifi_app_send_message(wifi_app_message_e msgID);
/*
 * starts the wifi RTOS task an take
 */
void wifi_app_start(void);

#endif /* MAIN_WIFI_APP_H_ */
