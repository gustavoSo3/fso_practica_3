all: serial hilos clonando paralelos

serial: serial.c
	gcc -o serial serial.c -lpthread
hilos: hilos.c
	gcc -o hilos hilos.c -lpthread
clonando: clonando.c
	gcc -o clonando clonando.c -lpthread
paralelos: paralelos.c
	gcc -o paralelos paralelos.c -lpthread
