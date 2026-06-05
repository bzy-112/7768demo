/**********************************************************
* @ File name -> ringbuffer.h
* @ Brief     -> 环形缓冲区，DMA中断与主循环间数据传递
*                head仅生产者(ISR)修改，tail仅消费者(主循环)修改，无需关中断
* 用法:
*   RingBuffer_t rb;  uint8_t buf[512];
*   RingBuffer_Init(&rb, buf, 512);
*   ISR中:  RingBuffer_Write(&rb, dma_buf, len);
*   主循环: RingBuffer_Read(&rb, process_buf, len);
**********************************************************/

#ifndef _ringbuffer_h_
#define _ringbuffer_h_

#include "sys.h"

typedef struct
{
    uint8_t *buffer;    /* 用户分配的存储空间 */
    uint16_t size;      /* 总容量（字节）     */
    uint16_t head;      /* 写指针，仅ISR修改  */
    uint16_t tail;      /* 读指针，仅主循环修改 */
} RingBuffer_t;

/* 初始化: 绑定存储空间，清零读写指针 */
uint8_t  RingBuffer_Init(RingBuffer_t *rb, uint8_t *buf, uint16_t size);

/* 写入数据(ISR中调用): 块拷贝，自动回绕，返回实际写入字节数 */
uint16_t RingBuffer_Write(RingBuffer_t *rb, uint8_t *src, uint16_t len);

/* 读出数据(主循环调用): 块拷贝，自动回绕，返回实际读出字节数 */
uint16_t RingBuffer_Read(RingBuffer_t *rb, uint8_t *dst, uint16_t len);

/* 判空: 返回1为空 */
uint8_t  RingBuffer_IsEmpty(RingBuffer_t *rb);

/* 获取已缓存数据字节数 */
uint16_t RingBuffer_DataCount(RingBuffer_t *rb);

/* 获取剩余可写空间字节数（保留1字节空位） */
uint16_t RingBuffer_SpaceCount(RingBuffer_t *rb);

/* 清空缓冲区(主循环调用): 重置读写指针 */
void     RingBuffer_Flush(RingBuffer_t *rb);

/* 预读(主循环调用): 读取但不移动读指针，数据仍在缓冲区中 */
uint16_t RingBuffer_Peek(RingBuffer_t *rb, uint8_t *dst, uint16_t len);

/* 跳过(主循环调用): 丢弃N字节，移动读指针 */
uint16_t RingBuffer_Skip(RingBuffer_t *rb, uint16_t len);

#endif
