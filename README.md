[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/MCJunYEq)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=23643366&assignment_repo_type=AssignmentRepo)
# Lab06: Comunicación UART con PIC18F45K22

## Integrantes
* [Santiago Leonardo Molina Bogotá]

* [Hellen Julieth Rincón Orjuela]

* [Miguel Angel Tarazona Peinado]

## Documentación
#include <xc.h>  
// Incluye la librería principal del microcontrolador PIC.
// Permite usar registros internos como OSCCON, PORT, TRIS, etc.

#include "uart.h"  
// Incluye el archivo de funciones UART.
// Aquí están las funciones para iniciar y enviar datos por comunicación serial.

#pragma config FOSC = INTIO67  
// Configura el oscilador interno del PIC.
// No se necesita cristal externo.

#pragma config WDTEN = OFF     
// Desactiva el Watchdog Timer.
// Evita que el microcontrolador se reinicie automáticamente.

#pragma config LVP = OFF       
// Desactiva la programación a bajo voltaje.
// Libera algunos pines para uso normal.

void main(void) {  
// Función principal donde inicia el programa.

    OSCCON = 0b01110000;  
    // Configura el oscilador interno a 16 MHz.
    // El PIC trabajará con esta frecuencia.

    UART_Init();          
    // Inicializa la comunicación UART.
    // Configura velocidad y transmisión serial.

    while(1) {  
    // Ciclo infinito.
    // Todo lo que esté dentro se repetirá siempre.

        UART_WriteString("Hola, UART funcionando!\r\n");  
        // Envía el mensaje por UART.
        // \r\n genera salto de línea en el monitor serial.

        __delay_ms(1000);  
        // Espera 1000 milisegundos.
        // Equivale a 1 segundo.
    }
}

/*
Este programa configura un microcontrolador PIC utilizando el oscilador interno a 16 MHz y habilita la comunicación UART. 
Primero se incluyen las librerías necesarias para el control del microcontrolador y las funciones de comunicación serial. 
Luego se configuran los bits del sistema, desactivando el Watchdog Timer y la programación a bajo voltaje. 
En la función principal se ajusta la frecuencia del oscilador y se inicializa la UART. 
Posteriormente, el programa entra en un ciclo infinito donde envía cada segundo el mensaje 
"Hola, UART funcionando!" al monitor serial, verificando que la transmisión UART funciona correctamente.
*/


#include "uart.h"  
// Incluye el archivo de cabecera UART.
// Contiene las declaraciones de las funciones de comunicación serial.

#include <stdio.h>  
// Incluye funciones estándar de entrada y salida en C.

void UART_Init(void) {  
// Función que inicializa la comunicación UART.

    TRISC6 = 0; 
    // Configura el pin RC6 (TX) como salida.

    TRISC7 = 1; 
    // Configura el pin RC7 (RX) como entrada.

    SPBRG1 = 25; 
    // Configura el baudrate a 9600 bps para una frecuencia de 16 MHz.

    TXSTA1bits.BRGH = 0; 
    // Selecciona modo de baja velocidad para UART.

    BAUDCON1bits.BRG16 = 0; 
    // Usa generador de baudrate de 8 bits.

    RCSTA1bits.SPEN = 1; 
    // Habilita el módulo serial UART.

    TXSTA1bits.SYNC = 0; 
    // Configura UART en modo asíncrono.

    TXSTA1bits.TXEN = 1; 
    // Habilita la transmisión UART.

    RCSTA1bits.CREN = 1; 
    // Habilita la recepción continua UART.

    PIE1bits.RC1IE = 1;   
    // Habilita interrupción por recepción UART.

    PIR1bits.RC1IF = 0;   
    // Limpia la bandera de interrupción.

    INTCONbits.PEIE = 1;  
    // Habilita interrupciones periféricas.

    INTCONbits.GIE = 1;   
    // Habilita interrupciones globales.
}

void UART_WriteChar(char data) {  
// Función para enviar un carácter por UART.

    while (!TXSTA1bits.TRMT); 
    // Espera hasta que el buffer de transmisión esté vacío.

    TXREG1 = data;
    // Envía el carácter al registro de transmisión.
}

void UART_WriteString(const char* str) {  
// Función para enviar una cadena de texto por UART.

    while (*str) {  
    // Recorre cada carácter de la cadena.

        UART_WriteChar(*str++);  
        // Envía cada carácter y avanza al siguiente.
    }
}

/*
Este código configura la comunicación UART en un microcontrolador PIC utilizando una frecuencia de 16 MHz y una velocidad de transmisión de 9600 baudios. 
Primero se incluyen las librerías necesarias para manejar la comunicación serial y las funciones estándar del lenguaje C. 
Luego, en la función UART_Init(), se configuran los pines RC6 como salida para transmisión (TX) y RC7 como entrada para recepción (RX). 
Después se ajusta el baudrate mediante el registro SPBRG1 y se establece el modo asíncrono de comunicación. 
También se habilitan la transmisión, la recepción y las interrupciones UART para permitir el envío y recepción de datos seriales.

La función UART_WriteChar() se encarga de enviar un solo carácter esperando primero a que el buffer de transmisión esté vacío. 
Finalmente, la función UART_WriteString() permite enviar cadenas completas de texto recorriendo cada carácter y transmitiéndolo uno por uno mediante UART. 
Este programa permite realizar comunicación serial entre el microcontrolador y dispositivos externos como una computadora o un monitor serial.
*/


#ifndef UART_H
// Verifica si UART_H no ha sido definido anteriormente.
// Evita que el archivo se incluya varias veces.

#define UART_H
// Define UART_H como identificador de protección.

#include <xc.h>
// Incluye la librería principal del microcontrolador PIC.
// Permite usar registros internos y configuraciones del PIC.

#include <stdint.h>
// Incluye tipos de datos estándar como uint16_t.

#define _XTAL_FREQ 16000000UL    
// Define la frecuencia del oscilador en 16 MHz.
// Se usa para funciones de retardos como __delay_ms().

void UART_Init(void);
// Declaración de la función que inicializa UART.

void UART_WriteChar(char data);
// Declaración de la función que envía un carácter por UART.

void UART_WriteString(const char* str);
// Declaración de la función que envía una cadena de texto.

void UART_WriteUInt(uint16_t value);
// Declaración de la función que envía números enteros sin signo.

void UART_WriteVoltage(uint16_t adcValue);
// Declaración de la función que envía valores de voltaje del ADC.

#endif
// Finaliza la protección del archivo de cabecera.

/*
Este archivo de cabecera corresponde a la librería UART para un microcontrolador PIC. 
Primero se utilizan las directivas #ifndef, #define y #endif para evitar que el archivo se incluya varias veces durante la compilación. 
Luego se incluyen las librerías <xc.h>, que permite acceder a los registros y configuraciones del PIC, y <stdint.h>, que proporciona tipos de datos estándar como uint16_t. 

La línea #define _XTAL_FREQ 16000000UL establece la frecuencia del oscilador en 16 MHz, necesaria para el funcionamiento correcto de las funciones de retardo. 

Posteriormente se declaran los prototipos de las funciones UART:
UART_Init() para inicializar la comunicación serial,
UART_WriteChar() para enviar un carácter,
UART_WriteString() para enviar cadenas de texto,
UART_WriteUInt() para transmitir números enteros sin signo,
y UART_WriteVoltage() para enviar valores de voltaje obtenidos desde el convertidor ADC.

Este archivo organiza y facilita el uso de las funciones UART dentro del programa principal.
*/





## Diagramas

## Evidencias de implementación

