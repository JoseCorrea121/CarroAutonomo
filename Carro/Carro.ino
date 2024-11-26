#include <WiFi.h>
#include "esp_http_server.h"

IPAddress ip_local(192, 168, 137, 14);
IPAddress gateway(192, 168, 137, 1);
IPAddress subnet(255, 255, 255, 0);

int decision = 0;

const char* ssid = "cambio";
const char* pass = "12345678";
int status = WL_IDLE_STATUS;

httpd_handle_t carro_httpd = NULL; // server
httpd_config_t config = HTTPD_DEFAULT_CONFIG();

static esp_err_t capture_handler(httpd_req_t *req) {
  esp_err_t res = ESP_OK;
  char *buf = NULL;
  size_t buf_len = httpd_req_get_url_query_len(req) + 1;

  if (buf_len > 1) {
    buf = (char *)malloc(buf_len);
    
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      char *param = strtok(buf, "=");

      Serial.print("param: ");
      Serial.println(param);

      if (strcmp("0", param) == 0) {
        Serial.println("ENTRO1");
        decision = 0;
      } else if (strcmp("1", param) == 0) {
        Serial.println("ENTRO2");
        decision = 1;
      }
    }
  }

  httpd_resp_set_hdr(req, "Content-Disposition", "inline");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  const char *resp = "URI GET Response";
  httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
  return ESP_OK;
}

void setup() {
  Serial.begin(115200);

  if (!WiFi.config(ip_local, gateway, subnet)) {
    Serial.println("Error en configuracion WIFI");
  }

  while (WiFi.status() != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(500);
  }

  Serial.print("ip: ");
  Serial.println(WiFi.localIP());

  httpd_uri_t capture_uri = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = capture_handler,
    .user_ctx = NULL
  };

  if (httpd_start(&carro_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(carro_httpd, &capture_uri);
  }

  log_i("Starting stream server on port: '%d'", config.server_port);

  pinMode(12, OUTPUT);
}

void loop() {
  if (decision) {
    digitalWrite(12, HIGH);
  } else {
    digitalWrite(12, LOW);
  }
}
