#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"

#include "ssd1306.h"
#include "pico-button/button.c"

#include "blink.pio.h"
#include "quadrature_encoder.pio.h"




int main() {
    stdio_init_all();

    //
    // Encoder PIO
    //
    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB = 10;
    PIO pio_encoder = pio0;
    const uint sm_encoder = 0;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    pio_add_program(pio_encoder, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio_encoder, sm_encoder, PIN_AB, 0);


    //
    // Stepper PIO
    //
    static const uint step_pin = 25;
    static const uint dir_pin = 14;
    gpio_init(dir_pin);
    gpio_set_dir(dir_pin, GPIO_OUT);

    // for visual testing
    //static const float pio_freq = 2000;
    static const float pio_freq = 0; // default 125MHz
    uint32_t loop_count = 195;
    uint32_t step_count = 5;

    // Choose PIO instance (0 or 1)
    PIO pio_stepper = pio1;

    // Get first free state machine in PIO 0
    uint sm_stepper = pio_claim_unused_sm(pio_stepper, true);

    // Add PIO program to PIO instruction memory. SDK will find location and
    // return with the memory offset of the program.
    uint offset = pio_add_program(pio_stepper, &blink_program);

    // Calculate the PIO clock divider
    float div = (float)clock_get_hz(clk_sys) / pio_freq;

    // Initialize the program using the helper function in our .pio file
    blink_program_init(pio_stepper, sm_stepper, offset, step_pin, div);

    // Start running our PIO program in the state machine
    pio_sm_set_enabled(pio_stepper, sm_stepper, true);

    // tell the sm how many times to loop
    pio_sm_put_blocking(pio_stepper, sm_stepper, loop_count);

    //
    // OLED display
    //
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);

    ssd1306_t disp;
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);

    ssd1306_draw_string(&disp, 8, 24, 2, "hello");
    ssd1306_show(&disp);


    //
    // button handling
    //
    uint32_t press_count = 0;
    char press_count_string[10];
    void update_display () {
        sprintf(press_count_string, "%u", press_count);
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 8, 24, 2, press_count_string);
        ssd1306_show(&disp);
    }

    void onchange(button_t *button_p) {
        button_t *button = (button_t*)button_p;
        printf("Button on pin %d changed state to %d\n", button->pin, button->state);
        if (button->state == 0) {
            press_count++;
            printf("press count %d\n", press_count);
            update_display();
            pio_sm_put_blocking(pio_stepper, sm_stepper, 148);
        }
    }

    // button
    button_t *b = create_button(22, onchange);


    // handle direction when following encoder
    void move_stepper (int steps_to_go) {
        if (steps_to_go > 0) {
            gpio_put(dir_pin, 0);
            pio_sm_put_blocking(pio_stepper, sm_stepper, steps_to_go);
        }
        if (steps_to_go < 0) {
            gpio_put(dir_pin, 1);
            pio_sm_put_blocking(pio_stepper, sm_stepper, abs(steps_to_go));
        }
    }

    int current_stepper_step = 0;
    int current_encoder_step = 0; // this probably needs to be
    int steps_to_go = 0;
    // Do nothing
    while (true) {
        // monitor PIO encoder and send steps when needed
        current_encoder_step = quadrature_encoder_get_count(pio_encoder, sm_encoder);
        steps_to_go = current_stepper_step - current_encoder_step;
        if (steps_to_go != 0) {
            move_stepper(steps_to_go);
            current_stepper_step = current_encoder_step;
            //printf("enc: %d, step: %d\n", current_encoder_step, current_stepper_step);
        }
        sleep_us(1);
    }
}
