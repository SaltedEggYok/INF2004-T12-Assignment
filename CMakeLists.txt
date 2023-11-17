set(PROGRAM_NAME main)
#${PROGRAM_NAME}

if (TARGET tinyusb_device)
        add_executable(${PROGRAM_NAME}
                main.c 
                util/irsensor/sensor.c 
                util/magnetometer/magnetometer.c 
                util/map/map.c 
                util/motor_controller/motor_controller.c 
                util/ultrasonic/ultrasonic.c 
                util/wheel_encoder/wheel_encoder.c 
                util/wifi/wifi.c
                util/pid_controller/pid_controller.c
                )
        
        #add .h files
        # Create an INTERFACE library
        add_library(main_lib INTERFACE)

        # Add include directories for the headers
        target_include_directories(main_lib INTERFACE
                util/irsensor
                util/magnetometer
                util/map
                util/motor_controller
                util/ultrasonic
                util/wheel_encoder
                util/wifi
                uill/pid_controller
        )
        # pull in common dependencies and additional pwm hardware support
        target_link_libraries(${PROGRAM_NAME}
                pico_stdlib              # for core functionality
                pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
                pico_cyw43_arch_lwip_threadsafe_background #from wifi
                hardware_adc
                hardware_gpio
                hardware_irq
                hardware_pwm
                hardware_timer
                hardware_i2c
                main_lib
                FreeRTOS-Kernel-Heap4 
                )
        
        # target_link_libraries(main 
        #         pico_stdlib 
        #         )
        # target_link_libraries(main pico_stdlib hardware_adc)
        # target_link_libraries(main pico_stdlib)

        # enable usb output, disable uart output
        pico_enable_stdio_usb(${PROGRAM_NAME} 1)
        pico_enable_stdio_uart(${PROGRAM_NAME} 0)

        # create map/bin/hex file etc.
        pico_add_extra_outputs(${PROGRAM_NAME})

        # add url via pico_set_program_url
        example_auto_set_url(${PROGRAM_NAME})

                
        target_include_directories(${PROGRAM_NAME} PRIVATE
                ${CMAKE_CURRENT_LIST_DIR}
                ${CMAKE_CURRENT_LIST_DIR}/util 
                ${CMAKE_CURRENT_LIST_DIR}/util/map
                ${CMAKE_CURRENT_LIST_DIR}/util/irsensor
                ${CMAKE_CURRENT_LIST_DIR}/util/magnetometer
                ${CMAKE_CURRENT_LIST_DIR}/util/motor_controller
                ${CMAKE_CURRENT_LIST_DIR}/util/ultrasonic
                ${CMAKE_CURRENT_LIST_DIR}/util/wheel_encoder
                ${CMAKE_CURRENT_LIST_DIR}/util/wifi	
                ${CMAKE_CURRENT_LIST_DIR}/util/pid_controller							
                ${PICO_LWIP_CONTRIB_PATH}/apps/ping
#                main INTERFACE ${CMAKE_CURRENT_LIST_DIR}
        )

elseif(PICO_ON_DEVICE)
        message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

if (NOT PICO_NO_HARDWARE)
    add_subdirectory(util)
endif ()
