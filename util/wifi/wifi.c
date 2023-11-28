/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/tcp.h"

#include "wifi.h"


int attempts = 0;
bool connected = false;

/*
    @brief: initialize the TCP server
    @param: None
    @return: state:success
*/
static TCP_SERVER_T* tcp_server_init(void) {
    TCP_SERVER_T *state = calloc(1, sizeof(TCP_SERVER_T));
    if (!state) {
        DEBUG_printf("failed to allocate state\n");
        return NULL;
    }
    return state;
}

/*
    @brief: Close the TCP server
    @param: arg
    @return: err:success
*/
static err_t tcp_server_close(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    err_t err = ERR_OK;
    if (state->client_pcb != NULL) {
        tcp_arg(state->client_pcb, NULL);
        tcp_poll(state->client_pcb, NULL, 0);
        tcp_sent(state->client_pcb, NULL);
        tcp_recv(state->client_pcb, NULL);
        tcp_err(state->client_pcb, NULL);
        err = tcp_close(state->client_pcb);
        if (err != ERR_OK) {
            DEBUG_printf("close failed %d, calling abort\n", err);
            tcp_abort(state->client_pcb);
            err = ERR_ABRT;
        }
        state->client_pcb = NULL;
    }
    if (state->server_pcb) {
        tcp_arg(state->server_pcb, NULL);
        tcp_close(state->server_pcb);
        state->server_pcb = NULL;
    }
    return err;
}

/*
    @brief: get the result of the TCP server
    @param: *arg: pointer to the arg variable
            status: status of the server
    @return: ERR_OK:success
*/
static err_t tcp_server_result(void *arg, int status) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (status == 0) {
        DEBUG_printf("test success\n");
    } else {
//        DEBUG_printf("test failed %d\n", status);
    }
    state->complete = false;
//    return tcp_server_close(arg);
    return ERR_OK;
}

/*
    @brief: check if message sent from tcp server
    @param: *arg: pointer to the arg variable
            *tpcb: pointer to the tpcb variable
             len: length of the buffer 
    @return: ERR_OK:success
*/
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("tcp_server_sent %u\n", len);
    state->sent_len += len;

    if (state->sent_len >= BUF_SIZE) {

        // We should get the data back from the client
        state->recv_len = 0;
        DEBUG_printf("Waiting for buffer from client\n");
    }

    return ERR_OK;
}

/*
    @brief: send a message from the tcp server
    @param: *arg: pointer to the arg variable
            *tpcb: pointer to the tpcb variable
    @return: ERR_OK:success
*/
err_t tcp_server_send_data(void *arg, struct tcp_pcb *tpcb)
{
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    for(int i=0; i< BUF_SIZE; i++) {
        state->buffer_sent[i] = rand();
    }
    memcpy(state->buffer_sent, "test1234 hello1\r", BUF_SIZE);

    state->sent_len = 0;
    DEBUG_printf("Writing %ld bytes to client\n", BUF_SIZE);
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    err_t err = tcp_write(tpcb, state->buffer_sent, BUF_SIZE, TCP_WRITE_FLAG_COPY);
    if (err != ERR_OK) {
        DEBUG_printf("Failed to write data %d\n", err);
        return tcp_server_result(arg, -1);
    }
    return ERR_OK;
}

/*
    @brief: tcp server receives a message
    @param: *arg: pointer to the arg variable
            *tpcb: pointer to the tpcb variable
             err: status code of the error
    @return: tcp_server_result(arg,-1) : error
             ERR_OK : success
*/
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (!p) {
        return tcp_server_result(arg, -1);
    }
    // this method is callback from lwIP, so cyw43_arch_lwip_begin is not required, however you
    // can use this method to cause an assertion in debug mode, if this method is called when
    // cyw43_arch_lwip_begin IS needed
    cyw43_arch_lwip_check();
    if (p->tot_len > 0) {
        DEBUG_printf("tcp_server_recv %d/%d err %d\n", p->tot_len, state->recv_len, err);
        printf("%s\n", p->payload);

        // Receive the buffer
        const uint16_t buffer_left = BUF_SIZE - state->recv_len;
        state->recv_len += pbuf_copy_partial(p, state->buffer_recv + state->recv_len,
                                             p->tot_len > buffer_left ? buffer_left : p->tot_len, 0);
        tcp_recved(tpcb, p->tot_len);
    }
    pbuf_free(p);

    // Have we have received the whole buffer
    if (state->recv_len == BUF_SIZE) {

        // check it matches
        if (memcmp(state->buffer_sent, state->buffer_recv, BUF_SIZE) != 0) {
            DEBUG_printf("buffer mismatch\n");
            return tcp_server_result(arg, -1);
        }
        DEBUG_printf("tcp_server_recv buffer ok\n");

        // Test complete?
        state->run_count++;
        if (state->run_count >= TEST_ITERATIONS) {
            tcp_server_result(arg, 0);
            return ERR_OK;
        }

        // Send another buffer
        return tcp_server_send_data(arg, state->client_pcb);
    }
    return ERR_OK;
}

/*
    @brief: poll to check the tcp server
    @param: *arg: pointer to the arg variable
            *tpcb: pointer to the tpcb variable
    @return: tcp_server_result(arg,-1) : error
             
*/
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb) {
    //DEBUG_printf("tcp_server_poll_fn\n");
    return tcp_server_result(arg, -1); // no response is an error?
}

/*
    @brief: check the tcp server's error
    @param: *arg: pointer to the arg variable
            err: status code of the error
    @return: None     
*/
static void tcp_server_err(void *arg, err_t err) {
    if (err != ERR_ABRT) {
        DEBUG_printf("tcp_client_err_fn %d\n", err);
        tcp_server_result(arg, err);
    }
}

/*
    @brief: accept the tcp server's connection
    @param: *arg: pointer to the arg variable
            *client_pcb: pointer to the client_pcb variable
            err: status code of the error
    @return: None     
*/
static err_t tcp_server_accept(void *arg, struct tcp_pcb *client_pcb, err_t err) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    if (err != ERR_OK || client_pcb == NULL) {
        DEBUG_printf("Failure in accept\n");
        tcp_server_result(arg, err);
        return ERR_VAL;
    }
    DEBUG_printf("Client connected\n");

    state->client_pcb = client_pcb;
    tcp_arg(client_pcb, state);
    tcp_sent(client_pcb, tcp_server_sent);
    tcp_recv(client_pcb, tcp_server_recv);
    tcp_poll(client_pcb, tcp_server_poll, POLL_TIME_S * 2);
    tcp_err(client_pcb, tcp_server_err);

    return tcp_server_send_data(arg, state->client_pcb);
}

/*
    @brief: open the tcp server
    @param: *arg: pointer to the arg variable
    @return: None     
*/
static bool tcp_server_open(void *arg) {
    TCP_SERVER_T *state = (TCP_SERVER_T*)arg;
    DEBUG_printf("Starting server at %s on port %u\n", ip4addr_ntoa(netif_ip4_addr(netif_list)), TCP_PORT);

    struct tcp_pcb *pcb = tcp_new_ip_type(IPADDR_TYPE_ANY);
    if (!pcb) {
        DEBUG_printf("failed to create pcb\n");
        return false;
    }

    err_t err = tcp_bind(pcb, NULL, TCP_PORT);
    if (err) {
        DEBUG_printf("failed to bind to port %u\n", TCP_PORT);
        return false;
    }

    state->server_pcb = tcp_listen_with_backlog(pcb, 1);
    if (!state->server_pcb) {
        DEBUG_printf("failed to listen\n");
        if (pcb) {
            tcp_close(pcb);
        }
        return false;
    }

    tcp_arg(state->server_pcb, state);
    tcp_accept(state->server_pcb, tcp_server_accept);

    return true;
}

/*
    @brief: test the tcp server
    @param: None
    @return: None     
*/
void run_tcp_server_test(void) {
    TCP_SERVER_T *state = tcp_server_init();
    if (!state) {
        return;
    }
    if (!tcp_server_open(state)) {
        tcp_server_result(state, -1);
        return;
    }
    while(!state->complete) {
        // the following #ifdef is only here so this same example can be used in multiple modes;
        // you do not need it in your code
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer) to check for Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        // you can poll as often as you like, however if you have nothing else to do you can
        // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
        cyw43_arch_wait_for_work_until(make_timeout_time_ms(1000));
#else
        // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(1000);
#endif
    }
    free(state);
}

/*
    @brief: attempt to conenct to the tcp server
    @param: None
    @return: 0: success 
             1: error     
*/
int attemptConnection(){
    printf("Connecting to Wi-Fi...\n");
    do {
        //if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 15000)) {
        if (cyw43_arch_wifi_connect_timeout_ms("Xlenahc", "PengyonStore", CYW43_AUTH_WPA2_AES_PSK, 30000)) {
            //printf("%s", WIFI_SSID);
            //printf("%s", WIFI_PASSWORD);
            ++attempts;
            printf("failed to connect. %d time(s). \n", attempts);
            if(attempts >= 10){
                printf("failed to connect 10 times, exiting...\n");
                return 1; 
            }
            printf("Retrying...\n");

            //return 1;
        } else {
            printf("Connected.\n");
            connected = true;;
        }
    } while (!connected);

    return 0;
}

/*
    @brief: changes the state of a boolean passed in, main.h will use to to allow or disallow wifi 
    @param: *wifiEnabled: pointer to the flag which checks if wifi is enabled
    @return: None     
*/
void initWifi(bool *wifiEnabled){

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        *wifiEnabled = false;
        return;
    }

    cyw43_arch_enable_sta_mode();

    if (attemptConnection()){ //1 - error, 0 - ok
        *wifiEnabled = false;
        return;
    }

    run_tcp_server_test();

    *wifiEnabled = true;
    return;
}
