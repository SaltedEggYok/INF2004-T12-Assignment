# add_executable(pid_controller
# pid_controller.c
#         )

# # pull in common dependencies
# target_link_libraries(pid_controller pico_stdlib)
# target_link_libraries(pid_controller pico_stdlib hardware_pwm)
# target_link_libraries(pid_controller pico_stdlib hardware_adc)
# target_link_libraries(pid_controller pico_stdlib hardware_rtc)

# pico_enable_stdio_usb(pid_controller 1)
# pico_enable_stdio_uart(pid_controller 0)

# # create map/bin/hex file etc.
# pico_add_extra_outputs(pid_controller)

# # add url via pico_set_program_url
# example_auto_set_url(pid_controller)
