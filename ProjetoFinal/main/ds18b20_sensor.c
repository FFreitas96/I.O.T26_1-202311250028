#include "ds18b20_sensor.h"

#include "ds18b20.h"
#include "esp_log.h"
#include "onewire_bus.h"

static const char *TAG = "DS18B20";

static ds18b20_device_handle_t s_ds18b20 = NULL;

esp_err_t ds18b20_sensor_init(int gpio_num)
{
    onewire_bus_handle_t bus = NULL;
    onewire_bus_config_t bus_config = {
        .bus_gpio_num = gpio_num,
    };
    onewire_bus_rmt_config_t rmt_config = {
        .max_rx_bytes = 10, /* 1 byte comando ROM + 8 bytes numero ROM + 1 byte comando device */
    };

    esp_err_t err = onewire_new_bus_rmt(&bus_config, &rmt_config, &bus);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar barramento 1-Wire no GPIO %d: %s", gpio_num, esp_err_to_name(err));
        return err;
    }

    /* Assume um unico DS18B20 no barramento, sem precisar de varredura de enderecos */
    ds18b20_config_t ds_cfg = {};
    err = ds18b20_new_device_from_bus(bus, &ds_cfg, &s_ds18b20);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Nenhum DS18B20 encontrado no GPIO %d (cheque fiacao/pull-up): %s", gpio_num,
                  esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "DS18B20 encontrado no GPIO %d", gpio_num);
    return ESP_OK;
}

esp_err_t ds18b20_sensor_read(float *temperature_c)
{
    if (s_ds18b20 == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    /* Essa funcao ja aguarda internamente o tempo de conversao do sensor */
    esp_err_t err = ds18b20_trigger_temperature_conversion(s_ds18b20);
    if (err != ESP_OK) {
        return err;
    }

    return ds18b20_get_temperature(s_ds18b20, temperature_c);
}
