# #empty
# #map.c
# set(PROGRAM_NAME map)

# add_executable(${PROGRAM_NAME}
#         map.c
#         )

# target_include_directories(${PROGRAM_NAME} PRIVATE
#         ${CMAKE_CURRENT_LIST_DIR}
#         pico_stdlib
#         )

        
# target_link_libraries(${PROGRAM_NAME} 
#         pico_stdlib
#         )

# pico_enable_stdio_usb(${PROGRAM_NAME} 1)
# #pico_enable_stdio_uart(${PROGRAM_NAME} 0)
# #suppress_pico_sdk_warnings()

# pico_add_extra_outputs(${PROGRAM_NAME})