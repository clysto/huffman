#include <huffman.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_BUFFER_SIZE (1024 * 1024)
#define QUEUE_SIZE 512

// 压缩哈夫曼树
// 将权重从哈夫曼树中剔除
void compressHuffmanTree(HuffmanTree HT, HuffmanTree8Bytes *HT8, int n) {
  *HT8 = (HuffmanTree8Bytes)malloc(n * sizeof(HTNode8Bytes));
  for (int i = 0; i < n; ++i) {
    (*HT8)[i].value = HT[i].value;
    (*HT8)[i].parent = HT[i].parent;
    (*HT8)[i].lchild = HT[i].lchild;
    (*HT8)[i].rchild = HT[i].rchild;
  }
}

unsigned char strToByte(const char *byteStr, int len) {
  int i = 0;
  unsigned char byte = 0;
  byte += byteStr[i++] - '0';
  for (; i < len; ++i) {
    byte <<= (unsigned)1;
    byte += byteStr[i] - '0';
  }
  for (; i < 8; ++i) {
    byte <<= (unsigned)1;
  }
  return byte;
}

void Select(HuffmanTree HT, int n, int *s1, int *s2) {
  int i;
  *s1 = 0;
  *s2 = 0;
  unsigned int firstMin = UINT_MAX;
  unsigned int secondMin = UINT_MAX;
  for (i = 1; i <= n; ++i) {
    if (HT[i].parent)
      continue;
    if (HT[i].weight < firstMin) {
      *s2 = *s1;
      *s1 = i;
      secondMin = firstMin;
      firstMin = HT[i].weight;
    } else if (HT[i].weight < secondMin) {
      *s2 = i;
      secondMin = HT[i].weight;
    }
  }
}

// void Select(HuffmanTree HT, int n, int *s1, int *s2) {
//     int i;
//     *s1 = 0;
//     *s2 = 0;
//     for (i = 1; i <= n; ++i) {
//         if (HT[i].parent) continue;
//         if (!*s1 || HT[i].weight < HT[*s1].weight) {
//             *s1 = i;
//         }
//     }
//     for (i = 1; i <= n; ++i) {
//         if (HT[i].parent || i == *s1) continue;
//         if (!*s2 || HT[i].weight < HT[*s2].weight) {
//             *s2 = i;
//         }
//     }
// }

void HuffmanCoding(HuffmanTree *HT, HuffmanCode *HC, const unsigned int *w,
                   const unsigned char *characters, unsigned int n) {
  if (n <= 1)
    return;
  int m = 2 * n - 1, i;
  *HT = (HuffmanTree)malloc((unsigned)(m + 1) * sizeof(HTNode)); // 0号单元不用
  HTNode *p = *HT + 1; // p指向下标为1的元素
  for (i = 1; i <= n; ++i, ++p) {
    p->weight = *(w + i - 1);
    p->lchild = 0;
    p->rchild = 0;
    p->parent = 0;
    p->value = *(characters + i - 1);
  }
  for (; i <= m; ++i, ++p) {
    p->weight = 0;
    p->lchild = 0;
    p->rchild = 0;
    p->parent = 0;
  }
  // 建立HuffmanTree
  for (i = n + 1; i <= m; ++i) {
    int s1, s2;
    Select(*HT, i - 1, &s1, &s2);
    (*HT)[s1].parent = (*HT)[s2].parent = (unsigned)i;
    (*HT)[i].lchild = (unsigned)s1;
    (*HT)[i].rchild = (unsigned)s2;
    (*HT)[i].weight = (*HT)[s1].weight + (*HT)[s2].weight;
  }
  // 哈希表
  *HC = (HuffmanCode)malloc((unsigned)256 * sizeof(char *));
  for (i = 0; i < n; ++i) {
    int start = n - 1;
    unsigned int pre = (unsigned)i + 1;
    char *code = (char *)malloc((unsigned)n * sizeof(char));
    unsigned char value = (*HT)[i + 1].value;
    code[start] = '\0';
    HTNode q = (*HT)[(*HT)[i + 1].parent];
    while (1) {
      code[--start] = q.lchild == pre ? '0' : '1';
      if (!q.parent) {
        break;
      }
      pre = (*HT)[pre].parent;
      q = (*HT)[q.parent];
    }
    (*HC)[value] = &code[start];
  }
}

void getFrequency(char *path, unsigned char **characters,
                  unsigned int **frequency, unsigned int *length) {
  FILE *fp = fopen(path, "rb");
  // temp用来统计频率
  unsigned int temp[256] = {0}, size = 0;
  int byte = 0, index = 0, count;
  unsigned char inputBytesBuffer[FILE_BUFFER_SIZE];

  // 预读FILE_BUFFER_SIZE个字节
  while (1) {
    count = (int)fread(inputBytesBuffer, 1, FILE_BUFFER_SIZE, fp);
    for (int i = 0; i < count; ++i) {
      byte = inputBytesBuffer[i];
      if (!temp[byte]) {
        size++;
      }
      temp[byte]++;
    }
    if (count < FILE_BUFFER_SIZE)
      break;
  }

  *length = size;
  (*frequency) = (unsigned int *)malloc(sizeof(unsigned int) * size);
  (*characters) = (unsigned char *)malloc(sizeof(char) * size);
  for (int i = 0; i < 256; ++i) {
    if (temp[i]) {
      (*frequency)[index] = temp[i];
      (*characters)[index++] = i;
    }
  }
  fclose(fp);
}

void encode(char *inputPath, char *outputPath, HuffmanCode huffmanCode,
            HuffmanTree T, int size) {
  // 打开文件
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");

  unsigned char inputByte = 0, outputByte = 0;
  char buffer[512] = {'\0'};
  int end;

  // 压缩哈夫曼树
  HuffmanTree8Bytes HT8;
  compressHuffmanTree(T, &HT8, size);

  // 写入哈夫曼树的长度
  fwrite(&size, sizeof(int), 1, output);

  // 此处应当写终止位置
  // 记录当前指针位置
  long temp = ftell(output);
  fwrite(&end, 1, 1, output);

  fwrite(HT8, sizeof(HTNode8Bytes), size, output);

  fread(&inputByte, 1, 1, input);
  while (!feof(input)) {
    strcat(buffer, huffmanCode[inputByte]);
    while (strlen(buffer) >= 8) {
      outputByte = 0;
      int i = 0;

      // 构造字节
      for (i = 0; i < 7; ++i) {
        outputByte += buffer[i] - '0';
        outputByte <<= (unsigned)1;
      }
      outputByte += buffer[i] - '0';

      fwrite(&outputByte, 1, 1, output);
      strcpy(buffer, buffer + 8);
    }
    // 读取一个字节
    fread(&inputByte, 1, 1, input);
  }

  // 若缓冲区还存在未编码的bits
  if (strlen(buffer)) {
    end = 8 - (int)strlen(buffer);
    char last[8] = {'\0'};
    strcpy(last, buffer);
    outputByte = 0;
    int i = 0;

    // 构造字节
    for (i = 0; i < 7; ++i) {
      outputByte += (last[i] ? last[i] - '0' : 0);
      outputByte <<= (unsigned)1;
    }
    outputByte += (last[i] ? last[i] - '0' : 0);
    fwrite(&outputByte, 1, 1, output);
  } else {
    end = 0;
  }

  // 写入最后一个字节的终止位置
  fseek(output, temp, SEEK_SET);
  fwrite(&end, 1, 1, output);

  fclose(input);
  fclose(output);
}

void encode2(char *inputPath, char *outputPath, HuffmanCode huffmanCode,
             HuffmanTree T, int size) {
  // 打开文件
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");

  unsigned char inputByte = 0, outputByte = 0;
  char buffer[512] = {'\0'};
  int end;

  // 压缩哈夫曼树
  HuffmanTree8Bytes HT8;
  compressHuffmanTree(T, &HT8, size);

  // 写入哈夫曼树的长度
  fwrite(&size, sizeof(int), 1, output);

  // 此处应当写终止位置
  // 记录当前指针位置
  long temp = ftell(output);
  fwrite(&end, 1, 1, output);

  fwrite(HT8, sizeof(HTNode8Bytes), size, output);

  fread(&inputByte, 1, 1, input);
  while (!feof(input)) {
    strcat(buffer, huffmanCode[inputByte]);
    while (strlen(buffer) >= 8) {
      outputByte = 0;
      int i = 0;

      outputByte = strToByte(buffer, 8);

      fwrite(&outputByte, 1, 1, output);
      strcpy(buffer, buffer + 8);
    }
    // 读取一个字节
    fread(&inputByte, 1, 1, input);
  }

  // 若缓冲区还存在未编码的bits
  if (strlen(buffer)) {
    end = 8 - (int)strlen(buffer);
    outputByte = strToByte(buffer, (int)strlen(buffer));
    fwrite(&outputByte, 1, 1, output);
  } else {
    end = 0;
  }

  // 写入最后一个字节的终止位置
  fseek(output, temp, SEEK_SET);
  fwrite(&end, 1, 1, output);

  fclose(input);
  fclose(output);
}

void decode(char *inputPath, char *outputPath) {
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");
  unsigned char inputByte = 0, outputByte = 0, nextByte = 0;

  int size;
  unsigned char end;
  // 读出哈夫曼树size
  fread(&size, sizeof(int), 1, input);
  // 读出最后一个字节的终止位置
  fread(&end, 1, 1, input);

  // 读出哈夫曼树
  HuffmanTree8Bytes T = (HuffmanTree8Bytes)malloc(size * sizeof(HTNode8Bytes));
  fread(T, sizeof(HTNode8Bytes), size, input);

  // 定义根节点和cursor游标
  unsigned int root = size - 1, cursor;

  unsigned int bit;
  int i, j = 0, result;
  cursor = root;

  fread(&nextByte, 1, 1, input);
  while (!feof(input)) {
    inputByte = nextByte;
    result = (int)fread(&nextByte, 1, 1, input);

    // 拆解字节
    if (!result) {
      j = end;
    }
    for (i = 7; i >= j; --i) {
      bit = (unsigned)inputByte >> (unsigned)i & (unsigned)1;
      if (!T[cursor].lchild && !T[cursor].rchild) {
        outputByte = T[cursor].value;
        fwrite(&outputByte, 1, 1, output);
        cursor = root;
      }
      if (!bit) {
        cursor = T[cursor].lchild;
      } else {
        cursor = T[cursor].rchild;
      }
    }
  }

  // 拆解最后一个字节
  if (!T[cursor].lchild && !T[cursor].rchild) {
    outputByte = T[cursor].value;
    fwrite(&outputByte, 1, 1, output);
  }
  fclose(input);
  fclose(output);
}

void encodeWithBuffer(char *inputPath, char *outputPath,
                      HuffmanCode huffmanCode, HuffmanTree T, int size) {
  // 打开文件
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");

  int end;
  unsigned char byte;
  char byteStrBuffer[512] = {'\0'};
  unsigned char outputBytesBuffer[FILE_BUFFER_SIZE];
  unsigned char inputBytesBuffer[FILE_BUFFER_SIZE];
  int outputBytesBufferLen = 0;

  // 压缩哈夫曼树
  HuffmanTree8Bytes HT8;
  compressHuffmanTree(T, &HT8, size);

  // 写入哈夫曼树的长度
  fwrite(&size, sizeof(int), 1, output);

  // 此处应当写终止位置
  // 记录当前指针位置
  long temp = ftell(output);
  fwrite(&end, sizeof(int), 1, output);

  // 写入哈夫曼树
  fwrite(HT8, sizeof(HTNode8Bytes), size, output);

  int bytesCount;
  while (1) {
    // 预读1024个字节
    bytesCount = (int)fread(inputBytesBuffer, 1, FILE_BUFFER_SIZE, input);
    for (int i = 0; i < bytesCount; ++i) {
      strcat(byteStrBuffer, huffmanCode[inputBytesBuffer[i]]);

      // 字节字符串缓冲区足够输出一个字节
      while (strlen(byteStrBuffer) >= 8) {
        byte = strToByte(byteStrBuffer, 8);
        outputBytesBuffer[outputBytesBufferLen++] = byte;
        // 延迟写
        if (outputBytesBufferLen == FILE_BUFFER_SIZE) {
          fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
          outputBytesBufferLen = 0;
        }
        strcpy(byteStrBuffer, byteStrBuffer + 8);
      }
    }
    if (bytesCount < FILE_BUFFER_SIZE)
      break;
  }

  // 若缓冲区还存在未编码的bits
  if (strlen(byteStrBuffer)) {
    end = 8 - (int)strlen(byteStrBuffer);
    byte = strToByte(byteStrBuffer, (int)strlen(byteStrBuffer));
    outputBytesBuffer[outputBytesBufferLen++] = byte;
  } else {
    end = 0;
  }

  if (outputBytesBufferLen) {
    fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
  }

  // 写入最后一个字节的终止位置
  fseek(output, temp, SEEK_SET);
  fwrite(&end, sizeof(int), 1, output);

  fclose(input);
  fclose(output);
}

void appendStrToQueue(unsigned char *queue, char *str, int *rear) {
  char *p = str;
  while (*p) {
    queue[*rear] = *p;
    *rear = (*rear + 1) % QUEUE_SIZE;
    ++p;
  }
}

unsigned char strToByte2(const unsigned char *byteStr, int len, int head) {
  int i = head;
  unsigned char byte = 0;
  byte += byteStr[i++] - '0';
  for (; i < head + len; ++i) {
    byte <<= (unsigned)1;
    byte += byteStr[i] - '0';
  }
  byte <<= (unsigned)head + 8 - i;
  return byte;
}

/**
 * 加入了环形队列作为缓冲区
 * 该函数为最后所使用的编码函数
 */
void encodeWithBuffer2(char *inputPath, char *outputPath,
                       HuffmanCode huffmanCode, HuffmanTree T, int size) {
  // 打开文件
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");

  int end;
  int head = 0, rear = 0;
  unsigned char byte;
  unsigned char byteStrBuffer[QUEUE_SIZE];
  // unsigned char outputBytesBuffer[FILE_BUFFER_SIZE];
  // unsigned char inputBytesBuffer[FILE_BUFFER_SIZE];

  unsigned char *outputBytesBuffer =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_BUFFER_SIZE);
  unsigned char *inputBytesBuffer =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_BUFFER_SIZE);

  int outputBytesBufferLen = 0;

  // 压缩哈夫曼树
  HuffmanTree8Bytes HT8;
  compressHuffmanTree(T, &HT8, size);

  // 写入哈夫曼树的长度
  fwrite(&size, sizeof(int), 1, output);

  // 此处应当写终止位置
  // 记录当前指针位置
  long temp = ftell(output);
  fwrite(&end, 1, 1, output);

  // 写入哈夫曼树
  fwrite(HT8, sizeof(HTNode8Bytes), size, output);

  int bytesCount;
  do {
    // 预读FILE_BUFFER_SIZE个字节
    bytesCount = (int)fread(inputBytesBuffer, 1, FILE_BUFFER_SIZE, input);
    for (int i = 0; i < bytesCount; ++i) {
      appendStrToQueue(byteStrBuffer, huffmanCode[inputBytesBuffer[i]], &rear);

      // 字节字符串缓冲区足够输出一个字节
      while ((rear < head ? rear + QUEUE_SIZE : rear) - head >= 8) {
        // 构建一个字节
        byte = strToByte2(byteStrBuffer, 8, head);
        // 写入输出缓冲区
        outputBytesBuffer[outputBytesBufferLen++] = byte;
        // 若输出缓冲区满则写入文件
        if (outputBytesBufferLen >= FILE_BUFFER_SIZE) {
          fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
          outputBytesBufferLen = 0;
        }
        // 移动队头
        head = (head + 8) % QUEUE_SIZE;
      }
    }
  } while (bytesCount == FILE_BUFFER_SIZE);

  int byteStrBufferLen = (rear < head ? rear + QUEUE_SIZE : rear) - head;
  // 若缓冲区还存在未编码的bits
  if (byteStrBufferLen) {
    end = 8 - byteStrBufferLen;
    byte = strToByte2(byteStrBuffer, byteStrBufferLen, head);
    outputBytesBuffer[outputBytesBufferLen++] = byte;
  } else {
    end = 0;
  }

  // 若输出缓冲区还存在内容
  if (outputBytesBufferLen) {
    fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
  }

  // 写入最后一个字节的终止位置
  fseek(output, temp, SEEK_SET);
  fwrite(&end, 1, 1, output);

  fclose(input);
  fclose(output);

  free(inputBytesBuffer);
  free(outputBytesBuffer);
}

/**
 * 加入了缓冲区
 * 该函数为最后所使用的解码函数
 */
void decodeWithBuffer(char *inputPath, char *outputPath) {
  // 打开文件
  FILE *input = fopen(inputPath, "rb");
  FILE *output = fopen(outputPath, "wb");

  unsigned char inputByte, outputByte;
  // unsigned char outputBytesBuffer[FILE_BUFFER_SIZE];
  // unsigned char inputBytesBuffer[FILE_BUFFER_SIZE];
  unsigned char *outputBytesBuffer =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_BUFFER_SIZE);
  unsigned char *inputBytesBuffer =
      (unsigned char *)malloc(sizeof(unsigned char) * FILE_BUFFER_SIZE);

  int outputBytesBufferLen = 0;
  unsigned char end;
  int size;

  // 读出哈夫曼树size
  fread(&size, sizeof(int), 1, input);
  // 读出最后一个字节的终止位置
  fread(&end, 1, 1, input);

  // 读出哈夫曼树
  HuffmanTree8Bytes T = (HuffmanTree8Bytes)malloc(size * sizeof(HTNode8Bytes));
  fread(T, sizeof(HTNode8Bytes), size, input);

  // 定义根节点和cursor游标
  unsigned int root = size - 1, cursor;

  unsigned int bit;
  int i, j = 0, count;
  cursor = root;

  do {
    count = (int)fread(inputBytesBuffer, 1, FILE_BUFFER_SIZE, input);
    for (int k = 0; k < count; ++k) {
      // 如果读取到最后一个字节
      if (count < FILE_BUFFER_SIZE && k == count - 1) {
        // 抛弃终止位置后的bits
        j = end;
      }
      inputByte = inputBytesBuffer[k];
      // 拆解字节
      for (i = 7; i >= j; --i) {
        bit = (unsigned)inputByte >> (unsigned)i & (unsigned)1;
        // 到达叶子节点
        if (!T[cursor].lchild && !T[cursor].rchild) {
          outputByte = T[cursor].value;
          // 写入输出缓冲区
          outputBytesBuffer[outputBytesBufferLen++] = outputByte;
          // 若输出缓冲区满则写入文件
          if (outputBytesBufferLen >= FILE_BUFFER_SIZE) {
            fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
            outputBytesBufferLen = 0;
          }
          // 回到树根
          cursor = root;
        }
        if (!bit) {
          cursor = T[cursor].lchild;
        } else {
          cursor = T[cursor].rchild;
        }
      }
    }

  } while (count == FILE_BUFFER_SIZE);

  // 拆解最后一个字节
  if (!T[cursor].lchild && !T[cursor].rchild) {
    outputByte = T[cursor].value;
    outputBytesBuffer[outputBytesBufferLen++] = outputByte;
  }

  // 若输出缓冲区还存在内容
  if (outputBytesBufferLen) {
    fwrite(outputBytesBuffer, 1, outputBytesBufferLen, output);
  }

  fclose(input);
  fclose(output);

  free(inputBytesBuffer);
  free(outputBytesBuffer);
}
