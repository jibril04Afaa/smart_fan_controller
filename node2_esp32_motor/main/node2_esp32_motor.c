#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

// define UART pins
#define UART_PORT_NUM UART_NUM_1 // (USART1 in stm32cubeIDE)
#define UART_TX 17 // GPIOP17
#define UART_RX 16 // GPIOP16
#define UART_BAUD_RATE 9600 // UART must be the same baud rate
#define UART_BUFSIZE 1024

// define motor pins
#define MOTOR_PWM_PIN 18 // PWMA
#define MOTOR_DIR_PIN 19 // AIN1

/* i'm using void parameters for void functions because esp-idf 
runs on C11(GNU11). fun fact - i use C23(GNU23), so this was
very good to know
*/

void init_uart(void)
{
    // configure UART communication parameters
    uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    // apply config to UART port 2
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));

    // set GPIO pins (TX->17, RX->16) 
    /* we ignore UART flow control since pins 18/19 will be used
    by my motor pwm and direction. it is also unecessary for the stm32,
    since it will be sending string messages 
    */
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // install UART drivers with ring buffer
    /* UART port number
    Size of RX ring buffer
    Size of TX ring buffer
    Event queue size
    Pointer to store the event queue handle
    Flags to allocate an interrupt*/
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUFSIZE*2, 0, 0, NULL, 0));
}

void init_motor(void)
{
    // configure the Direction Pin (AIN1) as a standard GPIO output
    gpio_reset_pin(MOTOR_DIR_PIN);
    gpio_set_direction(MOTOR_DIR_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MOTOR_DIR_PIN, 1); // set to 1, i.e, forward, AIN2 is hardwired to GND.

    // 2. Configure the PWM Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_8_BIT, // 8-bit resolution (0-255 speed)
        .freq_hz          = 5000,             // 5 kHz PWM frequency
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // configure the PWM Channel attached to GPIO 18
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_PWM_PIN,
        .duty           = 0, // Start with motor off (0 duty cycle)
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}


// FreeRTOS task to continuously listen for data
void rx_task(void* arg)
{
    uint8_t incoming_data[128];
    
    while (1)
    {
        // check ring buffer for new bytes
        int length = uart_read_bytes(UART_PORT_NUM, incoming_data, sizeof(incoming_data) - 1, 20/portTICK_PERIOD_MS);
    
        if(length > 0)
        {
            // null-terminate the data, we need to read it as a string (char*)
            incoming_data[length] = '\0';
            printf("Received from STM32: %s\n", (char*)incoming_data);

            // if STM32 sends "SPIN", set PWM duty cycle to 50% (128 out of 255)
            if (strstr((char*)incoming_data, "SPIN") != NULL) {
                printf("Spin command received! Starting motor...\n");
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 128); 
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            }
            // if STM32 sends "STOP", set PWM duty cycle to 0
            else if (strstr((char*)incoming_data, "STOP") != NULL) {
                printf("Stop command received! Halting motor...\n");
                ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0); 
                ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
            }
        }
    }
    
}

void app_main(void)
{
    printf("Init UART bus... \n");
    init_uart();
    printf("UART listening on GPIO 16 (receiving wire) \n");

    printf("Init Motor drivers... \n");
    init_motor();
    printf("Motor ready on GPIO 18 (PWM) and GPIO 19 (DIR)\n");

        
    // listener task
    xTaskCreate(rx_task, "UART_RX_TASK", 2048, NULL, 10, NULL);


    /* loopback test cos the UART is BUGGING! */
    // const char* test_msg = "ESP32 loopback success";
    // while (1)
    // {
    //     uart_write_bytes(UART_PORT_NUM, test_msg, strlen(test_msg));
    //     vTaskDelay(1000/portTICK_PERIOD_MS);
    // }
    

}
