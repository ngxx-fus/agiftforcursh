idf_component_register(
    SRCS
        "../lib/tft/tft.c"
        "../lib/tft/spi.c"
        "../lib/ctrl/ctrl.c"
        "agiftforcrush.c"
    
    INCLUDE_DIRS
        "../lib/tft"
        "../lib/ctrl"
        "../include"
        "."
)
