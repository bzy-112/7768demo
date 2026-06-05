#include "ringbuffer.h"
#include <string.h>

/*******************************************************************************
 * 初始化: 绑定用户分配的存储空间，清零读写指针
*******************************************************************************/
uint8_t RingBuffer_Init(RingBuffer_t *rb, uint8_t *buf, uint16_t size)
{
    if ((rb == NULL) || (buf == NULL) || (size < 2)) return 1;

    rb->buffer = buf;
    rb->size   = size;
    rb->head   = 0;
    rb->tail   = 0;
    return 0;
}

/*******************************************************************************
 * 写入数据(ISR中调用): 块拷贝，自动回绕，返回实际写入字节数
 * 可写空间 = size - 已用 - 1（保留一个空位区分空/满）
*******************************************************************************/
uint16_t RingBuffer_Write(RingBuffer_t *rb, uint8_t *src, uint16_t len)
{
    uint16_t space, written, first;

    if ((rb == NULL) || (src == NULL)) return 0;

    if (rb->head >= rb->tail)
        space = rb->size - rb->head + rb->tail - 1;
    else
        space = rb->tail - rb->head - 1;

    if (len > space) len = space;
    if (len == 0) return 0;
    written = len;

    /* 第一段：head → 缓冲区末尾 */
    first = rb->size - rb->head;
    if (first > len) first = len;
    memcpy(&rb->buffer[rb->head], src, first);
    rb->head += first;
    if (rb->head >= rb->size) rb->head = 0;
    src += first;
    len -= first;

    /* 第二段：缓冲区开头 → 继续写完（回绕） */
    if (len > 0)
    {
        memcpy(&rb->buffer[rb->head], src, len);
        rb->head += len;
        if (rb->head >= rb->size) rb->head = 0;
    }

    return written;
}

/*******************************************************************************
 * 读出数据(主循环调用): 块拷贝，自动回绕，返回实际读出字节数
*******************************************************************************/
uint16_t RingBuffer_Read(RingBuffer_t *rb, uint8_t *dst, uint16_t len)
{
    uint16_t avail, nread, first;

    if ((rb == NULL) || (dst == NULL)) return 0;

    if (rb->head >= rb->tail)
        avail = rb->head - rb->tail;
    else
        avail = rb->size - rb->tail + rb->head;

    if (len > avail) len = avail;
    if (len == 0) return 0;
    nread = len;

    /* 第一段：tail → 缓冲区末尾 */
    first = rb->size - rb->tail;
    if (first > len) first = len;
    memcpy(dst, &rb->buffer[rb->tail], first);
    rb->tail += first;
    if (rb->tail >= rb->size) rb->tail = 0;
    dst += first;
    len -= first;

    /* 第二段：缓冲区开头 → 继续读完（回绕） */
    if (len > 0)
    {
        memcpy(dst, &rb->buffer[rb->tail], len);
        rb->tail += len;
        if (rb->tail >= rb->size) rb->tail = 0;
    }

    return nread;
}

/*******************************************************************************
 * 判空: head == tail 即为空
*******************************************************************************/
uint8_t RingBuffer_IsEmpty(RingBuffer_t *rb)
{
    if (rb == NULL) return 1;
    return (rb->head == rb->tail) ? 1 : 0;
}

/*******************************************************************************
 * 获取已缓存数据字节数
*******************************************************************************/
uint16_t RingBuffer_DataCount(RingBuffer_t *rb)
{
    if (rb == NULL) return 0;
    if (rb->head >= rb->tail)
        return (rb->head - rb->tail);
    else
        return (rb->size - rb->tail + rb->head);
}

/*******************************************************************************
 * 获取剩余可写空间字节数（已减去保留的1字节空位）
*******************************************************************************/
uint16_t RingBuffer_SpaceCount(RingBuffer_t *rb)
{
    if (rb == NULL) return 0;
    return (rb->size - 1 - RingBuffer_DataCount(rb));
}

/*******************************************************************************
 * 清空缓冲区: 重置读写指针，仅在确认ISR不会并发写入时调用
*******************************************************************************/
void RingBuffer_Flush(RingBuffer_t *rb)
{
    if (rb == NULL) return;
    rb->head = 0;
    rb->tail = 0;
}

/*******************************************************************************
 * 预读: 读取数据但不移动 tail，数据仍留在缓冲区中，方便查找帧头等操作
*******************************************************************************/
uint16_t RingBuffer_Peek(RingBuffer_t *rb, uint8_t *dst, uint16_t len)
{
    uint16_t avail, tail_save, first, nread;

    if ((rb == NULL) || (dst == NULL)) return 0;

    if (rb->head >= rb->tail)
        avail = rb->head - rb->tail;
    else
        avail = rb->size - rb->tail + rb->head;

    if (len > avail) len = avail;
    if (len == 0) return 0;
    nread = len;

    tail_save = rb->tail;  /* 临时变量模拟，不动结构体中的 tail */

    first = rb->size - tail_save;
    if (first > len) first = len;
    memcpy(dst, &rb->buffer[tail_save], first);
    tail_save += first;
    if (tail_save >= rb->size) tail_save = 0;
    dst += first;
    len -= first;

    if (len > 0)
        memcpy(dst, &rb->buffer[tail_save], len);

    return nread;
}

/*******************************************************************************
 * 跳过: 丢弃N字节，移动 tail，等价于读了但不处理
*******************************************************************************/
uint16_t RingBuffer_Skip(RingBuffer_t *rb, uint16_t len)
{
    uint16_t avail;

    if (rb == NULL) return 0;
    if (rb->head >= rb->tail)
        avail = rb->head - rb->tail;
    else
        avail = rb->size - rb->tail + rb->head;

    if (len > avail) len = avail;

    rb->tail += len;
    if (rb->tail >= rb->size) rb->tail -= rb->size;

    return len;
}
