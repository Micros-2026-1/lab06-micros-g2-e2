#include <xc.h>      // Librería principal del compilador para los PIC
#include "uart.h"    // Incluye las funciones de nuestro módulo UART

// --- Bits de Configuración (Fuses) ---
#pragma config FOSC = INTIO67  // Configura el oscilador interno (sin cristal externo)
#pragma config WDTEN = OFF     // Apaga el Watchdog Timer para evitar reinicios inesperados
#pragma config LVP = OFF       // Apaga la programación en bajo voltaje (evita problemas de ruido)

void main(void) {
    // Configuración del reloj interno a 16 MHz
    OSCCON = 0b01110000;

    // Llama a la función que prepara el puerto serial
    UART_Init();

    // Variables para nuestra simulación del sensor
    uint16_t adc = 0;   // Empezamos con una lectura inicial de 0
    int16_t  paso = 20; // Cantidad que sumaremos o restaremos en cada ciclo

    while (1) {
        // Modificamos el valor del ADC simulado
        adc += paso;

        // Lógica de "rebote" para mantener el valor siempre entre 0 y 1023
        if (adc >= 1023) {
            adc  = 1023; // Topamos en el máximo permitido (10 bits)
            paso = -20;  // Invertimos la dirección (empieza a bajar)
        }
        if (adc <= 0) {
            adc  = 0;   // Topamos en el mínimo permitido
            paso = 20;  // Invertimos la dirección (empieza a subir)
        }

        // Enviamos la información por el puerto serial
        UART_WriteString("Voltaje: "); // Texto base
        UART_WriteVoltage(adc);        // Función que convierte de 0-1023 a 0.000V-5.000V
        UART_WriteString("\r\n");      // Retorno de carro y salto de línea (como presionar Enter)

        // Pausa para no saturar el puerto y que podamos leer cómodamente en la pantalla
        __delay_ms(200); 
    }
}