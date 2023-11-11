// for httpd_init
#include "lwip/apps/httpd.h"
// for cy43_* functions
#include "pico/cyw43_arch.h"
// for #define's that configure lwIP stack (enable what we need and not more)
#include "lwipopts.h"

#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
int main() {
    
    stdio_init_all();
    // needed to get the RP2040 chip talking with the wireless module
    if (cyw43_arch_init()) {
        return 1;
    }
    // we'll be connecting to an access point, not creating one
    cyw43_arch_enable_sta_mode();
    // WiFi credentials are taken from cmake/credentials.cmake
    // create it based on cmake/credentials.cmake.example if you haven't already!
    sleep_ms(10000); 
    printf("Connecting to WiFi...\n");
    printf("%s\n", WIFI_SSID);
    printf("%s\n", WIFI_PASSWORD);

    if (cyw43_arch_wifi_connect_timeout_ms("Xlenahc", "PengyonStore", CYW43_AUTH_WPA2_AES_PSK, 15000)) {
        printf("failed to connect.\n");
        return 1;
    }
    printf("Connected.\n");

    
    // turn on LED to signal connected
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    // let lwIP do it's magic
    httpd_init();
    // loop forever
    for (;;) {}
}