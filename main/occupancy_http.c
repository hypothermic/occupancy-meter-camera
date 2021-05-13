#include "occupancy_http.h"

#include <sys/param.h>
#include <esp_log.h>
#include <esp_system.h>
#include <esp_camera.h>
#include <esp_netif.h>
#include <esp_http_server.h>

#include "occupancy_log.h"

#define JPG_QUALITY			80
#define SNPRINTF_BUF			80
#define MULTIPART_BOUNDARY_CONTENT	"boundary!"
#define MULTIPART_BOUNDARY_COMMAND	"\r\n" MULTIPART_BOUNDARY_CONTENT "\r\n"
#define MULTIPART_BOUNDARY_LENGTH	strlen("\r\n" MULTIPART_BOUNDARY_CONTENT "\r\n")

static esp_err_t handle_stream(httpd_req_t *request);

const httpd_uri_t stream_uri = {
	.uri		= "/stream",
	.method		= HTTP_GET,
	.handler	= handle_stream,
	.user_ctx	= NULL
};

static httpd_handle_t httpd = NULL;

void occupancy_http_start(void) {
	if (!httpd) {
		httpd_config_t config = HTTPD_DEFAULT_CONFIG();
		config.server_port = 81;

		if (httpd_start(&httpd, &config) == ESP_OK) {
			httpd_register_uri_handler(httpd, &stream_uri);
		} else {
			OCC_LOG_ERROR("Fout bij starten HTTP server")
		}
	}
}

void occupancy_http_stop(void) {
	if (httpd) {
		httpd_stop(httpd);
		httpd = NULL;
	}
}

static esp_err_t handle_stream(httpd_req_t *request) {
	esp_err_t	status			= ESP_OK;
	camera_fb_t	*buffer			= NULL;
	size_t		snprintf_length		= 0,
			jpg_length		= 0;
	uint8_t		*jpg_data		= NULL;
	char		*snprintf_buf[SNPRINTF_BUF];

	// CORS maakt niet uit, geen security risico
	ESP_ERROR_CHECK(httpd_resp_set_hdr(request, "Access-Control-Allow-Origin", "*"));
	// We moeten multipart gebruiken omdat we elke keer een nieuwe Content-Length willen sturen met de lengte van de jpeg data
	ESP_ERROR_CHECK(httpd_resp_set_type(request, "multipart/x-mixed-replace;boundary=" MULTIPART_BOUNDARY_CONTENT));

	// Blijf verwerken zo lang als er geen error optreedt
	while (status == ESP_OK) {
		// Verkrijg buffer memory adres van de camera
		buffer = esp_camera_fb_get();

		// Check of er een fout opgetreden is
		if (!buffer) {
			OCC_LOG_ERROR("Fout bij het ophalen van de frame buffer")
			status = ESP_FAIL;
		}

		// Als de buffer nog niet in JPEG formaat is, converteer deze dan
		if (buffer->format != PIXFORMAT_JPEG) {
			ESP_ERROR_CHECK(frame2jpg(buffer, JPG_QUALITY, &jpg_data, &jpg_length) ? ESP_OK : ESP_FAIL);
			// geef frame buffer terug
			esp_camera_fb_return(buffer);
			buffer = NULL;
		} else {
			jpg_length = buffer->len;
			jpg_data = buffer->buf;
			// geef hier de frame buffer nog niet terug, dit doen we pas na het verzenden van de data
		}

		// Verstuur boundary
		ESP_ERROR_CHECK(httpd_resp_send_chunk(request, MULTIPART_BOUNDARY_COMMAND, MULTIPART_BOUNDARY_LENGTH));

		// Verstuur content length header
		snprintf_length = snprintf((char *)snprintf_buf, SNPRINTF_BUF, "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", jpg_length);
		ESP_ERROR_CHECK(httpd_resp_send_chunk(request, (const char *)snprintf_buf, snprintf_length));

		// Stuur JPEG data
		ESP_ERROR_CHECK(httpd_resp_send_chunk(request, (const char *)jpg_data, jpg_length));

		// Als buffer niet geconverteerd was, staat deze nog open
		if (buffer) {
			esp_camera_fb_return(buffer);
			buffer = NULL;
			jpg_length = 0;
			jpg_data = NULL;
		}

		// Als conversie gelukt is dan staat het resultaat nog in jpg_data. free deze data.
		if (jpg_data) {
			free(jpg_data);
			jpg_data = NULL;
		}

		// Delay ff om ruimte te maken voor andere taken
		vTaskDelay(0);
	}

	return status;
}
