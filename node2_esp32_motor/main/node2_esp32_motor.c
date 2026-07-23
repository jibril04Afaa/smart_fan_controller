#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"

// define UART pins
#define UART_PORT_NUM UART_NUM_2 // (USART2 in stm32cubeIDE)
#define UART_TX 17 // GPIOP17
#define UART_RX 16 // GPIOP16
#define UART_BAUD_RATE 115200 // UART must be the same baud rate
#define UART_BUFSIZE 1024

/* i'm using void parameters for void functions because esp-idf 
runs on C11(GNU11). fun fact - i use C23(GNU23), so this was
very good to know
*/

init_uart(void)
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
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TX, UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE)));

    // install UART drivers with ring buffer
    /* UART port number
    Size of RX ring buffer
    Size of TX ring buffer
    Event queue size
    Pointer to store the event queue handle
    Flags to allocate an interrupt*/
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, UART_BUFSIZE*2, 0, 0, NULL, 0));
}

void app_main(void)
{
    while (1)
    {
        printf("Init UART bus... \n");
        vTaskDelay(1000/portTICK_PERIOD_MS); // 1000 ms delay cos why not
    }
    
}
