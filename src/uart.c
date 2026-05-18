#include "UART.h"
#include <stdio.h> // Incluido por convención, aunque ya no necesitamos printf

void UART_Init(void) {
    // Configuración de la dirección de los pines
    TRISC6 = 0; // Pin RC6 configurado como SALIDA (TX - Transmisor)
    TRISC7 = 1; // Pin RC7 configurado como ENTRADA (RX - Receptor)

    // Configuración de la velocidad (Baudrate) a 9600 para un reloj de 16MHz
    // Se calcula usando la fórmula: SPBRG = (Fosc / (64 * Baud)) - 1
    SPBRG1  = 25; 
    TXSTA1bits.BRGH  = 0;   // Configura el generador en baja velocidad
    BAUDCON1bits.BRG16 = 0; // Utiliza un registro de 8 bits para el generador de baudios

    // Habilitación y configuración del módulo
    RCSTA1bits.SPEN = 1;    // Enciende el puerto serial completo
    TXSTA1bits.SYNC = 0;    // Configura en modo asíncrono (estándar para UART)
    TXSTA1bits.TXEN = 1;    // Activa la capacidad de transmitir datos
    RCSTA1bits.CREN = 1;    // Activa la capacidad de recibir datos continuamente

    // Configuración de interrupciones (Activas, aunque no haya función de interrupción aún)
    PIE1bits.RC1IE  = 1;    // Activa la interrupción cuando llega un dato por RX
    PIR1bits.RC1IF  = 0;    // Limpia la bandera de recepción por seguridad
    INTCONbits.PEIE = 1;    // Habilita las interrupciones de periféricos
    INTCONbits.GIE  = 1;    // Habilita el interruptor general (Global) de interrupciones
}

void UART_WriteChar(char data) {
    // El código se pausa aquí mientras el registro de transmisión (TRMT) esté lleno
    while (!TXSTA1bits.TRMT); 
    // Cuando se vacía, coloca el nuevo carácter para enviarlo
    TXREG1 = data;
}

void UART_WriteString(const char* str) {
    // Recorre la cadena hasta encontrar un carácter nulo '\0' (el final del texto)
    while (*str) {
        UART_WriteChar(*str++); // Envía la letra actual y avanza a la siguiente
    }
}

// Convierte un número entero a caracteres ASCII y lo envía por serial
void UART_WriteUInt(uint16_t value) {
    char buffer[6]; // Arreglo para guardar hasta 5 dígitos + el fin de cadena '\0'
    uint8_t i = 0;

    // Caso especial: si el valor es 0, simplemente envía '0' y sale de la función
    if (value == 0) {
        UART_WriteChar('0');
        return;
    }

    // Descompone el número de derecha a izquierda
    while (value > 0) {
        buffer[i++] = '0' + (value % 10); // Extrae el último dígito y lo vuelve un carácter ASCII
        value /= 10;                      // Elimina ese último dígito del número original
    }

    // Como los extrajimos de derecha a izquierda, los enviamos al revés para que salgan bien
    while (i > 0) {
        UART_WriteChar(buffer[--i]);
    }
}

// Convierte el valor del ADC en un formato de voltaje de 0 a 5V con 3 decimales
void UART_WriteVoltage(uint16_t adcValue) {
    // Regla de tres para pasar de 1023 niveles a 5000 milivoltios. 
    // Se usa 'UL' (Unsigned Long) para evitar que la multiplicación desborde el límite de 16 bits.
    uint32_t mV = ((uint32_t)adcValue * 5000UL) / 1023UL;
    
    // Separa la parte entera (voltios) de la parte fraccionaria (milivoltios)
    uint16_t volts  = (uint16_t)(mV / 1000);
    uint16_t millis = (uint16_t)(mV % 1000);

    // 1. Envía el número entero (ej. "3")
    UART_WriteUInt(volts);
    
    // 2. Envía el punto decimal
    UART_WriteChar('.');
    
    // 3. Añade ceros a la izquierda si los decimales son pequeños para que mantenga el formato
    // Por ejemplo, si millis es 5, debe enviar ".005", no ".5"
    if (millis < 100) UART_WriteChar('0');
    if (millis < 10)  UART_WriteChar('0');
    
    // 4. Envía la parte decimal
    UART_WriteUInt(millis);
}