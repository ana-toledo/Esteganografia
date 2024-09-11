#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>

void write_ppm(FILE *output_file, unsigned char *image_buffer, int width, int height) {
    fprintf(output_file, "P6\n%d %d\n255\n", width, height);  // Cabeçalho PPM
    fwrite(image_buffer, sizeof(unsigned char), width * height * 3, output_file);
}

// Função para esconder a mensagem usando a técnica de LSB
void hide_message(unsigned char *image_buffer, int buffer_size, const char *message) {
    int message_len = strlen(message);
    int bits_written = 0;

    for (int i = 0; i < buffer_size; i++) {
        if (bits_written / 8 >= message_len) {
            // Todos os bits da mensagem foram escritos
            break;
        }

        // Pega o caractere atual da mensagem
        unsigned char current_char = message[bits_written / 8];

        // Pega o bit correspondente do caractere (bits_written % 8 vai de 0 a 7)
        unsigned char bit = (current_char >> (7 - (bits_written % 8))) & 1;

        // Modifica o bit menos significativo do byte de cor
        image_buffer[i] = (image_buffer[i] & 0xFE) | bit;

        bits_written++;
    }
}

// Leitura da imagem JPEG e conversão para PPM com esteganografia
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <input.jpeg> <output.ppm> <mensagem>\n", argv[0]);
        return 1;
    }

    // Variáveis para a imagem JPEG
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, input_file);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int num_channels = cinfo.output_components;  // Normalmente 3 (RGB)

    // Verifica se a imagem é RGB
    if (num_channels != 3) {
        fprintf(stderr, "Erro: A imagem não é RGB.\n");
        return 1;
    }

    // Aloca memória para a imagem
    unsigned long bmp_size = width * height * num_channels;
    unsigned char *image_buffer = (unsigned char *)malloc(bmp_size);

    // Leitura da imagem JPEG
    while (cinfo.output_scanline < height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = image_buffer + (cinfo.output_scanline) * width * num_channels;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(input_file);

    // Esconde a mensagem na imagem PPM
    hide_message(image_buffer, bmp_size, argv[3]);

    // Escreve a imagem PPM com a mensagem escondida
    FILE *output_file = fopen(argv[2], "wb");
    if (!output_file) {
        perror("Erro ao abrir o arquivo de saída");
        free(image_buffer);
        return 1;
    }

    write_ppm(output_file, image_buffer, width, height);

    // Limpeza
    fclose(output_file);
    free(image_buffer);

    printf("Conversão e esteganografia completa. Mensagem escondida com sucesso.\n");
    return 0;
}