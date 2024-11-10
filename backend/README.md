# Projeto Matemática Discreta - AB2

Este projeto foi solicitado pelo Professor Bruno Pimentel para a matéria de **Matemática Discreta**. O objetivo é implementar um sistema de criptografia utilizando **RSA**. O projeto foi desenvolvido por uma equipe composta por:

- **Alexia Rodrigues**
- **Bárbara Albuquerque**
- **Lara Nogueira**

A implementação foi feita em **C**, utilizando as bibliotecas **GMP** (para manipulação de números grandes) e **GTK** (para criação da interface gráfica).

## Funções Principais

O código implementa as seguintes funções principais:

1. **Gerar Chave Pública**: Geração da chave pública (módulo `n` e expoente público `e`) e chave privada (expoente privado `d`), usando os algoritmos de criptografia RSA.
2. **Encriptar**: A função de encriptação usa a chave pública para criptografar uma mensagem. Cada caractere da mensagem é convertido para seu valor numérico (usando a tabela ASCII) e então encriptado pela operação modular m^e % n, onde m é o valor numérico do caractere.
3. **Desencriptar**: A função de desencriptação utiliza a chave privada para reverter o processo de encriptação. A operação usada é m^d % n, onde d é o expoente privado, e o valor resultante é convertido de volta para o caractere original.

## Requisitos

Antes de rodar o código, é necessário ter as seguintes bibliotecas instaladas no seu sistema:

- **GMP**: Biblioteca para manipulação de números grandes.
- **GTK+ 3.0**: Biblioteca para criar interfaces gráficas.

### No Ubuntu/Debian (Linux)
```bash
sudo apt update
sudo apt install libgmp-dev libgtk-3-dev pkg-config

### No Windows (Via MSYS2)
1. Instale o **MSYS2**
2. No terminal do MSYS2, execute os seguintes comandos para instalar as bibliotecas:

pacman -S mingw-w64-x86_64-gmp mingw-w64-x86_64-gtk3 pkg-config

## Como Executar o Código
### Passo 1: Baixe o Repositório
Clone o repositório para o seu computador utilizando o comando:
git clone https://github.com/alexias1/projeto-md.git
### Passo 2: Compilar o Código
Abra o terminal e navegue até o diretório do projeto. Em seguida, compile o código com o seguinte comando:
gcc main.c -o main.exe `pkg-config --cflags --libs gtk+-3.0 gmp`
### Passo 3: Executar o Programa
Depois de compilar, execute o programa com o seguinte comando:
./main.exe