#pragma once

#include "esp_err.h"

/* Procura um DS18B20 no barramento 1-Wire ligado em gpio_num e prepara para leitura */
esp_err_t ds18b20_sensor_init(int gpio_num);

/* Dispara uma conversao e bloqueia (~750ms) até o resultado sair */
esp_err_t ds18b20_sensor_read(float *temperature_c);
