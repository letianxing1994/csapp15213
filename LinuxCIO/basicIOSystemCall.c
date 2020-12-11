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

//头文件<unistd.h>
//ssize_t pread(int fd, void *buf, size_t count, off_t pos);
//从fd的pos位置读count字节到buf中
//ssize_t pwrite(int fd, const void *buf, size_t count, off_t pos);
//从fd的pos位置开始，从buf写count字节到文件中
//避免了多线程用lseek的竞争

//文件截短
//头文件:<unistd.h>
//<sys/types.h>
//int ftruncate(int fd, off_t len);
//int truncate(const char *path, off_t len);
//含义：将给定的文件描述符或者字符串表示的可读文件长度截短，成功返回0，失败返回-1，并设置errno，若截短后的len比原文件长度小，
//则多余数据丢弃，并不可再读，若截短长度比原文件长度大，则补字符0

//多路复用
//进程睡眠直到发现有fd处于I/O就绪的状态，没有阻塞
//1.int select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
//头文件:<sys/select.h>
//当返回后，对应的三个集合中存放着各自分别可以进行无阻塞操作的fd，
// 第一个参数存放的是所有文件描述符集中最大的那个文件描述符+1
// 第二个参数readfds返回时指定的是这个集合是否有数据可无阻塞读，初始参数指定的是基于观察某种操作的文件集
// 第三个参数writefds返回时指定的是这个集合是否有数据可无阻塞写，初始参数指定的是基于观察某种操作的文件集
// 第四个参数exceptfds返回时指定的是这个集合是否有带外数据，初始参数指定的是基于观察某种操作的文件集
// 第五个参数struct timeval {
//      long tv_sec;  秒
//      long tv_usec; 毫秒
//}
//当第五个参数不为NULL,则当tv_sec秒，tv_usec毫秒后select调用一定会返回，即使没有任何一个fd处于I/O就绪状态,实际上这个结构体会被系统自动修改，假设结构体
// 设置5秒当第一次返回耗时3秒，则这个结构体剩余时间为2秒，因此后续调用必须想办法初始化该参数。若超时值都为0， 调用直接返回

//文件描述符集通过辅助宏进行操作，这些文件描述符集实现为位数组
//1.删除指定集合中所有的文件描述符：FD_ZERO(&fds)
//2.向指定集中添加一个文件描述符：FD_SET(fd, &fds)
//3.向指定集中删除一个文件描述符：FD_CLR(fd, &fds)
//4.向指定集中检查文件描述符是否就绪：FD_ISSET(fd, &fds)
//Linux中文件描述符数存在上限值，这个值由FD_SETSIZE设置，这个值为1024

// 返回值和错误码：
// 当select()调用成功返回三个集合中I/O就绪的文件描述符总数，如果给出超时设置，返回的可能是0。出错时，返回-1，并把errno设置为以下值之一：
// 1.EBADF：某个集合中存在非法文件描述符
// 2.EINTR: 等待时捕获了一个信号，可以重新发起调用
// 3.EINVAL: 参数n是负数，或者设置的超时时间值非法。
// 4.ENOMEM：没有足够的内存来完成该请求

// 黑科技：把select当成sleep来用，三个集合设置为NULL，超时值设置为非NULL
// pselect(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
// const struct timespec *timeout, const sigset_t *sigmask);
// 此处多了一个参数sigset_t型的参数，是为了解决文件描述符和信号之间等待而出现竞争条件，若信号处理程序设置了全局标志位，进程每次调用select（）前会检查
// 该标志位，假定在检查标志位和调用之间收到信号，应用会一直阻塞，永远不会响应该信号。pselect的最后一个参数屏蔽了部分信号，当这些信号出现，不会产生阻塞，pselect
//返回时，屏蔽信号也变回原来的信号掩码
//struct timespec {
//  long tv_sec;  秒
//  long tv_nsec; 纳秒
//};

// int poll(struct pollfd *fds, nfds_t nfds, int timeout);
//头文件：<poll.h>
//参数意义：fds表示指向pollfd数组的一个指针，nfds是这个数组的长度，timeout是参数指定等待的时间长度，单位是毫秒
//结构体：
// struct pollfd {
//  int fd;
//  short events;
//  short revents;
//}
//结构体pollfd中各个成员变量的含义：
//1.fd表示观测的文件描述符
//2.events是一个要监视的文件描述符的事件的位掩码，用户可以设置该变量。
//3.revents是该文件描述符的结果事件的位掩码，内核在返回时会设置revents变量。events变量中请求的所有事件都可能在revents变量中返回。
//合法的events：
//POLLIN:有数据可读
//POLLRDNORM:有普通数据可读
//POLLRDBAND:有优先数据可读
//POLLPRI:有高优先级数据可读
//POLLOUT:写操作不会阻塞
//POLLWRNORM:写普通数据不会阻塞
//POLLWRBAND:写优先数据不会阻塞
//POLLMSG:有SIGPOLL消息可用

//revents除了可以与上述变量测试，还可以额外与下述变量测试：
//POLLER:给定的文件描述符出现错误
//POLLHUP:给定的文件描述符有挂起事件
//POLLNVAL:给定的文件描述符非法

//返回值：返回revents变量不为0的所有文件描述符个数
//ppoll():指定屏蔽阻塞信号的信号集

//页缓存是指内核将磁盘中使用较多的数据读到缓存中进行存储， 当应用进程使用数据时，优先从页缓存中读取，而非磁盘，运用了"时间局部性"
//当用于页缓存的内存空间越来越小以至于不够用的时候，就要进行数据清除或者交换（将不常用的数据放入磁盘中的虚拟内存或删除），是交换还是清除
//由/proc/sys/vm/swappiness来调整（值小的则优先清除缓存，值大的进行数据交换，即将不常用的数据写入磁盘中的虚拟内存）