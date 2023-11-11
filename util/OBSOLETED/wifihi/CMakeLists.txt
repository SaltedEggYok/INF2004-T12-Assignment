# cmake_minimum_required(VERSION 3.18)

# find_package(Perl)
# if(NOT PERL_FOUND)
#     message(FATAL_ERROR "Perl is needed for generating the fsdata.c file")
# endif()

# set(PICO_BOARD pico_w)
# include(cmake/utils.cmake)

# get_pico_sdk_import_cmake()
# # initialize pico-sdk from GIT
# # (note this can come from environment, CMake cache etc)
# set(PICO_SDK_FETCH_FROM_GIT on)
# # pico_sdk_import.cmake is a single file copied from this SDK
# # note: this must happen before project()
# include(cmake/pico_sdk_import.cmake)


# if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/credentials.cmake)
#     # copy it over from cmake/credentials.cmake.example
#     include(cmake/credentials.cmake)
# else()
#     message(WARNING "Credentials file not found, using default values!")
#     set(WIFI_SSID "SSID" CACHE INTERNAL "WiFi SSID for examples")
#     set(WIFI_PASSWORD "password" CACHE INTERNAL "WiFi password for examples")
# endif()

# project(pico_w_webserver C CXX ASM)
# set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 17)

# add_compile_options(
#         -Wall
#         -Werror
#         )


# # initialize the Raspberry Pi Pico SDK
# pico_sdk_init()


set(PROGRAM_NAME pico_w_webserver_test2)

#set(MAKE_FS_DATA_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/external/makefsdata)

# if (NOT EXISTS ${MAKE_FS_DATA_SCRIPT})
#         file(DOWNLOAD
#                 https://raw.githubusercontent.com/krzmaz/lwip/e15654409d14a238aec5ed4bd5516063938c9345/src/apps/http/makefsdata/makefsdata
#                 ${MAKE_FS_DATA_SCRIPT}
#                 )
# endif()
# message("Running makefsdata script")
# execute_process(COMMAND
#         perl ${MAKE_FS_DATA_SCRIPT}
#         WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
#         ECHO_OUTPUT_VARIABLE
#         ECHO_ERROR_VARIABLE
#         )
#file(RENAME external/fsdata.c external/my_fsdata.c)

# add_executable(${PROGRAM_NAME}
#         main.cpp
#         ssi.cpp
#         cgi.cpp
#         )

add_executable(${PROGRAM_NAME}
        testmain2.c
        )

target_compile_definitions(${PROGRAM_NAME} PRIVATE
        WIFI_SSID=\"${WIFI_SSID}\"
        WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
        NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
        LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
        PING_USE_SOCKETS=1
        )
target_include_directories(${PROGRAM_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        )
# target_link_libraries(${PROGRAM_NAME}
#         pico_cyw43_arch_lwip_threadsafe_background
#         #pico_cyw43_arch_lwip_sys_freertos
#         pico_stdlib
#         #pico_lwip_iperf
#         #FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
#         pico_lwip_http
#         hardware_adc
#         )
# target_link_libraries(${PROGRAM_NAME}
#         pico_cyw43_arch_lwip_threadsafe_background
#         pico_lwip_http
#         pico_stdlib
#         hardware_adc
#         )
target_link_libraries(${PROGRAM_NAME}
        #pico_cyw43_arch_lwip_threadsafe_background
        pico_cyw43_arch_lwip_sys_freertos
        pico_stdlib
        #pico_lwip_iperf
        pico_lwip_http
        FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
)

pico_enable_stdio_usb(${PROGRAM_NAME} TRUE)
pico_enable_stdio_uart(${PROGRAM_NAME} TRUE)
#suppress_pico_sdk_warnings()

pico_add_extra_outputs(${PROGRAM_NAME})
