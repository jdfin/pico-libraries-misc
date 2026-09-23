
add_compile_options(-Wall -Wextra -Werror)

add_executable(pwm_measure
    pwm_measure.cpp
)

pico_enable_stdio_uart(pwm_measure 0)
pico_enable_stdio_usb(pwm_measure 1)

target_link_libraries(pwm_measure PRIVATE
    pico_stdlib
    pico_stdio_usb
    misc
)

pico_add_extra_outputs(pwm_measure)
