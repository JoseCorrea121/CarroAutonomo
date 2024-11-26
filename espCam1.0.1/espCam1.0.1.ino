#include <WiFi.h>
#include "esp_camera.h"
#include "esp_http_server.h"

#include "camera_pins.h"

IPAddress ip_local(192, 168, 137, 13);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

const char* ssid = "cambio";
const char* pass = "12345678";
int status = WL_IDLE_STATUS;

httpd_handle_t camera_httpd = NULL; // server
httpd_config_t config = HTTPD_DEFAULT_CONFIG();

static esp_err_t capture_handler(httpd_req_t *req) {
  camera_fb_t *fb = esp_camera_fb_get();
  esp_err_t res = ESP_OK;

  if (!fb) {
    log_e("Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Content-Disposition", "inline; filename=capture.jpg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  char ts[32];
  snprintf(ts, 32, "%lld.%06ld", fb->timestamp.tv_sec, fb->timestamp.tv_usec);
  httpd_resp_set_hdr(req, "X-Timestamp", (const char *)ts);

  size_t fb_len = 0;

  if (fb->format == PIXFORMAT_JPEG) {
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
  }

  esp_camera_fb_return(fb);

  return res;
}

void setup() {
  Serial.begin(115200);

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  if (!WiFi.config(ip_local, gateway, subnet)) {
    Serial.println("Error en configuracion WIFI");
  }

  while (WiFi.status() != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(500);
  }

  Serial.print("ip: ");
  Serial.println(WiFi.localIP());

  // SERVIDOR
  httpd_uri_t capture_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &capture_uri);
  }

  log_i("Starting stream server on port: '%d'", config.server_port);

  // CAMARA
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG; // for streaming
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  const esp_err_t init = esp_camera_init(&config);
  if (init != ESP_OK) {
    Serial.print("error en inicializacion: ");
    Serial.println(init);
  }
}

void loop() {
}
