#include <gtk/gtk.h>
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>

int calcular_primo(mpz_t n) {
    if (mpz_cmp_ui(n, 1) <= 0)
        return 0; 
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0)
        return 1; 
    if (mpz_even_p(n) || mpz_divisible_ui_p(n, 3))
        return 0;

    return mpz_probab_prime_p(n, 25); 
}

//Declarando funções (GTK)
void gerar_chave_publica(GtkWidget *widget, gpointer data);
void encriptar(GtkWidget *widget, gpointer data);
void desencriptar(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    //Janela principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sistema de Criptografia");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    //Botão "Gerar chave pública"
    GtkWidget *btn_gerar_chave = gtk_button_new_with_label("Gerar chave pública");
    g_signal_connect(btn_gerar_chave, "clicked", G_CALLBACK(gerar_chave_publica), window);
    gtk_box_pack_start(GTK_BOX(vbox), btn_gerar_chave, TRUE, TRUE, 5);

    //Botão "Encriptar"
    GtkWidget *btn_encriptar = gtk_button_new_with_label("Encriptar");
    g_signal_connect(btn_encriptar, "clicked", G_CALLBACK(encriptar), window);
    gtk_box_pack_start(GTK_BOX(vbox), btn_encriptar, TRUE, TRUE, 5);

    //Botão "Desencriptar"
    GtkWidget *btn_desencriptar = gtk_button_new_with_label("Desencriptar");
    g_signal_connect(btn_desencriptar, "clicked", G_CALLBACK(desencriptar), window);
    gtk_box_pack_start(GTK_BOX(vbox), btn_desencriptar, TRUE, TRUE, 5);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

//Função gerar chave pública
void gerar_chave_publica(GtkWidget *widget, gpointer data) {
    GtkWidget *parent_window = GTK_WIDGET(data);

    //Cria a caixa de diálogo para receber valor P, Q e E
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Gerar Chave Pública",
                                                    GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Digite os valores de P, Q e E:");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    
    GtkWidget *entry_p = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p), "P");
    gtk_container_add(GTK_CONTAINER(content_area), entry_p);

    GtkWidget *entry_q = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_q), "Q");
    gtk_container_add(GTK_CONTAINER(content_area), entry_q);

    GtkWidget *entry_e = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_e), "E");
    gtk_container_add(GTK_CONTAINER(content_area), entry_e);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *p_text = gtk_entry_get_text(GTK_ENTRY(entry_p));
        const char *q_text = gtk_entry_get_text(GTK_ENTRY(entry_q));
        const char *e_text = gtk_entry_get_text(GTK_ENTRY(entry_e));

        //Convertendo para GMP e gerando chave pública
        mpz_t n, produtopq, p, q, e, p1, q1, mdc;
        mpz_inits(n, produtopq, p, q, e, p1, q1, mdc, NULL);

        if (mpz_set_str(p, p_text, 10) != 0 ||
            mpz_set_str(q, q_text, 10) != 0 ||
            mpz_set_str(e, e_text, 10) != 0) {
            GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_OK,
                                                          "Erro ao converter os valores de P, Q ou E.");
            gtk_dialog_run(GTK_DIALOG(error_msg));
            gtk_widget_destroy(error_msg);
            return;
        } //pode retirar essa parte, sera que tiro? pq se tirar fica sem aparecer mensagem se der algum erro

        //Checando se P e Q são primos
        if (calcular_primo(p) == 0 || calcular_primo(q) == 0) {
            GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_OK,
                                                          "P e Q devem ser primos!");
            gtk_dialog_run(GTK_DIALOG(error_msg));
            gtk_widget_destroy(error_msg);
            mpz_clears(n, produtopq, p, q, e, p1, q1, mdc, NULL);
            return;
        }

        mpz_sub_ui(p1, p, 1);
        mpz_sub_ui(q1, q, 1);
        mpz_mul(produtopq, p1, q1);

        //Verificando se E é coprimo a (p-1)*(q-1)
        mpz_gcd(mdc, e, produtopq);
        if (mpz_cmp_ui(mdc, 1) != 0) {
            GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_OK,
                                                          "E deve ser coprimo ao produto (p-1)*(q-1)!");
            gtk_dialog_run(GTK_DIALOG(error_msg));
            gtk_widget_destroy(error_msg);
            mpz_clears(n, produtopq, p, q, e, p1, q1, mdc, NULL);
            return;
        }

        // Cálculo de n = p * q e gravação no arquivo
        mpz_mul(n, p, q);
        FILE *pont_arq = fopen("chave_publica.txt", "w");
        if (pont_arq != NULL) {
            gmp_fprintf(pont_arq, "%Zd %Zd\n", n, e);
            fclose(pont_arq);
        } else {
            GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                          GTK_DIALOG_MODAL,
                                                          GTK_MESSAGE_ERROR,
                                                          GTK_BUTTONS_OK,
                                                          "Erro ao salvar a chave pública!");
            gtk_dialog_run(GTK_DIALOG(error_msg));
            gtk_widget_destroy(error_msg);
        }

        mpz_clears(n, produtopq, p, q, e, p1, q1, mdc, NULL);

        // Exibe mensagem de sucesso
        GtkWidget *msg = gtk_message_dialog_new(GTK_WINDOW(parent_window), GTK_DIALOG_MODAL,
                                                GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                "Chave pública gerada e salva!");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
    }

    gtk_widget_destroy(dialog);
}

//Função para encriptar mensagem
void encriptar(GtkWidget *widget, gpointer data) {
    GtkWidget *parent_window = GTK_WIDGET(data);

    // Cria a caixa de diálogo para inserir N e E
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Encriptar",
                                                    GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Digite os valores de N e E:");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    GtkWidget *entry_n = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_n), "N");
    gtk_container_add(GTK_CONTAINER(content_area), entry_n);

    GtkWidget *entry_e = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_e), "E");
    gtk_container_add(GTK_CONTAINER(content_area), entry_e);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *n_text = gtk_entry_get_text(GTK_ENTRY(entry_n));
        const char *e_text = gtk_entry_get_text(GTK_ENTRY(entry_e));

        mpz_t n, e;
        mpz_inits(n, e, NULL);
        mpz_set_str(n, n_text, 10);
        mpz_set_str(e, e_text, 10);

        gtk_widget_destroy(dialog); 

        // Cria a segunda janela para escolher a opção
        GtkWidget *choice_dialog = gtk_dialog_new_with_buttons("Escolha de Método",
                                                               GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               "Digitar Mensagem", 1,
                                                               "Upload de Arquivo", 2,
                                                               "Cancelar", GTK_RESPONSE_CANCEL,
                                                               NULL);
        gtk_widget_show_all(choice_dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(choice_dialog));
        gtk_widget_destroy(choice_dialog); 

        if (response == 1) {
            //Janela para digitar a mensagem
            GtkWidget *msg_dialog = gtk_dialog_new_with_buttons("Digite a Mensagem",
                                                                GTK_WINDOW(parent_window),
                                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                "_OK", GTK_RESPONSE_OK,
                                                                "_Cancelar", GTK_RESPONSE_CANCEL,
                                                                NULL);
            GtkWidget *msg_content_area = gtk_dialog_get_content_area(GTK_DIALOG(msg_dialog));
            GtkWidget *msg_entry = gtk_entry_new();
            gtk_entry_set_placeholder_text(GTK_ENTRY(msg_entry), "Digite sua mensagem aqui");
            gtk_container_add(GTK_CONTAINER(msg_content_area), msg_entry);
            gtk_widget_show_all(msg_dialog);

            if (gtk_dialog_run(GTK_DIALOG(msg_dialog)) == GTK_RESPONSE_OK) {
                const char *msg_text = gtk_entry_get_text(GTK_ENTRY(msg_entry));

                FILE *mensagem_crpt = fopen("mensagem_encriptada.txt", "w+");
                for (int i = 0; msg_text[i] != '\0'; i++) {
                    mpz_t encriptada;
                    mpz_init_set_ui(encriptada, (unsigned char)msg_text[i]);
                    mpz_powm(encriptada, encriptada, e, n);
                    gmp_fprintf(mensagem_crpt, "%Zd ", encriptada);
                    mpz_clear(encriptada);
                }
                fclose(mensagem_crpt);

                GtkWidget *success_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL,
                                                               GTK_MESSAGE_INFO,
                                                               GTK_BUTTONS_OK,
                                                               "Mensagem encriptada e salva em mensagem_encriptada.txt!");
                gtk_dialog_run(GTK_DIALOG(success_msg));
                gtk_widget_destroy(success_msg);
            }
            gtk_widget_destroy(msg_dialog);

        } else if (response == 2) {
            //Janela de upload do arquivo
            GtkWidget *upload_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_INFO,
                                                           GTK_BUTTONS_OK,
                                                           "Por favor, faça o upload do arquivo para o diretório.");
            gtk_dialog_run(GTK_DIALOG(upload_msg));
            gtk_widget_destroy(upload_msg);

            //Realiza encriptação do arquivo
            FILE *arquivoEntrada = fopen("mensagem_inicial.txt", "r");
            FILE *arquivoSaida = fopen("mensagem_encriptada.txt", "w");

            if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                              GTK_DIALOG_MODAL,
                                                              GTK_MESSAGE_ERROR,
                                                              GTK_BUTTONS_OK,
                                                              "Erro ao abrir o arquivo.");
                gtk_dialog_run(GTK_DIALOG(error_msg));
                gtk_widget_destroy(error_msg);
            } else {
                char texto;
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

                GtkWidget *success_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL,
                                                               GTK_MESSAGE_INFO,
                                                               GTK_BUTTONS_OK,
                                                               "Mensagem encriptada do arquivo salva em mensagem_encriptada.txt!");
                gtk_dialog_run(GTK_DIALOG(success_msg));
                gtk_widget_destroy(success_msg);
            }
        }

        mpz_clears(n, e, NULL);
    } else {
        gtk_widget_destroy(dialog);  
    }
}

//Função para desencriptar mensagem
void desencriptar(GtkWidget *widget, gpointer data) {
    GtkWidget *parent_window = GTK_WIDGET(data);

    //Janela para inserir P, Q e E
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Desencriptar",
                                                    GTK_WINDOW(parent_window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Digite os valores de P, Q e E:");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    GtkWidget *entry_p = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_p), "P");
    gtk_container_add(GTK_CONTAINER(content_area), entry_p);

    GtkWidget *entry_q = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_q), "Q");
    gtk_container_add(GTK_CONTAINER(content_area), entry_q);

    GtkWidget *entry_e = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_e), "E");
    gtk_container_add(GTK_CONTAINER(content_area), entry_e);

    gtk_widget_show_all(dialog);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
        const char *p_text = gtk_entry_get_text(GTK_ENTRY(entry_p));
        const char *q_text = gtk_entry_get_text(GTK_ENTRY(entry_q));
        const char *e_text = gtk_entry_get_text(GTK_ENTRY(entry_e));

        mpz_t p, q, e, p1, q1, produtopq, d, n;
        mpz_inits(p, q, e, p1, q1, produtopq, d, n, NULL);

        mpz_set_str(p, p_text, 10);
        mpz_set_str(q, q_text, 10);
        mpz_set_str(e, e_text, 10);

        mpz_sub_ui(p1, p, 1);
        mpz_sub_ui(q1, q, 1);
        mpz_mul(produtopq, p1, q1);

        mpz_invert(d, e, produtopq);

        if (mpz_cmp_ui(d, 0) < 0) {
            mpz_add(d, d, produtopq);
        }

        mpz_mul(n, p, q);

        gtk_widget_destroy(dialog);

        //Janela para escolher o método de entrada
        GtkWidget *choice_dialog = gtk_dialog_new_with_buttons("Escolha de Método",
                                                               GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                               "Digitar Mensagem", 1,
                                                               "Upload de Arquivo", 2,
                                                               "Cancelar", GTK_RESPONSE_CANCEL,
                                                               NULL);
        gtk_widget_show_all(choice_dialog);

        gint response = gtk_dialog_run(GTK_DIALOG(choice_dialog));
        gtk_widget_destroy(choice_dialog); 

        if (response == 1) {
            //Janela para digitar a mensagem encriptada
            GtkWidget *msg_dialog = gtk_dialog_new_with_buttons("Digite a Mensagem Encriptada",
                                                                GTK_WINDOW(parent_window),
                                                                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                "_OK", GTK_RESPONSE_OK,
                                                                "_Cancelar", GTK_RESPONSE_CANCEL,
                                                                NULL);
            GtkWidget *msg_content_area = gtk_dialog_get_content_area(GTK_DIALOG(msg_dialog));
            GtkWidget *msg_entry = gtk_entry_new();
            gtk_entry_set_placeholder_text(GTK_ENTRY(msg_entry), "Digite a mensagem encriptada");
            gtk_container_add(GTK_CONTAINER(msg_content_area), msg_entry);
            gtk_widget_show_all(msg_dialog);

            if (gtk_dialog_run(GTK_DIALOG(msg_dialog)) == GTK_RESPONSE_OK) {
                const char *msg_text = gtk_entry_get_text(GTK_ENTRY(msg_entry));

                //Processa e desencripta cada número inserido
                FILE *mensagem_desencriptada = fopen("mensagem_desencriptografada.txt", "w+");
                mpz_t numero_encriptado;
                mpz_init(numero_encriptado);

                char *token = strtok((char *)msg_text, " ");
                while (token != NULL) {
                    mpz_set_str(numero_encriptado, token, 10);
                    mpz_t desencriptado;
                    mpz_init(desencriptado);
                    mpz_powm(desencriptado, numero_encriptado, d, n);

                    unsigned long char_val = mpz_get_ui(desencriptado);
                    fprintf(mensagem_desencriptada, "%c", (char)char_val);
                    printf("%c", (char)char_val);

                    mpz_clear(desencriptado);
                    token = strtok(NULL, " ");
                }

                fclose(mensagem_desencriptada);
                mpz_clear(numero_encriptado);

                GtkWidget *success_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL,
                                                               GTK_MESSAGE_INFO,
                                                               GTK_BUTTONS_OK,
                                                               "Mensagem desencriptada e salva em mensagem_desencriptografada.txt!");
                gtk_dialog_run(GTK_DIALOG(success_msg));
                gtk_widget_destroy(success_msg);
            }
            gtk_widget_destroy(msg_dialog);

        } else if (response == 2) {
            //Janela de upload do arquivo
            GtkWidget *upload_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                           GTK_DIALOG_MODAL,
                                                           GTK_MESSAGE_INFO,
                                                           GTK_BUTTONS_OK,
                                                           "Por favor, faça o upload do arquivo encriptado no diretório.");
            gtk_dialog_run(GTK_DIALOG(upload_msg));
            gtk_widget_destroy(upload_msg);

            //Desencripta o conteúdo de mensagem_encriptada.txt
            FILE *arquivoEntrada = fopen("mensagem_encriptada.txt", "r");
            FILE *arquivoSaida = fopen("mensagem_desencriptografada.txt", "w");

            if (arquivoEntrada == NULL || arquivoSaida == NULL) {
                GtkWidget *error_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                              GTK_DIALOG_MODAL,
                                                              GTK_MESSAGE_ERROR,
                                                              GTK_BUTTONS_OK,
                                                              "Erro ao abrir o arquivo.");
                gtk_dialog_run(GTK_DIALOG(error_msg));
                gtk_widget_destroy(error_msg);
            } else {
                mpz_t numero;
                mpz_init(numero);

                while (gmp_fscanf(arquivoEntrada, "%Zd", numero) != EOF) {
                    mpz_t desencriptado;
                    mpz_init(desencriptado);
                    mpz_powm(desencriptado, numero, d, n);

                    unsigned long char_val = mpz_get_ui(desencriptado);
                    fprintf(arquivoSaida, "%c", (char)char_val);
                    printf("%c", (char)char_val);

                    mpz_clear(desencriptado);
                }

                fclose(arquivoEntrada);
                fclose(arquivoSaida);
                mpz_clear(numero);

                GtkWidget *success_msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                                                               GTK_DIALOG_MODAL,
                                                               GTK_MESSAGE_INFO,
                                                               GTK_BUTTONS_OK,
                                                               "Mensagem desencriptada do arquivo salva em mensagem_desencriptografada.txt!");
                gtk_dialog_run(GTK_DIALOG(success_msg));
                gtk_widget_destroy(success_msg);
            }
        }

        mpz_clears(p, q, e, p1, q1, produtopq, d, n, NULL);
    } else {
        gtk_widget_destroy(dialog);
    }
}
