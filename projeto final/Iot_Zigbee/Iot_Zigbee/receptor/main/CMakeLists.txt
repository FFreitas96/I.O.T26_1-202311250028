idf_component_register(SRCS "rede.c" "main.c" "receptor.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_event esp_netif nvs_flash mqtt)
