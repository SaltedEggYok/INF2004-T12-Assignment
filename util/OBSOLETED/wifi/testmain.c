// for httpd_init
#include "lwip/apps/httpd.h"
// for cy43_* functions
#include "pico/cyw43_arch.h"
// for #define's that configure lwIP stack (enable what we need and not more)
#include "lwipopts.h"


int main() {
    // needed to get the RP2040 chip talking with the wireless module
    if (cyw43_arch_init()) {
        return 1;
    }
    // we'll be connecting to an access point, not creating one
    cyw43_arch_enable_sta_mode();
    // WiFi credentials are taken from cmake/credentials.cmake
    // create it based on cmake/credentials.cmake.example if you haven't already!
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        return 1;
    }
    // let lwIP do it's magic
    httpd_init();
    // loop forever
    for (;;) {}
}