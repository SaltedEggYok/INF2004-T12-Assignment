add_executable(motor_controller
motor_controller.c
        )

# pull in common dependencies
target_link_libraries(motor_controller pico_stdlib)
target_link_libraries(motor_controller pico_stdlib hardware_pwm)
target_link_libraries(motor_controller pico_stdlib hardware_adc)
target_link_libraries(motor_controller pico_stdlib hardware_rtc)

pico_enable_stdio_usb(motor_controller 1)
pico_enable_stdio_uart(motor_controller 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(motor_controller)

# add url via pico_set_program_url
example_auto_set_url(motor_controller)
