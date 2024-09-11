#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 1024

// Função para extrair a mensagem escondida do buffer da imagem PPM
void extract_message(unsigned char *image_buffer, int buffer_size, char *message, int max_message_length) {
    int bit_count = 0;
    int byte_index = 0;
    unsigned char current_byte = 0;
    
    // Extrai a mensagem escondida
    for (int i = 0; i < buffer_size; i++) {
        // Obtém o bit menos significativo do pixel
        unsigned char bit = image_buffer[i] & 1;

        // Adiciona o bit ao byte atual
        current_byte = (current_byte << 1) | bit;
        bit_count++;

        // Se o byte estiver completo, armazena o caractere e reinicia o byte
        if (bit_count % 8 == 0) {
            message[byte_index++] = current_byte;
            current_byte = 0;

            // Se o caractere nulo (fim da mensagem) for encontrado, pare
            if (byte_index >= max_message_length || message[byte_index - 1] == '\0') {
                break;
            }
        }
    }

    // Adiciona o caractere nulo ao final da mensagem
    message[byte_index] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <input.ppm> <output.txt>\n", argv[0]);
        return 1;
    }

    // Abrir o arquivo PPM
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    // Ler o cabeçalho do PPM
    char header[256];
    int width, height, max_color_value;
    if (!fgets(header, sizeof(header), input_file) || strncmp(header, "P6", 2) != 0) {
        fprintf(stderr, "Formato do arquivo não suportado (não é PPM P6).\n");
        fclose(input_file);
        return 1;
    }

    if (fscanf(input_file, "%d %d", &width, &height) != 2) {
        fprintf(stderr, "Erro ao ler dimensões da imagem.\n");
        fclose(input_file);
        return 1;
    }

    if (fscanf(input_file, "%d", &max_color_value) != 1 || max_color_value != 255) {
        fprintf(stderr, "Valor máximo de cor não suportado.\n");
        fclose(input_file);
        return 1;
    }

    // Ignora a nova linha após o cabeçalho
    fgetc(input_file);

    // Aloca memória para o buffer da imagem
    int buffer_size = width * height * 3;
    unsigned char *image_buffer = (unsigned char *)malloc(buffer_size);
    if (!image_buffer) {
        perror("Erro ao alocar memória");
        fclose(input_file);
        return 1;
    }

    // Lê a imagem para o buffer
    if (fread(image_buffer, 1, buffer_size, input_file) != buffer_size) {
        perror("Erro ao ler o arquivo de imagem");
        free(image_buffer);
        fclose(input_file);
        return 1;
    }

    fclose(input_file);

    // Extrai a mensagem escondida
    char message[MAX_MESSAGE_LENGTH];
    extract_message(image_buffer, buffer_size, message, sizeof(message));

    // Salva a mensagem em um arquivo de texto
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Erro ao abrir o arquivo de saída");
        free(image_buffer);
        return 1;
    }

    fprintf(output_file, "%s", message);
    fclose(output_file);

    // Limpeza
    free(image_buffer);

    printf("Mensagem extraída com sucesso e salva em %s.\n", argv[2]);
    return 0;
}
