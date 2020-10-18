#include <huffman.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void test() {
  unsigned int *frequency;
  unsigned char *characters;
  unsigned int length;
  char filePath[] = "D:/code/huffman/test/data.txt";
  char encodePath[] = "D:/code/huffman/test/data.huf";
  char decodePath[] = "D:/code/huffman/test/data.huf.txt";

  getFrequency(filePath, &characters, &frequency, &length);
  HuffmanTree T;
  HuffmanCode code;
  HuffmanCoding(&T, &code, frequency, characters, length);
  for (int i = 0; i < length; ++i) {
    printf("%d<%c>:%s\n", characters[i], characters[i], code[characters[i]]);
  }
  // encodeWithBuffer("D:/code/huffman/test/data.txt",
  // "D:/code/huffman/test/data.bin", code, T, 2 * length);
  encodeWithBuffer2(filePath, encodePath, code, T, 2 * length);
  // encode("D:/code/huffman/test/data.txt", "D:/code/huffman/test/data.bin",
  // code, T, 2 * length); encode2(filePath, encodePath, code, T, 2 * length);

  printf("%d bytes\n", 2 * length * (int)sizeof(HTNode8Bytes));
  printf("%d\n", 2 * length);
  printf("%d\n", (int)sizeof(HTNode8Bytes));

  // decode("D:/code/huffman/test/data.bin", "D:/code/huffman/test/decode.txt");
  decodeWithBuffer(encodePath, decodePath);
}

void decompress(char *target, char *output) {
  decodeWithBuffer(target, output);
}

void compress(char *target, char *output) {
  unsigned int *frequency;
  unsigned char *characters;
  unsigned int length;
  getFrequency(target, &characters, &frequency, &length);
  if (length <= 1) {
    printf("Although I know this is a bug, I don't want to fix it :)\n");
    return;
  }
  HuffmanTree T;
  HuffmanCode code;
  HuffmanCoding(&T, &code, frequency, characters, length);
  // encode(target, output, code, T, 2 * length);
  // encode2(target, output, code, T, 2 * length);
  // encodeWithBuffer(target, output, code, T, 2 * length);
  encodeWithBuffer2(target, output, code, T, 2 * length);
}

int main(int argc, char **argv) {
  int ch, mode = 0;
  char outputPath[128] = {'\0'};
  while ((ch = getopt(argc, argv, "o:r")) != -1) {
    switch (ch) {
    case 'o':
      strcpy(outputPath, optarg);
      break;
    case 'r':
      mode = 1;
      break;
    default:
      printf("unknown option: %c\n", ch);
    }
  }
  if (!strlen(outputPath)) {
    printf("please input the output path.\n");
    return 0;
  }
  char *target = argv[argc - 1];
  if (mode) {
    decompress(target, outputPath);
  } else {
    compress(target, outputPath);
  }
  // test();
  return 0;
}