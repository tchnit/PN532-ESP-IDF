
idf_component_register(
    SRCS "pn532.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer read_serial)
