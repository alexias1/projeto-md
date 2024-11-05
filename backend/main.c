#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>

void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int calcular_primo(mpz_t n) {
    if (mpz_cmp_ui(n, 1) <= 0)
        return 0; 
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0)
        return 1; 
    if (mpz_even_p(n) || mpz_divisible_ui_p(n, 3))
        return 0;

    return mpz_probab_prime_p(n, 25); 
}

int main() {
    printf("Escolha 1 (Gerar chave publica), 2 (Encriptar) ou 3 (Desencriptar):\n");
    int escolha;
    mpz_t n, produtopq, p, q, e, d, t, p1, q1, mdc;
    mpz_inits(n, produtopq, p, q, e, d, t, p1, q1, mdc, NULL); 

    scanf("%d", &escolha);

    switch (escolha) {
        case 1: // Gerar chave pública
            printf("Digite P, Q, e E:\n");
            gmp_scanf("%Zd %Zd %Zd", p, q, e);

            // Checar se p e q são primos
            while (calcular_primo(p) == 0 || calcular_primo(q) == 0) {
                printf("P e Q nao sao primos! Insira P, Q e E novamente:\n");
                gmp_scanf("%Zd %Zd %Zd", p, q, e);
            }

            mpz_sub_ui(p1, p, 1);
            mpz_sub_ui(q1, q, 1);
            mpz_mul(produtopq, p1, q1);

            while (1) {
                mpz_gcd(mdc, e, produtopq);
                if (mpz_cmp_ui(mdc, 1) == 0) break;

                printf("e nao eh coprimo ao produto (p-1)*(q-1)! Insira um novo E:\n");
                gmp_scanf("%Zd", e);
            }

            mpz_mul(n, p, q);
            FILE *pont_arq = fopen("chave_publica.txt", "w");
            if (pont_arq != NULL) {
                gmp_fprintf(pont_arq, "%Zd %Zd\n", n, e);
                fclose(pont_arq);
                printf("Chave publica gerada e salva no arquivo 'chave_publica.txt'.\n");
            } else {
                printf("Erro ao abrir o arquivo!\n");
            }
            break;

        case 2: // Encriptar
            printf("Digite N e E:\n");
            gmp_scanf("%Zd %Zd", n, e);
            printf("Escolha 1 (Digitar mensagem a encriptar) ou 2 (Fazer upload da mensagem a encriptar):\n");
            int opcao;
            scanf("%d", &opcao);
            limparBuffer();

            switch (opcao) {
                case 1: {
                    char letra;
                    FILE *mensagem_crpt = fopen("mensagem_encriptada.txt", "w+");
                    printf("Digite a mensagem de texto a encriptar e pressione Enter para enviar:\n");
                    while ((letra = getchar()) != '\n') {
                        mpz_t encriptada;
                        mpz_init(encriptada);
                        mpz_set_ui(encriptada, (unsigned char)letra);
                        mpz_powm(encriptada, encriptada, e, n);
                        gmp_fprintf(mensagem_crpt, "%Zd ", encriptada);
                        mpz_clear(encriptada);
                    }
                    fclose(mensagem_crpt);
                    break;
                }
                case 2: {
                    char texto;
                    FILE *arquivoEntrada = fopen("arquivo.txt", "r");
                    FILE *arquivoSaida = fopen("mensagem_encriptada.txt", "w");

                    if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                        printf("Erro ao abrir o arquivo.\n");
                        break;
                    }

                    while ((texto = fgetc(arquivoEntrada)) != EOF) {
                        mpz_t mensagem, encriptada;
                        mpz_init_set_ui(mensagem, (unsigned char)texto);
                        mpz_init(encriptada);
                        mpz_powm(encriptada, mensagem, e, n);
                        gmp_fprintf(arquivoSaida, "%Zd ", encriptada);
                        mpz_clear(mensagem);
                        mpz_clear(encriptada);
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

            break;

        case 3: // Desencriptar
            printf("Digite P, Q, e E:\n");
            gmp_scanf("%Zd %Zd %Zd", p, q, e);

            mpz_sub_ui(p1, p, 1);
            mpz_sub_ui(q1, q, 1);
            mpz_mul(produtopq, p1, q1);

            mpz_invert(d, e, produtopq);

            if (mpz_cmp_ui(d, 0) < 0) {
                mpz_add(d, d, produtopq);
            }

            mpz_mul(n, p, q);

            gmp_printf("Chave privada (D,E): (%Zd, %Zd)\n", d, e);
            printf("Escolha 1 (Digitar mensagem a desencriptar) ou 2 (Fazer upload da mensagem a desencriptar):\n");
            int escolher;
            scanf("%d", &escolher);
            limparBuffer();

            switch (escolher) {
                case 1: {
                    printf("Digite a mensagem a desencriptar e digite -1 ao final para enviar:\n");
                    mpz_t numero_encriptado;
                    mpz_init(numero_encriptado);
                    FILE *mensagem_desencriptada = fopen("mensagem_desencriptografada.txt", "w+");
                     while (1) { 
                    gmp_scanf("%Zd", numero_encriptado);
                    if (mpz_cmp_si(numero_encriptado, -1) == 0) break;
                    
                    mpz_t desencriptado;
                    mpz_init(desencriptado);
                    mpz_powm(desencriptado, numero_encriptado, d, n);

                    // Convertendo para caractere ASCII
                    unsigned long char_val = mpz_get_ui(desencriptado);
                    fprintf(mensagem_desencriptada, "%c", (char)char_val);
                    printf("%c", (char)char_val);

                    mpz_clear(desencriptado);
                }
                mpz_clear(numero_encriptado);
                fclose(mensagem_desencriptada);
                break;
}

                case 2: {
                    mpz_t numero;
                    mpz_init(numero);
                    FILE *arquivoEntrada = fopen("arquivo.txt", "r");
                    FILE *arquivoSaida = fopen("mensagem_desencriptografada.txt", "w");

                    if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                        printf("Erro ao abrir o arquivo.\n");
                        break;
                    }

                    while (gmp_fscanf(arquivoEntrada, "%Zd", numero) != EOF) { 
                    mpz_t desencriptado;
                    mpz_init(desencriptado);
                    mpz_powm(desencriptado, numero, d, n);

                    // Convertendo para caractere ASCII
                    unsigned long char_val = mpz_get_ui(desencriptado);
                    fprintf(arquivoSaida, "%c", (char)char_val);
                    printf("%c", (char)char_val); // opcional para verificação

                    mpz_clear(desencriptado);
                }
                    mpz_clear(numero);
                    fclose(arquivoEntrada);
                    fclose(arquivoSaida);
                    break;
                }
                default:
                    printf("Escolha invalida!\n");
                    break;
            }
            mpz_clears(p1, q1, NULL);
            break;

        default:
            printf("Escolha invalida!\n");
            break;
    }

    mpz_clears(n, produtopq, p, q, e, d, t, p1, q1, mdc, NULL); 
    return 0;
}