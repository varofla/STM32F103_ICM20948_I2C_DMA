#ifndef SRC_HW_DRIVER_UART_H_
#define SRC_HW_DRIVER_UART_H_

#include <stdbool.h>
#include <stdint.h>

bool uart_init(void);

void uart_send_string(uint8_t *buff);

#endif /* SRC_HW_DRIVER_UART_H_ */