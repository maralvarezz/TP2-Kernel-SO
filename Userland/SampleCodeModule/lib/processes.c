#include "processes.h"
#define isVowel(c) ((c == 'a') || (c == 'e') || (c == 'i') || (c == 'o') || (c == 'u'))
#define toLower(n) ((n) >= 'A' && (n) <= 'Z' ? (n) - ('A' - 'a') : (n))


void loop(int argc, char *argv[]){
    if(argc != 2){
        printf("Es necesario que se pase la cantidad de segundos\n");
        exit();
        return;
    }

    int sec = atoi(argv[1]);
    if(sec < 0){
        printf("Los segundos indicados debe ser un entero positivo\n");
        exit();
        return;
    }

    while(1){
        printf("Hola, soy el proceso de PID: %d\n", getPid());
        sleep(sec);
    }
    exit();
    
}

void cat(int argc, char *argv[]){
    if(argc != 1){
        printf("No son necesarios argumentos para este comando.\n");
        exit();
        return;
    }

    char c;
    while((int)(c = getchar()) != -1){
        putchar(c);
    }
    exit();

}

void wc(int argc, char *argv[]){
    if(argc != 1){
        printf("No son necesarios argumentos para este comando.\n");
        exit();
        return;
    }

    int count = 0;
    char c;
    while((int) (c = getchar()) != -1){
        if(c == '\n'){
            count++;
        }
    }
    printf("Cantidad de lineas: %d\n", count);
    exit();
}

void filter(int argc, char *argv[]){
    if(argc != 1){
        printf("No son necesarios argumentos para este comando.\n");
        exit();
        return;
    }
    
    char c;
    while((int)(c = getchar()) != -1){
        if(isVowel(toLower(c))){
            putchar(c);
        }
    }
}

void testMemManager(int argc, char *argv[]){
    if(argc != 2){
        printf("Debe ingresar un un argumento con el tamaño de la memoria que desea testear.\n");
        exit();
        return;
    }
    if(atoi(argv[1]) <= 0){
        printf("El tamaño de la memoria debe ser un número positivo.\n");
        exit();
        return;
    }
    char *args[] = {argv[1]};
    int passed = test_mm(1, args);
    if(passed == -1){
        printf("Error al correr el test de memoria.\n");
        exit();
        return;
    }
    printf("Test de memoria finalizado corrextamente\n");
    exit();
    return;
}

void testPriority(int argc, char *argv[]){
    if(argc != 1){
        printf("No son necesarios argumentos para este comando.\n");
        exit();
        return;
    }
    test_prio();
    exit();
    return;
}

void testProcesses(int argc, char *argv[]){
    if(argc != 2){
        printf("Debe ingresar un argumento con la cantidad de procesos que desea testear.\n");
        exit();
        return;
    }
    int cant = atoi(argv[1]);
    if(cant <= 0 || cant > MAX_PROCESSES){
        printf("La cantidad de procesos debe ser un número entre 1 y %d.\n", MAX_PROCESSES);
        exit();
        return;
    }
    char *args[] = {argv[1]};
    int passed = test_processes(1, args);
    if(passed == -1){
        printf("Error al correr el test de procesos.\n");
        exit();
        return;
    }
    printf("Test de procesos finalizado correctamente.\n");
    exit();
    return;
}

void testSync(int argc, char *argv[]){
    if(argc != 3){
        printf("Debe ingresar dos argumentos con la cantidad de iteraciones queridas y si se desea usar semáforos o no.\n");
        exit();
        return;
    }

    if(atoi(argv[1]) <= 0){
        printf("La cantidad de iteraciones debe ser un número positivo.\n");
        exit();
        return;
    }

    if(atoi(argv[2]) < 0){
        printf("El segundo argumento debe ser 0 si no se desea usar semáforos o mayor a 0 de lo contrario.\n");
        exit();
        return;
    }

    char *args[] = {argv[1], argv[2]};

    int passed = test_sync(2, args);

    if(passed == -1){
        printf("Error al correr el test de sincronización.\n");
        exit();
        return;
    }

    printf("Test de sincronización finalizado correctamente.\n");
    exit();
    return;
}


