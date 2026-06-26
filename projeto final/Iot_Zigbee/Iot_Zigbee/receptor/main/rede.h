#pragma once

/* ── Wi-Fi ──────────────────────────────────────────────────────────────── */
#define WIFI_SSID        "MERCUSYS_7E02"
#define WIFI_PASSWORD    "70960594"
#define WIFI_MAX_RETRIES  10

/* ── MQTT ───────────────────────────────────────────────────────────────── */
#define MQTT_BROKER_URI "mqtt://192.168.1.105" /* <- altere para o IP da BBB */
#define MQTT_PORT         1883
#define MQTT_TOPIC_TEMP  "zigbee/temperatura"      /* <- altere se quiser */
#define MQTT_CLIENT_ID   "esp32c6_coordinator"

/* Inicializa o Wi-Fi (STA); ao conectar, sobe o cliente MQTT automaticamente */
void rede_iniciar(void);

/* Publica a temperatura (graus C) em MQTT_TOPIC_TEMP, se o MQTT estiver conectado */
void rede_publicar_temperatura(float temperatura_c);

