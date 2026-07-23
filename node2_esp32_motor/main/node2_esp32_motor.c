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

void app_main(void)
{
    while (1)
    {
        printf("Hello from ESP32 node...! \n");
        vTaskDelay(1000/portTICK_PERIOD_MS); // 1000 ms delay cos why not
    }
    
}
