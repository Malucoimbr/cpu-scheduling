# Escalonamento em Tempo Real
Este projeto implementa dois algoritmos de escalonamento para sistemas de tempo real: Rate-Monotonic (rate) e Earliest-Deadline-First (edf).

# Formato do Arquivo de Entrada
O arquivo de entrada deve ter o seguinte formato: <br>

[TOTAL TIME] <br>
[TASK NAME 1] [PERIOD 1] [CPU BURST 1] <br>
[TASK NAME 2] [PERIOD 2] [CPU BURST 2] <br>
... <br>
[TASK NAME n] [PERIOD n] [CPU BURST n] <br>


# Compilação
Para compilar o projeto, use o comando make seguido do algoritmo desejado: <br>

make rate  # Compila para Rate-Monotonic <br>
make edf   # Compila para Earliest-Deadline-First <br>


# Execução
Para executar o programa, use o comando correspondente ao algoritmo, seguido do nome do arquivo de entrada: <br>

./rate [namefile]  # Para Rate-Monotonic <br>
./edf [namefile]   # Para Earliest-Deadline-First <br>
