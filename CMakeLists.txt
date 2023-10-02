if (TARGET tinyusb_device)
        add_executable(main
                main.c
                )
        target_link_libraries(main
                pico_stdlib              # for core functionality
                pico_cyw43_arch_none     # we need Wifi to access the GPIO, but we don't need anything else
                )
        # pull in common dependencies and additional pwm hardware support
        target_link_libraries(main pico_stdlib hardware_pwm)
        target_link_libraries(main pico_stdlib hardware_adc)
        target_link_libraries(main pico_stdlib)

        # enable usb output, disable uart output
        pico_enable_stdio_usb(main 1)
        pico_enable_stdio_uart(main 0)

        # create map/bin/hex file etc.
        pico_add_extra_outputs(main)

        # add url via pico_set_program_url
        example_auto_set_url(main)

elseif(PICO_ON_DEVICE)
        message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()

if (NOT PICO_NO_HARDWARE)
    add_subdirectory(util)
endif ()
