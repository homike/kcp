//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )
// 用于在已知结构体里面成员member和该成员指针ptr（就是地址）和结构体类型type, 返回该成员所在的结构体的指针（就是地址）
#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// BYTE ORDER & ALIGNMENT
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif

#ifndef IWORDS_MUST_ALIGN
	#if defined(__i386__) || defined(__i386) || defined(_i386_)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(_M_IX86) || defined(_X86_) || defined(__x86_64__)
		#define IWORDS_MUST_ALIGN 0
	#elif defined(__amd64) || defined(__amd64__)
		#define IWORDS_MUST_ALIGN 0
	#else
		#define IWORDS_MUST_ALIGN 1
	#endif
#endif


//=====================================================================
// SEGMENT
// IKCPSEG 是 KCP 一个报文的结构
// KCP 报文结构
// +----------+---+---+------+
// |   conv   |cmd|frg|  wnd |
// +----------+--------------+
// |   ts     |     sn       |
// +----------+--------------+
// |   una    |     len      |
// +-------------------------+
// |        Data             |
// +-------------------------+
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node; // 节点用来串接多个 KCP segment，也就是前向后向指针；
	IUINT32 conv;           // 会话编号, 通信双方需要保证conv相同, 相互的数据包才能被认可
	IUINT32 cmd;            // 分片的类型, IKCP_CMD_PUSH:数据分片 IKCP_CMD_ACK:ack分片 IKCP_CMD_WASK请求告知窗口大小 IKCP_CMD_WINS:告知窗口大小
	IUINT32 frg;            // 分片的编号，当输出数据大于 MSS 时，需要将数据进行分片，frg 记录了分片时的倒序序号
	IUINT32 wnd;            // 剩余接收窗口大小(接收窗口大小-接收队列大小)
	IUINT32 ts;             // 记录了发送时的时间戳，用来估计 RTT
	IUINT32 sn;             // message分片segment的序号，按1累次递增 
	IUINT32 una;            // 待接收消息序号(接收滑动窗口左端)。对于未丢包的网络来说，una是下一个可接收的序号，如收到sn=10的包，una为11 
	IUINT32 len;            // 数据长度 
	IUINT32 resendts;       // 下一次重发该报文的时间
	IUINT32 rto;            // 重传超时时间 
	IUINT32 fastack;        // 记录了该报文在收到 ACK 时被跳过了几次，用于快重传 
	IUINT32 xmit;           // 记录了该报文被传输了几次 
	char data[1];           // 数据
};


//---------------------------------------------------------------------
// IKCPCB
// IKCPCB是KCP中最重要的结构，也是在会话开始就创建的对象，代表着这次会话，所以这个结构体体现了一个会话所需要涉及到的所有组件
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 conv,                   // 标识这个会话
    IUINT32 mtu, mss;               // mtu: IP层最大传输单元, mss: 传输层提交给IP层最大分段大小, 通常是server 和 client协商双方mss的最小值
    IUINT32 state;                  // 连接状态（0xFFFFFFFF表示断开连接）
	IUINT32 snd_una,                // 第一个未确认的包 
    IUINT32 snd_nxt,                // 下一个待分配的包的序号；
    IUINT32 rcv_nxt;                // 待接收消息序号。为了保证包的顺序，接收方会维护一个接收窗口，接收窗口有一个起始序号rcv_nxt（待接收消息序号）以及尾序号 rcv_nxt + rcv_wnd（接收窗口大小）；
	IUINT32 ts_recent;
    IUINT32 ts_lastack;
    IUINT32 ssthresh;               // 拥塞窗口阈值，以包为单位（TCP以字节为单位）
	IINT32  rx_rttval;              // RTT的变化量，代表连接的抖动情况
    IINT32  rx_srtt,                // smoothed round trip time，平滑后的RTT
    IINT32  rx_rto,                 // 由ACK接收延迟计算出来的重传超时时间
    IINT32  rx_minrto;              // 最小重传超时时间
	IUINT32 snd_wnd;                // 发送窗口大小
    IUINT32 rcv_wnd;                // 接收窗口大小
    IUINT32 rmt_wnd;                // 远端接收窗口大小
    IUINT32 cwnd;                   // 拥塞窗口大小
    IUINT32 probe;                  // 探查变量，IKCP_ASK_TELL表示告知远端窗口大小。IKCP_ASK_SEND表示请求远端告知窗口大小
	IUINT32 current, xmit; 
    IUINT32 interval;               // interval: 内部flush刷新间隔，对系统循环效率有非常重要影响
    IUINT32 ts_flush;               // ts_flush: 下次flush刷新时间戳
	IUINT32 nrcv_buf, nsnd_buf;
	IUINT32 nrcv_que,               // 接收队列中消息数量; 
    IUINT32 nsnd_que;               // 发送队列中消息数量；
	IUINT32 nodelay;                // 是否启动无延迟模式。无延迟模式rtomin将设置为0，拥塞控制不启动
	IUINT32 updated;                // 是否调用过update函数的标识 
	IUINT32 ts_probe, probe_wait;
	IUINT32 dead_link, incr;
	struct IQUEUEHEAD snd_queue;    // 发送消息的队列
	struct IQUEUEHEAD rcv_queue;    // 接收消息的队列
	struct IQUEUEHEAD snd_buf;      // 发送消息的缓存
	struct IQUEUEHEAD rcv_buf;      // 接收消息的缓存
	IUINT32 *acklist;               // 当收到一个数据报文时，将其对应的 ACK 报文的 sn 号以及时间戳 ts 同时加入到acklist 中，即形成如 [sn1, ts1, sn2, ts2 …] 的列表
	IUINT32 ackcount;               // 记录 acklist 中存放的 ACK 报文的数量
	IUINT32 ackblock;               // acklist 数组的可用长度，当 acklist 的容量不足时，需要进行扩容
	void *user;
	char *buffer;
	int fastresend;                 // 触发快速重传的重复ACK个数
	int fastlimit;
	int nocwnd, stream;             // nocwnd: 取消拥塞控制
	int logmask;
	int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
	void (*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// interface
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
// 工作模式
// nodelay ：是否启用 nodelay模式，0不启用；1启用。
// interval ：协议内部工作的 interval，单位毫秒，比如 10ms或者 20ms
// resend ：快速重传模式，默认0关闭，可以设置2（2次ACK跨越将会直接重传）
// nc ：是否关闭流控，默认是0代表不关闭，1代表关闭。
// 普通模式： ikcp_nodelay(kcp, 0, 40, 0, 0);
// 极速模式： ikcp_nodelay(kcp, 1, 10, 2, 1);
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


