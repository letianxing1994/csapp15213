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
//流和文件描述符不要混合使用，否则会出现错误

//流缓存区的大小设置
//setvbuf(FILE *stream, char *buf, int mode, size_t size)
//mode可以设置成如无缓冲，行缓冲，块缓冲的类型
//_IONBF:无缓冲
//_IOLBF:行缓冲
//_IOFBF:块缓冲
//buf如果设置成局部变量，会出现泄漏的隐患，假设不把流及时关掉，函数结束后，缓冲会被垃圾回收掉


//保证线程安全的两个方法：
//1.数据同步机制，对一个同步访问的数据加锁
//2.把数据存储在线程的局部变量中（线程封闭）
//在标准IO中，多个线程调用标准IO操作，是不会发生冲突的，因为每个IO操作内部都有一把锁，调用的时候自动加锁（本身是原子操作）

//提高性能和安全的两个问题：
//1。一个线程中有多个IO操作，如何与另外一个线程保证相对安全
//2。在一些特殊场景下取消所有锁以保证程序效率

//额外情况（A对应的情况是对多个IO操作变为原子操作，B对应的情况是对一个IO原子操作变为非原子操作）
//A.同一个线程中多个IO操作与其他线程如何保持有序性，进行重入锁加锁（对流加锁,将多个IO操作变为原子性操作）
//若一个线程中的函数要连续进行多次标准IO操作不被其他线程的IO抢占资源（计数锁，类似于重入锁）
//阻塞版，当一个线程无法获取锁，则调用flockfile的线程阻塞
//void flockfile(FILE *stream):等待指定stream被解锁（当前锁住的stream不是要锁的stream），然后增加自己的锁计数，获得锁，该函数的执行线程成为流的持有者，并返回
//void funlockfile(FILE *stream):对被flockfile加计数锁的线程进行计数锁减1，当锁的计数变为0的时候，当前线程失去了对这个流的控制，该流被解锁
//非阻塞版，当一个线程无法获取锁，调用ftrylockfile的线程直接返回一个非0值
//int ftrylockfile(FILE *stream):若指定stream流当前已经加锁，此函数不做任何处理，立即返回一个非0值；若stream没有加锁，执行ftrylockfile的线程
//会获得锁，增加锁计数值，成为该stream的持有者，并返回0；

//B.对流操作解锁
//对应的io操作后面加_unblock，表示不是线程间安全的
//int fgetc_unlocked(FILE *stream);
//char *fgets_unlocked(char *str, int size, FILE *stream);
//size_t fread_unlocked(void *buf, size_t size, size_t nr, FILE *stream);
//int fputc_unlocked(int c, FILE *stream)
//int fputs_unlock(const char *str, FILE *stream);
//size_t fwrite_unlock(void *buf, size_t size, size_t nr, FILE *stream);
//int fflush_unlocked(FILE *stream);
//int feof_unlocked(FILE *stream);
//int ferror_unlocked(FILE *stream);
//int fileno_unlocked(FILE *stream);
//void clearerr_unlocked(FILE *stream);