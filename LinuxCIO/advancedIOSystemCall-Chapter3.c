//高级IO系统调用
//1.分散/聚集IO,也叫向量IO(vector IO)，线性IO就是第一，第二章描述的IO
//优势：
// 1.与传统线性IO相比，向量IO提供了更直观的数据处理方式（如预定义的结构体的变量），即编码模式更自然
// 2.效率更高，单个向量IO操作可以取代多个线性IO操作（一个系统调用可以从多个缓冲区读数据，或者向多个缓冲区写数据）
// 3.性能更好，减少了发起的系统调用次数，通过内部优化，向量IO可以比线性IO提供更好的性能
// 4.这是原子性的操作，没有安全问题


//知识点一：分散/聚集IO读写
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



//知识点二：Event Poll
//epoll把监听注册从实际监听中分离出来，不同于select和poll把文件描述符的监听注册合并起来，并需要遍历所有文件描述符
//epoll有三个系统调用，第一个是初始化epoll上下文，第二个是从上下文加入或者删除监视的文件描述符，第三个是执行真正的事件等待
//头文件：<sys/epoll.h>

//系统调用1：创建epoll上下文
//int epoll_create1(int flags)
//创建新的epoll实例，并返回和该实例关联的文件描述符，这个文件描述符是为了后续调用epoll而创建的。参数flags支持修改epoll的行为，当前，只有
//EPOLL_CLOEXEC是个合法的flag,表示进程被替换时，关闭文件描述符
//返回值：出错时，返回-1，并设置errno为以下值之一：
//EINVAL:参数flags非法。
//EMFILE:用户打开的文件数达到上限。
//ENFILE:系统打开的文件数达到上限
//ENOMEN:内存不足，无法完成本次操作
//当完成监视后，需要通过close(epfd)来关闭epoll实例

//系统调用2：控制epoll
//epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//向指定的epoll上下文中加入或删除文件描述符
//epoll event结构体：
//struct epoll_event {
//  __u32 events;
//  union {
//      void *ptr;
//      int fd;
//      __u32 u32;
//      __u64 u64;
//  } data;
//}
//
//参数op的有效值：
//1.EPOLL_CTL_ADD:把文件描述符fd所指向的文件添加到epfd指定的epoll监听实例集中，监听event中定义的事件。
//2.EPOLL_CTL_DEL:把文件描述符fd所指向的文件从epfd指定的epoll监听集中删除。
//3.EPOLL_CTL_MOD:使用event指定的更新事件修改在已有fd上的监听行为。

//epoll_events结构体中的events变量列出了在指定文件描述符上要监听的事件。多个监听事件可以通过位或运算同时指定。以下为有效的events值：
//1.EPOLLERR:文件出错，即使没有设置，这个事件也是被监听的。
//2.EPOLLET:在监听文件上开启边缘触发。
//3.EPOLLHUP:文件被挂起。即使没有设置，这个事件也是被监听的。
//4.EPOLLIN:文件未阻塞，可读。
//5.EPOLLONESHOT:在事件生成并处理后，文件不会再被监听。必须通过EPOLL_CTL_MOD指定新的文件掩码。
//6.EPOLLOUT:文件未阻塞，可写。
//7.EPOLLPRI:存在高优先级的带外（out-of-band）数据可读

//union data变量是由用户私有使用，当接收到请求的事件，data会被返回给用户，通常是把event.data.fd设置为epoll_ctl中的fd，这样就可以很容易查看哪个
//文件描述符触发了事件

//返回值：当成功时，epoll_ctl()返回0。失败时，返回-1，并相应设置errno为下列值：
//EBADF:epfd不是有效的epoll实例，或者fd不是有效的文件描述符
//EEXIST:op值设置为EPOLL_CTL_ADD，但是fd已经与epfd关联
//EINVAL:epfd不是epoll实例，epfd和fd相同，或op无效
//ENOENT:op值设置为EPOLL_CTL_MOD或EPOLL_CTL_DEL，但是fd没有和epfd关联
//ENOMEN:没有足够的内存来处理请求
//EPERM:fd不支持epoll


//系统调用3：等待epoll事件
//头文件：<sys/epoll.h>
//int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
//作用：等待和指定epoll实例关联的文件描述符上的事件。等待epoll实例epfd中的文件fd上的事件，时限为timeout毫秒。
// 成功时，events指向描述每个事件的epoll_event结构体的内存（数组），且最多可以有maxevents个事件，返回值是事件数；出错时，返回1，并将errno设置为以下值：
// EBADF:epfd是一个无效的文件描述符
// EFAULT:进程对events所指向的内存没有写权限
// EINTR:系统调用在完成前发生信号中断或超时。
// EINVAL:epfd不是有效的epoll实例，或者maxevents值小于或等于0，如果timeout为0，即使没有事件发生，调用也会立即返回0。如果timeout为1，调用
//将一直等待到有事件发生才返回
//注意：epoll_event结构体中的events变量描述了发生的事件。data变量保留了用户在调用epoll_ctl()前的所有内容

//边缘触发事件和条件触发事件：
//EPOLLET:边缘触发，文件描述符中数据从无到有一瞬间，触发了
//否则是条件触发，默认行为：只要文件描述符中数据尚存，不管多少，就是已触发


//知识点三：存储映射
//1.将文件映射到内存中，即内存地址和文件数据一一对应。开发人员可以直接通过内存来访问文件，就像操作内存中的数据块一样，甚至可以写入内存数据区，然后通过
//透明的映射机制将文件写入磁盘
//系统调用：1.mmap()
//含义：请求内核将文件描述符fd所指向的对象的len个字节数据映射到内存中，起始位置从offset开始。如果指定addr，表示优先使用addr作为内存中的起始地址
//参数prot指定了访存权限，flags指定了其他操作行为
//头文件：<sys/mman.h>
//void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
//addr不是强制性的，大多时候传0
//prot参数描述了对内存区域所请求的访问权限：
//1.PROT_NONE:表示无法访问映射区域的页
//2.PROT_READ:页可读
//3.PROT_WRITE:页可写
//4.PROT_EXEC:页可执行
//注：上述权限必须与文件实际可打开方式对应