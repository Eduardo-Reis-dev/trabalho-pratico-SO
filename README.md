# Trabalho-pratico-SO
Trabalho prático de Sistemas Operacionais- Entrega par ao dia 21/04/2026

Integrantes: Aldo Constantino Mendes e Eduardo Araujo Reis

# Trabalho Prático 1: Algoritmo do Banqueiro

Este repositório contém a implementação multithreaded do Algoritmo do Banqueiro (descrito na Seção 7.5.3 do livro de Silberschatz), utilizando a linguagem C e a biblioteca POSIX Threads (`pthreads`).

## Pré-requisitos
- Um compilador C (como o GCC).
- Sistema operacional baseado em Unix/Linux (ou WSL no Windows) para o suporte nativo a `pthreads`.

## Como compilar

No terminal, navegue até a pasta onde o arquivo `banqueiro.c` está salvo e execute o seguinte comando:

`gcc -pthread banqueiro.c -o banqueiro -Wall`

## Como executar

O programa exige que os recursos iniciais sejam passados via linha de comando. Sabendo que o código está configurado para 3 recursos, você deve passar 3 inteiros. Exemplo:

`./banqueiro 10 5 7`

A execução entrará em um loop contínuo onde as threads (clientes) tentarão requisitar e liberar recursos continuamente, sendo autorizadas pelo banqueiro apenas se o estado permanecer seguro. Use Ctrl+C para encerrar.
