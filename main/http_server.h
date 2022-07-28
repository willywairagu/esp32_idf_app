/*
 * http_server.h
 *
 *  Created on: Jul 27, 2022
 *      Author: Wilberforce
 */

#ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

/*
 * Messages for the HTTP monitor
 */
typedef enum http_server_message
{
	HTTP_MSG_WIFI_CONNECT_INIT = 0,
	HTTP_MSG_WIFI_CONNECT_SUCCESS,
	HTTP_MSG_WIFI_CONNECT_FAIL,
	HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
	HTTP_MSG_OTA_UPDATE_FAIL,
	HTTP_MSG_OTA_UPDATE_INITIALIZED
} http_server_message_e;

/*
 * Structure for the message queue
 */
typedef struct http_server_queue_message
{
	http_server_message_e msgID;
} http_server_queue_message_t;

/*
 * prototype for a function that sends message to the queue
 * @param msgID  message ID from the http_server_message_e enum
 * @return pdTrue if an item was successfully sent to the queue, otherwise pdFalse
 * @note Expand based on your requirements eg how you've expanded the http_server_queue_message_t
 */

BaseType_t http_server_monitor_send_message(http_server_message_e msgID);

/*
 *prototype to Starts HTTP server
 */
void http_server_start(void);

/*
 * prototype to stop the http server
 */
void http_server_stop(void);

#endif /* MAIN_HTTP_SERVER_H_ */