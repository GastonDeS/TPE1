Configuración del entorno:
1- Instale los siguientes paquetes antes de construir :

apt-get install minisat

2- Construir solve slave and view

Ejecute los siguientes comandos en TP1:

user@linux:$ make all

3-  correr el programa

1 :

    user@linux:$ ./solve files/.../* | ./view

2 :

    En una terminal:
        user@linux:$ ./solve files/.../*

    En otra terminal:
        user@linux:$ ./view (output de ./solve)

Autores: Gaston De Schant, Felipe Cupito, Segundo Espina Mairal