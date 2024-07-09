# Regra para compilar o real.c com algoritmo rate
rate: real.c 
	gcc -Wall -o rate real.c

# Regra para compilar o real.c com algoritmo edf
edf: real.c
	gcc -Wall -o edf real.c