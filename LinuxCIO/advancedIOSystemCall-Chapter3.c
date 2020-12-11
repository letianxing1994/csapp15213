//高级IO系统调用
//1.分散/聚集IO,也叫向量IO(vector IO)，线性IO就是第一，第二章描述的IO
//优势：
// 1.与传统线性IO相比，向量IO提供了更直观的数据处理方式（如预定义的结构体的变量），即编码模式更自然
// 2.效率更高，单个向量IO操作可以取代多个线性IO操作（一个系统调用可以从多个缓冲区读数据，或者向多个缓冲区写数据）
// 3.性能更好，减少了发起的系统调用次数，通过内部优化，向量IO可以比线性IO提供更好的性能
// 4.这是原子性的操作，没有安全问题

//readv()和writev()
//头文件：<sys/uio.h>
//1.ssize_t readv(int fd, const struct iovec *iov, int count):
//readv从文件描述符fd中读取count个段（segment）到参数iov所指示的缓冲区中，第二个参数为段数组，即向量

//2.ssize_t writev(int fd, const struct iovec *iov, int count):
//writev从iov缓冲区里写入count个段（segment）的数据到文件描述符fd中，第二个参数为段数组，即向量

//结构体iovec：(指的是一个独立的，物理不连续的缓冲区，也称为段)
//struct iovec {
//  void *iov_base;
//  size_t iov_len;
//}
//
//段iovec的集合数组被称为向量（vector）
//每个段中存了一个缓冲区（首字节的地址）和缓冲区的大小（字节为单位）
//readv和writev会按顺序处理向量中的段（从iov[0]~iov[count-1]）

//返回值：
//1.分别返回读写的字节数。即count个iov_len的大小，出错时返回-1，并相应设置errno值。
//2.这两个系统调用除了返回read,write返回的错误errno值，额外还会返回另外两种错误场景
//第一个场景，若count个iov_len的和超出SSIZE_MAX，则不会处理任何数据，返回-1，并把errno值设置为EINVAL
//第二个场景，count的数值必须大于0，且小于等于IOV_MAX，在LINUX中IOV_MAX的值为1024，若count为0，系统调用返回0。但是若count大于IOV_MAX，不会处理
//任何数据，返回-1，并把errno设置为EINVAL

//注：此处的count<=8时会使性能提升很多，内核栈会直接分配一个很小的段数组，避免了动态分配段内存