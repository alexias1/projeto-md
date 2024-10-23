#include <stdio.h>
#include <math.h>
#include <stdlib.h>

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
    printf("Escolha 1 (Gerar chave pública), 2 (Encriptar) ou 3 (Desencriptar):\n");
    int escolha;
    long int p, q, e;
    long long int n, produtopq;
    scanf("%d", &escolha);

    switch (escolha) {
    case 1: // Gerar chave pública
        printf("Digite P, Q, e E:\n");
        scanf("%ld%ld%ld", &p, &q, &e);
        // Checar se p e q são primos
        while (calcular_primo(p) == 0 || calcular_primo(q) == 0) {
            printf("P e Q não são primos! Insira P, Q e E novamente:\n");
            scanf("%ld%ld%ld", &p, &q, &e);
        }
        produtopq = (p - 1) * (q - 1);
        // Checar se e é coprimo ao produto (p-1)*(q-1)
        while (algoritmo_euclides(e, produtopq) != 1) {
            printf("e nao e coprimo ao produto (p-1)*(q-1)! Insira P, Q e E novamente:\n");
            scanf("%ld%ld%ld", &p, &q, &e);
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
        printf("Digite o valor a ser encriptado (um número):\n");
        long long int mensagem;
        
        //scanf("%lld", &mensagem);
        
        //recebe txt para ser encriptografado
        FILE *pont_m = fopen("teste.txt", "r");
        fscanf(pont_m, "%lld", &mensagem);
        fclose(pont_m);

        long long int encriptada = exponenciacao_modular(mensagem, e, n);
        
        //exporta txt encriptografado
        FILE *pont = fopen("mensagem_encriptografada.txt", "w");
        if (pont != NULL) {
            fprintf(pont, "%lld\n", encriptada);
            fclose(pont);
            printf("Mensagem gerada e salva no arquivo 'mensagem_encriptografada.txt'.\n");
        } else {
            printf("Erro ao abrir o arquivo!\n");
        }
        
        printf("Mensagem encriptada: %lld\n", encriptada);
        break;

    case 3: // Desencriptar
        printf("Digite P, Q, e E:\n");
        scanf("%ld %ld %ld", &p, &q, &e);
        produtopq = (p - 1) * (q - 1);
        int d, t;
        algoritmo_euclides_estendido(e, produtopq, &d, &t);
        if (d < 0) {
            d += produtopq;
        }        
        long long int n = p * q;
        printf("Chave privada (D,E): (%d,%lld)\n", d, n);
        //pegar o texto puro. por enquanto sera um numero so

        int numero;
        scanf("%d", &numero);
        long long int desencriptado = exponenciacao_modular(numero, d, n);
        //printf("%lld\n", desencriptado);
        printf("%lld\n", desencriptado);
        break;

    default:
        printf("Escolha invalida!\n");
        break;
    }

    return 0;
}