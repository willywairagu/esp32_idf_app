/*
 * http_server.c
 *
 *  Created on: Jul 27, 2022
 *      Author: Wilberforce
 */
#include "esp_http_server.h"  // http_server components
#include "esp_log.h"   // logging component

#include "http_server.h"
#include "tasks_common.h"
#include "wifi_app.h"

// Tag used for ESP Serial Information console messages
static const char TAG[] = "htp_server";

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;    // instance handle to call various function from the http api

// HTTP Server monitor task handle
static TaskHandle_t task_http_server_monitor = NULL;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t http_server_monitor_queue_handle;

// Embedded files: jquery, index.html, app.css, app.js, favicon.ico files
extern const uint8_t jquery_3_3_1_min_js_start[]	asm("_binary_jquery_3_3_1_min_js_start");   // jquery_3_3_1_min.js
extern const uint8_t jquery_3_3_1_min_js_end[]		asm("_binary_jquery_3_3_1_min_js_end");
extern const uint8_t index_html_start[]				asm("_binary_index_html_start");        // index.html
extern const uint8_t index_html_end[]				asm("_binary_index_html_end");
extern const uint8_t app_css_start[]				asm("_binary_app_css_start");			// app.css
extern const uint8_t app_css_end[]					asm("_binary_app_css_end");
extern const uint8_t app_js_start[]					asm("_binary_app_js_start");			// app.js
extern const uint8_t app_js_end[]					asm("_binary_app_js_end");
extern const uint8_t favicon_ico_start[]			asm("_binary_favicon_ico_start");		//favicon.ico
extern const uint8_t favicon_ico_end[]				asm("_binary_favicon_ico_end");

/*
 * HTTP Server monitor task used to track events of the http server
 * @param pvParameters parameters which can be passed to the task
 */
static void http_server_monitor(void *parameter)
{
	http_server_queue_message_t msg;

	for (;;)
	{
		if (xQueueReceive(http_server_monitor_queue_handle, &msg, portMAX_DELAY))
		{
			switch (msg.msgID)
			{
				case HTTP_MSG_WIFI_CONNECT_INIT:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_INIT");

					break;

				case HTTP_MSG_WIFI_CONNECT_SUCCESS:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_SUCCESS");

					break;

				case HTTP_MSG_WIFI_CONNECT_FAIL:
					ESP_LOGI(TAG, "HTTP_MSG_WIFI_CONNECT_FAIL");

					break;

				case HTTP_MSG_OTA_UPDATE_SUCCESSFUL:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_SUCCESSFUL");

					break;

				case HTTP_MSG_OTA_UPDATE_FAILED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_FAILED");

					break;

				case HTTP_MSG_OTA_UPDATE_INITIALIZED:
					ESP_LOGI(TAG, "HTTP_MSG_OTA_UPDATE_INITIALIZED");

					break;

				default:
					break;
			}
		}
	}
}

/* Jquery get handler, is requested when accessing the web page
 * @param req HTTP request for which the url needs to be handled
 * @return ESP_OK
 *
 */
static esp_err_t http_server_jquery_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "Jquery Requested. ");

	httpd_resp_set_type(req, 'application/javascript');
	httpd_resp_send(req, (const char *)jquery_3_3_1_min_js_start, jquery_3_3_1_min_js_end - jquery_3_3_1_min_js_start);

	return ESP_OK;
}

/* Sends the index.html page
 * @param req HTTP request for which the url needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_index_html_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "Index.html Requested. ");

	httpd_resp_set_type(req, 'text/html');
	httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);

	return ESP_OK;
}

/* Sends the app.css page
 * @param req HTTP request for which the url needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_app_css_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.css Requested. ");

	httpd_resp_set_type(req, 'text/css');
	httpd_resp_send(req, (const char *)app_css_start, app_css_end - app_css_start);

	return ESP_OK;
}

/* Sends the app.js page
 * @param req HTTP request for which the url needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_app_js_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "app.js Requested. ");

	httpd_resp_set_type(req, 'application/javascript');
	httpd_resp_send(req, (const char *)app_js_start, app_js_end - app_js_start);

	return ESP_OK;
}

/* Sends the favicon.ico file when accessing the webpage
 * @param req HTTP request for which the url needs to be handled
 * @return ESP_OK
 */
static esp_err_t http_server_favicon_ico_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "favicon.ico Requested. ");

	httpd_resp_set_type(req, 'image/x-icon');
	httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start);

	return ESP_OK;
}

/*
 * sets up the default httpd_server configuration
 * @return http server instance handle if successful, null if otherwise
 */
static httpd_handle_t http_server_configure(void)
{
	// Generate the default configuration
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();   // instance of the handle

	// create HTTP server monitor task
	xTaskCreatePinnedToCore(&http_server_monitor,
			"http_server_monitor", HTTP_SERVER_MONITOR_STACK_SIZE,
			NULL, HTTP_SERVER_MONITOR_PRIORITY,
			&task_http_server_monitor, HTTP_SERVER_MONITOR_CORE_ID);

	//create the message queue
	http_server_monitor_queue_handle = xQueueCreate(3, sizeof(http_server_queue_message_t));

	// The core that the HTTP server will run on
	config.core_id = HTTP_SERVER_TASK_CORE_ID;

	// Adjust the default priority to 1 less than the WIFI application task
	config.task_priority = HTTP_SERVER_TASK_PRIORITY;

	// Bump up the task size default is 4096
	config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

	// Increase uri handles
	config.max_uri_handlers = 20;

	// Increase the timeouts limits
	config.recv_wait_timeout = 10;
	config.send_wait_timeout = 10;

	ESP_LOGI(TAG,
			"http_server_configure: Starting Server at port: '%d' with task priority: '%d'",
			config.server_port,
			config.task_priority);

	// Start the httpd_server
	if (httpd_start(&http_server_handle, &config) == ESP_OK)
	{
		ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

		// register jquery handler
		httpd_uri_t jquery_js ={
					.uri = "/jquery-3.3.1.min.js",
					.method = HTTP_GET,
					.handler = http_server_jquery_handler,
					.user_ctx = NULL
			};
		httpd_register_uri_handler(http_server_handle, &jquery_js);

		// index.html handler
		httpd_uri_t index_html ={
				.uri = "/",
				.method = HTTP_GET,
				.handler = http_server_index_html_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &index_html);

		// app.css handler
		httpd_uri_t app_css ={
				.uri = "/app.css",
				.method = HTTP_GET,
				.handler = http_server_app_css_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &app_css);

		// app.js handler
		httpd_uri_t app_js ={
				.uri = "/app.js",
				.method = HTTP_GET,
				.handler = http_server_app_js_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &app_js);

		// favucon.ico handler
		httpd_uri_t favicon_ico ={
				.uri = "/favicon.ico",
				.method = HTTP_GET,
				.handler = http_server_favicon_ico_handler,
				.user_ctx = NULL
		};
		httpd_register_uri_handler(http_server_handle, &favicon_ico);

		return http_server_handle;
	}

	return NULL;
}

/*
 *prototype to Starts HTTP server
 */
void http_server_start(void){
	if (http_server_handle == NULL)
	{
		http_server_handle = http_server_configure();
	}
}

/*
 * prototype to stop the http server
 */
void http_server_stop(void){
	if (http_server_handle)
	{
		httpd_stop(http_server_handle);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP Server");
		http_server_handle = NULL;

	}
	if (task_http_server_monitor)
	{
		vTaskDelete(task_http_server_monitor);
		ESP_LOGI(TAG, "http_server_stop: stopping HTTP server monitor");
		task_http_server_monitor = NULL;
	}
}

BaseType_t http_server_monitor_send_message(http_server_message_e msgID)
{
	http_server_queue_message_t msg;
	msg.msgID = msgID;
	return xQueueSend(http_server_monitor_queue_handle, &msg, portMAX_DELAY);
}

