add_library(misc INTERFACE)

target_sources(misc INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/argv.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/assert.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/buf_log.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/dma_x.c
    ${CMAKE_CURRENT_LIST_DIR}/src/dump.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/i2c_x.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pretty_io.c
    ${CMAKE_CURRENT_LIST_DIR}/src/pwm_x.c
    ${CMAKE_CURRENT_LIST_DIR}/src/str_ops.c
    ${CMAKE_CURRENT_LIST_DIR}/src/sys_led.cpp
)

target_include_directories(misc INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(misc INTERFACE
    pico_stdlib     # (all)
    pico_status_led # sys_led
    hardware_dma    # dma_irq_mux
    hardware_gpio   # dbg_gpio, sys_led
    hardware_i2c    # i2c_x
    hardware_irq    # pwm_x
    hardware_pwm    # pwm_x
    hardware_timer  # sys_led
)

add_subdirectory(test)
