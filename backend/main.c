#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <gmp.h>

//opção: fazer só parar quando digitar -1

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int calcular_primo(int n) {
    if (n <= 1)
        return 0;
    if (n == 2 || n == 3)
        return 1;
    if (n % 2 == 0 || n % 3 == 0)
        return 0;
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0)
            return 0;
    }
    return 1;
}

int algoritmo_euclides(int a, int b) {
    while (b != 0) {
        int resto = a % b;
        a = b;
        b = resto;
    }
    return a;
}

int algoritmo_euclides_estendido(int a, int b, int *s, int *t) {
    if (b == 0) {
        *s = 1;
        *t = 0;
        return a;
    }

    int s1, t1;
    int mdc = algoritmo_euclides_estendido(b, a % b, &s1, &t1);

    *s = t1;
    *t = s1 - (a / b) * t1;

    return mdc;
}

long long int exponenciacao_modular(long long int base, long long int exp, long long int mod) {
    long long int resultado = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            resultado = (resultado * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return resultado;
}

int main() {
    printf("Escolha:\n 1 - Gerar chave publica\n 2 - Encriptar\n 3 - Desencriptar\n");
    int escolha;
    long int p, q, e;
    long long int n, produtopq;
    scanf("%d", &escolha);

    switch (escolha) {
        case 1: // Gerar chave pública
            printf("Digite P, Q, e E:\n");
            scanf("%ld %ld %ld", &p, &q, &e);
            // Checar se p e q são primos
            while (calcular_primo(p) == 0 || calcular_primo(q) == 0) {
                printf("P e Q nao sao primos! \nInsira P, Q e E novamente:\n");
                scanf("%ld %ld %ld", &p, &q, &e);
            }
            produtopq = (p - 1) * (q - 1);
            // Checar se e é coprimo ao produto (p-1)*(q-1)
            while (algoritmo_euclides(e, produtopq) != 1) {
                printf("e nao eh coprimo ao produto (p-1)*(q-1)!\n Insira P, Q e E novamente:\n");
                scanf("%ld %ld %ld", &p, &q, &e);
            }
            n = p * q;
            FILE *pont_arq = fopen("chave_publica.txt", "w");
            if (pont_arq != NULL) {
                fprintf(pont_arq, "%lld %ld\n", n, e);
                fclose(pont_arq);
                printf("Chave publica gerada e salva no arquivo 'chave_publica.txt'.\n");
            } else {
                printf("Erro ao abrir o arquivo!\n");
            }
            break;

        case 2: // Encriptar
            printf("Digite N e E:\n");
            scanf("%lld %ld", &n, &e);

            printf("Escolha:\n 1 - Digitar mensagem\n 2 - Fazer upload da mensagem\n");
            int opcao;
            limparBuffer();
            scanf("%d", &opcao);
            limparBuffer();

            switch (opcao) {
                case 1: { // Digitar mensagem
                    char letra;
                    FILE *mensagem_crpt = fopen("mensagem_encriptografada.txt", "w+");
                    printf("Digite a mensagem de texto a encriptar:\n");
                    while ((letra = getchar()) != '\n') {
                        long long int encriptada = exponenciacao_modular(letra, e, n);
                        fprintf(mensagem_crpt, "%lld ", encriptada);
                    }
                    fclose(mensagem_crpt);
                    break;
                }

                case 2: { // Upload da mensagem
                    char texto;
                    long long int encriptada;
                    FILE *arquivoEntrada = fopen("arquivo.txt", "r");
                    FILE *arquivoSaida = fopen("mensagem_encriptografada.txt", "w");

                    if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                        printf("Erro ao abrir o arquivo.\n");
                        break;
                    }

                    while ((texto = fgetc(arquivoEntrada)) != EOF) {
                        long long int mensagem = (long long int) texto;
                        encriptada = exponenciacao_modular(mensagem, e, n);
                        fprintf(arquivoSaida, "%lld ", encriptada);
                    }
                    fclose(arquivoEntrada);
                    fclose(arquivoSaida);
                    break;
                }

                default:
                    printf("Escolha invalida!\n");
                    break;
            }
            break;

        case 3: // Desencriptar
        printf("Digite P, Q, e E:\n");
        scanf("%ld %ld %ld", &p, &q, &e);

        printf("Escolha:\n 1 - Digitar mensagem\n 2 - Fazer upload da mensagem\n");
        int escolher;
        limparBuffer();
        scanf("%d", &escolher);
        limparBuffer();

        produtopq = (p - 1) * (q - 1);
        int d, t;
        algoritmo_euclides_estendido(e, produtopq, &d, &t);
        if (d < 0) {
            d += produtopq;
        }        
        long long int n = p * q;
        printf("Chave privada (D,E): (%d,%ld)\n", d, e);

        long long int numero;

        switch (escolher) {
            case 1: { // Digitar mensagem
            long long int numero;
            FILE *mensagem_desencripitada = fopen("mensagem_desencriptografada.txt", "w+");
            while (scanf("%lld", &numero) == 1 && numero != -1) { 
                long long int desencriptado = exponenciacao_modular(numero, d, n);
                fprintf(mensagem_desencripitada, "%c", (char)desencriptado);
                printf("%c", (char)desencriptado);
            }
            fclose(mensagem_desencripitada);
            break;
            }

            case 2: { //Upload mensagem

                long long int numero;
                long long int desencriptada;
                FILE *arquivoEntrada = fopen("arquivo.txt", "r");
                FILE *arquivoSaida = fopen("mensagem_desencriptografada.txt", "w");

                if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                    printf("Erro ao abrir o arquivo.\n");
                    break;
                }

                while (fscanf(arquivoEntrada, "%lld", &numero) == 1 && numero != -1) { 
                long long int desencriptado = exponenciacao_modular(numero, d, n);
                fprintf(arquivoSaida, "%c", (char)desencriptado); 
                }
                fclose(arquivoEntrada);
                fclose(arquivoSaida);
                    break;

            }
        }

        break;

    default:
        printf("Escolha invalida!\n");
        break;
    }

    return 0;
}