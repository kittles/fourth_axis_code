#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

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
    // ENCODER PIO
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
    // STEPPER PIO
    //
    static const uint step_pin = 15;
    static const uint dir_pin = 14;
    // i guess the step pin cannot be shared with cpu,
    // so only access it through PIO
    //gpio_init(step_pin);
    //gpio_set_dir(step_pin, GPIO_OUT);
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
    // OLED DISPLAY
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
    // BUTTON HANDLING: step forward and backward by 1 degree increments
    //
    uint32_t old_degree_offset = 0;
    uint32_t degree_offset = 0;
    char press_count_string[10];
    void update_display () {
        sprintf(press_count_string, "%u", degree_offset);
        ssd1306_clear(&disp);
        ssd1306_draw_string(&disp, 8, 24, 2, press_count_string);
        ssd1306_show(&disp);
    }

    uint32_t old_steps_total = 0;
    uint32_t steps_total = 0;
    uint32_t degree_steps_to_go = 0;

    // handle button press
    void onchange(button_t *button_p) {
        button_t *button = (button_t*)button_p;
        if (button->state == 0) {
            old_degree_offset = degree_offset;
            if (button->pin == 22) {
                degree_offset++;
                gpio_put(dir_pin, 0);
            } else {
                degree_offset--;
                gpio_put(dir_pin, 1);
            }
            printf("press count %d\n", degree_offset);
            update_display();
            sleep_us(5); // give time for dir pin to take effect
            // determine steps need from degree offset
            // need degree_offset instead of direct stepping
            // because:
            // steps per 1 degree = 148.1481481
            // so error will accumulate
            old_steps_total = round(old_degree_offset * 148.148148148);
            steps_total = round(degree_offset * 148.148148148);
            degree_steps_to_go = abs(old_steps_total - steps_total);
            int i;
            for (i = 0; i < degree_steps_to_go; i++) {
                // have to use PIO to step because they cant share step pin
                // choose forward or reverse
                pio_sm_put_blocking(pio_stepper, sm_stepper, 1);
                sleep_us(5);
            }
        }
    }

    // make buttons
    button_t *b = create_button(22, onchange);
    button_t *bb = create_button(21, onchange);

    //
    // X-AXIS TRACKING: set up for left hand helix
    //
    void move_stepper (int steps_to_go) {
        if (steps_to_go > 0) {
            gpio_put(dir_pin, 1);
            pio_sm_put_blocking(pio_stepper, sm_stepper, steps_to_go);
        }
        if (steps_to_go < 0) {
            gpio_put(dir_pin, 0);
            pio_sm_put_blocking(pio_stepper, sm_stepper, abs(steps_to_go));
        }
    }

    int current_stepper_step = 0;
    int current_encoder_step = 0;
    int steps_to_go = 0;
    int step_multiple = 4; // should make a 2.67" pitch helix
    while (true) {
        // monitor PIO encoder and send steps when needed
        current_encoder_step = quadrature_encoder_get_count(pio_encoder, sm_encoder);
        steps_to_go = current_stepper_step - current_encoder_step;
        if (steps_to_go != 0) {
            move_stepper(steps_to_go * step_multiple);
            current_stepper_step = current_encoder_step;
            //printf("enc: %d, step: %d\n", current_encoder_step, current_stepper_step);
        }
        sleep_us(1);
    }
}
