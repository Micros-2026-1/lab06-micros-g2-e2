#ifndef UART_H
#define UART_H

#include <xc.h>      // Incluye definiciones del hardware del PIC
#include <stdint.h>  // Nos permite usar tipos de datos claros como uint16_t y uint32_t

// Define la frecuencia del oscilador (16 MHz) para que las funciones __delay funcionen
#define _XTAL_FREQ 16000000UL    

// Prototipos de las funciones: avisan al compilador qué funciones existen en UART.c
void UART_Init(void);
void UART_WriteChar(char data);
void UART_WriteString(const char* str);
void UART_WriteUInt(uint16_t value);
void UART_WriteVoltage(uint16_t adcValue);

#endif