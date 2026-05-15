[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/MCJunYEq)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=23643366&assignment_repo_type=AssignmentRepo)
# Lab06: Comunicación UART con PIC18F45K22

## Integrantes

* [Santiago Leonardo Molina Bogotá](https://github.com/SaintGao-cmd)

* [Hellen Julieth Rincón Orjuela](https://github.com/hellenjurinconor-crypto)

* [Miguel Angel Tarazona Peinado](https://github.com/miguetarazona) 

## Documentación

# Informe de Laboratorio: Comunicación UART en Microcontrolador PIC

## 1. Introducción

Este proyecto implementa un módulo de comunicación serial **UART** (*Universal Asynchronous Receiver-Transmitter*) para un microcontrolador PIC de 8 bits (familia PIC18). El objetivo es transmitir datos simulados de un sensor ADC hacia una terminal serial en una PC, presentando los valores como voltajes con tres decimales de precisión.

El proyecto se compone de tres archivos:

| Archivo | Rol |
|---|---|
| `UART.h` | Cabecera del módulo: definiciones y prototipos |
| `UART.c` | Implementación de las funciones UART |
| `main_UART.c` | Programa principal y lógica de la aplicación |

---

## 2. Archivo `UART.h` — Cabecera del Módulo

### Descripción general

El archivo de cabecera actúa como la **interfaz pública** del módulo UART. Define constantes y declara las funciones disponibles para que otros archivos puedan usarlas sin conocer su implementación interna.

### Análisis del código

```c
#ifndef UART_H
#define UART_H
```
Guardas de inclusión (*include guards*): evitan que el compilador procese este archivo más de una vez si es incluido desde múltiples lugares, previniendo errores de redefinición.

```c
#include <xc.h>
#include <stdint.h>
```
- `xc.h`: librería del compilador XC8 de Microchip; da acceso a los registros del hardware del PIC (como `TRISC6`, `TXSTA1`, etc.).
- `stdint.h`: permite usar tipos enteros de tamaño fijo (`uint8_t`, `uint16_t`, `uint32_t`), haciendo el código portable y explícito en el tamaño de datos.

```c
#define _XTAL_FREQ 16000000UL
```
Define la frecuencia del oscilador en **16 MHz**. Esta macro es requerida por la función `__delay_ms()` del compilador para calcular correctamente los tiempos de espera.

### Prototipos de funciones

```c
void UART_Init(void);
void UART_WriteChar(char data);
void UART_WriteString(const char* str);
void UART_WriteUInt(uint16_t value);
void UART_WriteVoltage(uint16_t adcValue);
```

Cada prototipo le informa al compilador la firma de la función antes de su uso:

| Función | Propósito |
|---|---|
| `UART_Init()` | Inicializa y configura el módulo UART del PIC |
| `UART_WriteChar()` | Transmite un único carácter ASCII |
| `UART_WriteString()` | Transmite una cadena de texto completa |
| `UART_WriteUInt()` | Convierte y transmite un entero sin signo |
| `UART_WriteVoltage()` | Convierte un valor ADC a voltaje y lo transmite |

---

## 3. Archivo `UART.c` — Implementación del Módulo

### 3.1 Función `UART_Init()`

Esta función configura todos los registros necesarios para que el hardware UART del PIC funcione correctamente.

```c
TRISC6 = 0; // RC6 → SALIDA (TX)
TRISC7 = 1; // RC7 → ENTRADA (RX)
```
Configura la dirección de los pines del puerto C: **RC6** es la línea de transmisión y **RC7** la de recepción, siguiendo el estándar del módulo EUSART del PIC18.

```c
SPBRG1  = 25;
TXSTA1bits.BRGH  = 0;
BAUDCON1bits.BRG16 = 0;
```
Configura el **generador de baudios** a 9600 bps. El valor 25 se obtiene con la fórmula:

$$SPBRG = \frac{F_{osc}}{64 \times Baud} - 1 = \frac{16{,}000{,}000}{64 \times 9600} - 1 \approx 25$$

Con `BRGH = 0` y `BRG16 = 0` se selecciona el modo de baja velocidad con registro de 8 bits.

```c
RCSTA1bits.SPEN = 1; // Enciende el puerto serial
TXSTA1bits.SYNC = 0; // Modo asíncrono (UART estándar)
TXSTA1bits.TXEN = 1; // Habilita transmisión
RCSTA1bits.CREN = 1; // Habilita recepción continua
```
Habilita el módulo EUSART del PIC en modo asíncrono, que es el modo estándar de UART.

```c
PIE1bits.RC1IE  = 1; // Interrupción por recepción
PIR1bits.RC1IF  = 0; // Limpia bandera de recepción
INTCONbits.PEIE = 1; // Habilita interrupciones de periféricos
INTCONbits.GIE  = 1; // Habilita interrupción global
```
Activa el sistema de **interrupciones por recepción**, de modo que el microcontrolador puede reaccionar automáticamente cuando llega un dato por el puerto serial, sin necesidad de verificar constantemente el registro.

---

### 3.2 Función `UART_WriteChar()`

```c
void UART_WriteChar(char data) {
    while (!TXSTA1bits.TRMT);
    TXREG1 = data;
}
```

Implementa el envío de un solo byte con **espera activa** (*polling*):
1. El ciclo `while` pausa la ejecución mientras el registro de desplazamiento del transmisor (`TRMT`) esté ocupado.
2. Cuando se libera, el carácter se carga en `TXREG1` y el hardware lo transmite automáticamente bit a bit.

---

### 3.3 Función `UART_WriteString()`

```c
void UART_WriteString(const char* str) {
    while (*str) {
        UART_WriteChar(*str++);
    }
}
```

Recorre una cadena de caracteres hasta encontrar el carácter nulo `'\0'` (terminador estándar de cadenas en C), enviando cada carácter individualmente mediante `UART_WriteChar()`. El operador `*str++` lee el carácter actual y avanza el puntero.

---

### 3.4 Función `UART_WriteUInt()`

```c
void UART_WriteUInt(uint16_t value) {
    char buffer[6];
    uint8_t i = 0;

    if (value == 0) { UART_WriteChar('0'); return; }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    while (i > 0) {
        UART_WriteChar(buffer[--i]);
    }
}
```

Convierte un entero sin signo a su representación ASCII para transmitirlo como texto:

1. Se extrae cada dígito mediante el residuo (`% 10`) y se convierte a ASCII sumando el código de `'0'` (48 en ASCII).
2. Los dígitos se extraen de **derecha a izquierda** (del menos significativo al más significativo), por lo que se almacenan en un buffer.
3. Finalmente se envían en orden **inverso** para que el número aparezca correctamente en la terminal.

**Ejemplo:** Para `value = 342`:
- Iteración 1: `'2'` → buffer[0]
- Iteración 2: `'4'` → buffer[1]
- Iteración 3: `'3'` → buffer[2]
- Se transmite: `'3'`, `'4'`, `'2'` ✓

---

### 3.5 Función `UART_WriteVoltage()`

```c
void UART_WriteVoltage(uint16_t adcValue) {
    uint32_t mV = ((uint32_t)adcValue * 5000UL) / 1023UL;

    uint16_t volts  = (uint16_t)(mV / 1000);
    uint16_t millis = (uint16_t)(mV % 1000);

    UART_WriteUInt(volts);
    UART_WriteChar('.');
    if (millis < 100) UART_WriteChar('0');
    if (millis < 10)  UART_WriteChar('0');
    UART_WriteUInt(millis);
}
```

Convierte un valor de ADC de 10 bits (rango 0–1023) a voltaje real (0–5 V):

**Conversión:** Se aplica una regla de tres lineal escalada a milivoltios para evitar decimales:

$$V_{mV} = \frac{adcValue \times 5000}{1023}$$

El cast a `uint32_t` es **crítico**: sin él, `adcValue * 5000` podría superar el límite de 16 bits (máximo 65,535), causando desbordamiento. Con el cast se opera en 32 bits (máximo ~4,294 millones).

**Formato de salida:** Los milivoltios se separan en parte entera y decimal. Se añaden ceros a la izquierda si es necesario para mantener siempre 3 dígitos decimales:

| `millis` | Salida |
|---|---|
| 5 | `.005` |
| 47 | `.047` |
| 312 | `.312` |

---

## 4. Archivo `main_UART.c` — Programa Principal

### 4.1 Bits de Configuración (Fuses)

```c
#pragma config FOSC  = INTIO67 // Oscilador interno (sin cristal externo)
#pragma config WDTEN = OFF     // Watchdog Timer desactivado
#pragma config LVP   = OFF     // Programación en bajo voltaje desactivada
```

Los *fuses* son bits de configuración grabados en la memoria del PIC que determinan su comportamiento desde el arranque, independientemente del programa.

### 4.2 Configuración del Oscilador

```c
OSCCON = 0b01110000; // 16 MHz con oscilador interno
```

El registro `OSCCON` controla la frecuencia del oscilador interno. El valor `0b01110000` selecciona 16 MHz, que es la frecuencia máxima del oscilador interno del PIC18.

### 4.3 Lógica de Simulación del Sensor ADC

```c
uint16_t adc  = 0;
int16_t  paso = 20;

while (1) {
    adc += paso;

    if (adc >= 1023) { adc = 1023; paso = -20; }
    if (adc <= 0)    { adc = 0;    paso =  20; }

    UART_WriteString("Voltaje: ");
    UART_WriteVoltage(adc);
    UART_WriteString("\r\n");

    __delay_ms(200);
}
```

Se simula una señal de sensor triangular (sube y baja) dentro del rango válido de un ADC de 10 bits (0–1023):

- La variable `adc` se incrementa o decrementa en pasos de 20.
- Al alcanzar los límites (0 ó 1023), el paso se invierte, generando una onda triangular.
- Cada 200 ms se transmite el voltaje equivalente por UART.
- `"\r\n"` representa retorno de carro y nueva línea, estándar en terminales seriales.

**Ejemplo de salida en terminal:**
```
Voltaje: 0.000V
Voltaje: 0.097V
Voltaje: 0.195V
...
Voltaje: 4.902V
Voltaje: 5.000V
Voltaje: 4.902V
...
```

---

## 5. Diagrama de Flujo General

```
[Inicio]
   │
   ▼
[Configurar oscilador a 16 MHz]
   │
   ▼
[UART_Init(): configurar pines, baudios, interrupciones]
   │
   ▼
[Loop infinito]
   │
   ├─► adc += paso
   │
   ├─► ¿adc >= 1023? → adc=1023, paso=-20
   ├─► ¿adc <= 0?    → adc=0,    paso=+20
   │
   ├─► Enviar "Voltaje: " + UART_WriteVoltage(adc) + "\r\n"
   │
   └─► Esperar 200 ms → [repite]
```

---

## 6. Conclusiones

- Se implementó exitosamente un módulo UART modular y reutilizable, separando la lógica en cabecera (`.h`) e implementación (`.c`).
- La conversión de enteros a texto ASCII se realizó sin usar funciones de la librería estándar como `printf`, lo que reduce el uso de memoria en el microcontrolador.
- El manejo del tipo `uint32_t` en la conversión de voltaje es un ejemplo crítico de la importancia de gestionar el tamaño de los datos en sistemas embebidos para evitar desbordamientos.
- La simulación de señal triangular permite verificar el correcto funcionamiento del módulo en todo el rango del ADC sin necesidad de hardware adicional.

## Diagramas

![Gráfica UART](UART.png)



![Terminal PuTTY con UART funcionando](Mensaje_Putty.png)

## Evidencias de implementación