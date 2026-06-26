#pragma once

#include "esp_zigbee_core.h"

#define INSTALLCODE_POLICY_ENABLE   false
#define MAX_CHILDREN                (10)
#define HA_RECEPTOR_ENDPOINT        (1)
#define ESP_ZB_PRIMARY_CHANNEL_MASK ESP_ZB_TRANSCEIVER_ALL_CHANNELS_MASK

/* Precisa bater com o endpoint usado pelo transmissor (main/transmissor.h) */
#define HA_ESP_SENSOR_ENDPOINT (10)

#define LEITURA_TEMPERATURA_INTERVAL_S (1)

#define ESP_ZB_ZC_CONFIG()                                \
    {                                                       \
        .esp_zb_role = ESP_ZB_DEVICE_TYPE_COORDINATOR,       \
        .install_code_policy = INSTALLCODE_POLICY_ENABLE,    \
        .nwk_cfg.zczr_cfg = {                                 \
            .max_children = MAX_CHILDREN,                    \
        },                                                    \
    }

#define ESP_ZB_DEFAULT_RADIO_CONFIG()      \
    {                                       \
        .radio_mode = ZB_RADIO_MODE_NATIVE, \
    }

#define ESP_ZB_DEFAULT_HOST_CONFIG()                    \
    {                                                     \
        .host_connection_mode = ZB_HOST_CONNECTION_MODE_NONE, \
    }

/* Forma a rede Zigbee e fica lendo periodicamente a temperatura do sensor que entrar nela */
void receptor_start(void);
