//
// Created by TianxingLe on 2020/12/2.
//

//1。用户缓冲读写
//缓冲区分为用户缓冲区和内核缓冲区
//系统调用：
//1。当把用户缓冲区的大小设置为文件块大小的整数倍，可以减少调用系统调用时内核读磁盘时候对数据的切割
//2。用户足够大的缓冲区可以减少系统调用次数

//应用的缓冲读写(对系统调用read write等的封装)：
//标准IO库是对系统调用的一种应用：
//1。文件指针*FILE中有一块缓冲区，是调用系统调用的时候存的
//fopen fdopen
//fclose fcloseall
//fgetc ungetc fgets fread
//fputc fputs fwrite

//定位流
//fseek(FILE *stream, long offset, int whence)
//whence:可以取值SEEK_CUR,SEEK_SET,SEEK_END(文件的当前位置，开始位置，结束位置)
//offset:根据whence的偏移量
//成功返回0，出错返回1，并设置对应的errno

//rewind(FILE *stream):将文件定位于文件开始偏移量为0的地方
// 无返回值
// 若要获得调用后的errno，必须先清空errno

//long ftell(FILE *stream):获取当前文件流的位置

//int fflush(FILE *stream):
//将用户缓冲区中的数据全部写入内核缓冲区(类似系统调用write)，但是无法保证内核缓冲区的数据会写到物理介质（磁盘上），如果要写入物理介质，必须用到
//类似fdatasync或者fsync
//O_DIRECT类似于调用了RDMA技术

//检查文件流的错误标志是否被设置
//头文件：<stdio.h>
//int ferror(FILE *stream):如果被设置返回非0值，否则返回0值
//int feof(FILE *stream):判断当前文件流是否被设置了eof标志，即文件已经读到了结尾，如果设置了，返回非0，否则返回0
//void clearer(FILE *stream):清空文件流的错误代码和标志

//从流获取文件描述符
//int fileno(FILE *stream)
//流和文件描述符不要混合使用，
