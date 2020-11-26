//
// Created by TianxingLe on 2020/11/25.
//
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


//int open(const char *name, int flags, ...<mode_t mode>)
//open系统调用，flags必备选项五个：O_RDONLY,O_WRONLY,O_RDWR,O_EXEC,O_SEARCH，第三个参数mode只有在第二个为O_CREATE是才定义，mode提供文件
//本身的访问权限，用这种open方式打开的新建文件不会检查权限(IRWXU,IRUSR,IWUSR,IXUSR,IRWXG,IRGRP,IWGRP,IXGRP,IRWXO,IROTH,IWOTH,IXOTH)，可以执行与打开方式不同的操作
//openat系统调用，多了一个fd参数，和第二个参数配合使用，第二个参数是否是绝对路径还是相对路径，与fd是否是AT_FDCWD决定了文件位置
//成功返回文件描述符，失败返回-1


//creat系统调用
//int creat(const char *name, mode_t mode)和int open(const char *name, O_WRONLY|O_CREAT|O_TRUNC, mode_t mode)是一样的
//成功返回文件描述符，失败返回-1


//头文件：unistd.h
//read系统调用，会根据返回值和该读数的区别，如果返回-1，会根据errno(EINTR,EAGAIN,...)来确定为何读取失败
//write系统调用
//read,write都是只保证应用缓存区和内核缓存区的数据交换
//读写交替，为了保持效率，延迟写：当内核缓冲区到一定数量以后再将数据运到磁盘，
// 读有可能是从内核缓存区直接读数据到应用缓存区，省去了内核到磁盘，磁盘到内核的过程

//O_APPEND,每次写操作前，都会把i节点中的长度更新到文件项的偏移量，保证写同步


//同步I/O：
//希望可以控制内核缓存区中数据与磁盘的流动
//fsync(int fd)和fdatasync(int fd)，前两个函数只是保证了文件数据写入了磁盘，无法保证包含文件的目录也更新。sync(void)保证了同步缓冲区全部进入了磁盘
//open的flag里面有一个O_SYNC,与fsync(int fd)含义一致
//open的O_DSYNC与隐式调用fdatasync(int fd)含义一致
//open的O_RSYNC指的是读取的数据必须是已经更新到磁盘中的

//直接I/O:
//保证应用程序和磁盘间直接进行数据交互，O_DIRECT，可以让内核对I/O的管理到达最小

//关闭当前进程的文件：close(fd)，取消当前进程与文件项的映射，成功返回0，失败返回-1并设置errno值。对内核内存中的文件项进行内存清除，若当前文件项是最后一个对
// 文件索引节点的链接项，则把内存文件索引节点也清除


// 头文件<unistd.h>
// off_t lseek(int fd, off_t pos, int origin), origin可以是SEEK_CUR,SEEK_END,SEEK_SET，若从SEEK_END尾部开始读，可能产生空洞（0表示）
//  包含空洞稀疏文件会让内存的文件总大小大于磁盘的总大小，若返回-1，表示调用出错，会设置errno
//  （EBADF, EINVAL, EOVERFLOW(超出off_t的范围，计算机操作系统通用寄存器位数大小), ESPIPE）