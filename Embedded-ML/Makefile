all: main

main: main.c ShotIdentifier.c EvaluateNetwork.c parseEncog.c
	gcc -g -O0 main.c ShotIdentifier.c EvaluateNetwork.c parseEncog.c -o main -lm

clean:
	rm main