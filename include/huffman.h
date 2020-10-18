#ifndef HUFFMAN_H
#define HUFFMAN_H

typedef struct {
    unsigned int weight;
    unsigned short parent, lchild, rchild;
    unsigned char value;
} HTNode, *HuffmanTree;

typedef struct {
    unsigned short parent, lchild, rchild;
    unsigned char value;
} HTNode8Bytes, *HuffmanTree8Bytes;

typedef char **HuffmanCode;

void HuffmanCoding(HuffmanTree *, HuffmanCode *, const unsigned int *, const unsigned char *, unsigned int);

void getFrequency(char *, unsigned char **, unsigned int **, unsigned int *);

void encode(char *inputPath, char *outputPath, HuffmanCode huffmanCode, HuffmanTree T, int);

void encode2(char *inputPath, char *outputPath, HuffmanCode huffmanCode, HuffmanTree T, int);

void decode(char *inputPath, char *outputPath);

void decodeWithBuffer(char *inputPath, char *outputPath);

void encodeWithBuffer(char *inputPath, char *outputPath, HuffmanCode huffmanCode, HuffmanTree T, int size);

void encodeWithBuffer2(char *inputPath, char *outputPath, HuffmanCode huffmanCode, HuffmanTree T, int size);

#endif  // HUFFMAN_H
