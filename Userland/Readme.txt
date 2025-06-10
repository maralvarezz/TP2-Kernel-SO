# Script de Build y Ejecución para TP2-Kernel (SO - ITBA)

Este proyecto contiene un sistema operativo básico para arquitectura Intel 64 bits, desarrollado como parte de la materia Sistemas Operativos del ITBA.

## Imagen Docker

El entorno de build y ejecución está pensado para usarse con la imagen:

agodio/itba-so-multi-platform:3.0

Incluye todas las herramientas necesarias: nasm, qemu, gcc, make, etc.

---

## Uso del script de build

El script principal para compilar y ejecutar el proyecto es:

./build.sh [all|buddy] [gdb]

### Opciones disponibles

| Flag/Argumento    | Descripción                                                                 |
|-------------------|-----------------------------------------------------------------------------|
| all               | Compila todo el proyecto usando el memory manager por defecto (bitmap).     |
| buddy             | Compila todo el proyecto usando el memory manager tipo buddy.               |
| gdb               | Ejecuta el kernel con soporte para GDB.                                     |

> Nota: El script ejecuta todos los comandos dentro de un contenedor Docker, por lo que no necesitas instalar dependencias en tu máquina.

---

## Ejemplos de uso

Compilar y ejecutar normalmente:
./build.sh all

Compilar con memory manager buddy:
./build.sh buddy

Compilar y ejecutar con GDB:
./build.sh all gdb

---

## Requisitos

- Docker instalado y funcionando.
- El proyecto debe estar ubicado en el mismo directorio que el script.
- run.sh debe estar presente para ejecutar el kernel.
- qemu

---

## Información relevante de uso del sistema

- Para usar mayúsculas, | y &, se debe utilizar la tecla Shift izquierda.
- El teclado está mapeado como un teclado en inglés.

---

## Combinaciones de teclas relevantes

- CTRL + C: Mata el proceso actual en foreground.
- CTRL + D: Envía un EOF (-1).
- CTRL + R: Toma una snapshot de la memoria de ese momento exacto.

---

## Foreground y background

Para enviar procesos a background, utilice el operador & al final del comando:

Ejemplo: loop 1 &

---

## GDB

- Para usar GDB deben instalar gdb-multiarch en su sistema.

---

## Créditos

- Basado en el proyecto x64BareBones de Rodrigo Rearden (RowDaBoat) y colaboradores.

## Integrantes

María Victoria Alvarez(Legajo:63165)
Santiago Sanchez Marostica(Legajo:64056)
Martín Gallardo Bertuzzi(63510)