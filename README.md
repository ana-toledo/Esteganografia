# Esteganografia em Imagens PPM

Este repositório contém um exemplo de esteganografia utilizando imagens no formato PPM (Portable Pixmap Format). O projeto inclui dois programas em C:

1. **`codifica.c`**: Codifica uma mensagem em uma imagem PPM.
2. **`decodifica.c`**: Decodifica uma mensagem escondida de uma imagem PPM.

## Requisitos

Para compilar e executar os programas, você precisará de:

- Um compilador C (por exemplo, `gcc`).
- Biblioteca **libjpeg** (se necessário para conversões JPEG para PPM).

## Compilação

### Codificador

Para compilar o codificador (`codifica.c`):

```bash
gcc -o encode_steganography encode_steganography.c -ljpeg

### Decodificador

Para compilar o decodificador(`decodifica.c`);