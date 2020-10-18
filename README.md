# 数据结构课程设计

## 使用

使用huffman编码压缩和解压缩文件。使用方法如下：

```bash
# 压缩
huffman -o a.huf a.mp4
# 解压缩
huffman -o a.huf.mp4 -r a.huf
```

## 编译

使用cmake编译：

```bash
mkdir build
cd build
cmake ..
make
```