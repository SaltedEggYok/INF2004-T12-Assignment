# set(PROGRAM_NAME pico_w_webserver_final)

# add_executable(${PROGRAM_NAME}
#         wifi.c
#         )
# target_compile_definitions(${PROGRAM_NAME} PRIVATE
#         WIFI_SSID=\"${WIFI_SSID}\"
#         WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
#         # NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
#         # LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
#         PING_USE_SOCKETS=1

#         )
# target_include_directories(${PROGRAM_NAME} PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         #${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
#         ${PICO_LWIP_CONTRIB_PATH}/apps/ping
#         )
# target_link_libraries(${PROGRAM_NAME}
#         pico_cyw43_arch_lwip_threadsafe_background
#         pico_stdlib
#         )

# pico_add_extra_outputs(${PROGRAM_NAME})
# pico_enable_stdio_usb(${PROGRAM_NAME} 1) 

# # add_executable(picow_tcpip_server_poll
# #         picow_tcp_server.c
# #         )
# # target_compile_definitions(picow_tcpip_server_poll PRIVATE
# #         WIFI_SSID=\"${WIFI_SSID}\"
# #         WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
# #         # NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
# #         # LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
# #         PING_USE_SOCKETS=1
# #         )
# # target_include_directories(picow_tcpip_server_poll PRIVATE
# #         ${CMAKE_CURRENT_LIST_DIR}
# #         ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts
# #         ${PICO_LWIP_CONTRIB_PATH}/apps/ping
# #         )
# # target_link_libraries(picow_tcpip_server_poll
# #         pico_cyw43_arch_lwip_poll
# #         pico_stdlib
# #         )
# # pico_add_extra_outputs(picow_tcpip_server_poll)
# # pico_enable_stdio_usb(picow_tcpip_server_poll 1) 

