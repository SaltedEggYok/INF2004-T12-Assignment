if (TARGET tinyusb_device)
    add_executable(irsensore2
    irsensore2.c
        )

    # pull in common dependencies
    target_link_libraries(irsensore2
        pico_stdlib
        pico_stdio
        hardware_adc
        hardware_gpio
        hardware_irq
        hardware_pwm
        hardware_timer
        hardware_i2c
    )

    # enable usb output, disable uart output
    pico_enable_stdio_usb(irsensore2 1)
    pico_enable_stdio_uart(irsensore2 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(irsensore2)

    # add url via pico_set_program_url
    example_auto_set_url(irsensore2)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building irSensore because TinyUSB submodule is not initialized in the SDK")
endif()