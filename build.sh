#!/bin/bash

# Entrar a la imagen de Docker y ejecutar los comandos
docker run --rm -v "$(pwd):/workspace" -w /workspace agodio/itba-so-multi-platform:3.0 bash -c "
    # Make clean en directorio inicial
    make clean
    
    # Entrar a Toolchain y hacer make clean
    cd Toolchain
    make clean
    
    # Hacer make all en Toolchain
    make all
    
    # Volver al directorio inicial
    cd ..
    
    # Hacer make all en directorio inicial
    make all
"

QCOW2_FILE=$(find Image/ -name "*.qcow2" -type f | head -1)

if [ -n "$QCOW2_FILE" ]; then
    sudo chmod 777 "$QCOW2_FILE"
else
    echo "No se encontró archivo .qcow2 en Image/"
    exit 1
fi

# Ejecutar run.sh con el parámetro recibido (si existe)
if [ -f "run.sh" ]; then
    echo "Ejecutando run.sh"
    if [ "$1" = "gdb" ]; then
        ./run.sh gdb
    else
        ./run.sh
    fi
else
    echo "No se encontró run.sh en el directorio actual"
    exit 1
fi