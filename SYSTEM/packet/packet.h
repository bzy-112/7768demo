/**********************************************************
* @ File name -> packet.h
* @ Brief     -> 菊花链多节点数据包组帧/解帧
*
* 帧格式: [SYNC1 0xAA][SYNC2 0x55][LEN_H][LEN_L][PAYLOAD][CRC8][END 0x5A]
*   CRC8 多项式 0x07, 校验范围 = [LEN_H, LEN_L, PAYLOAD]
*
* 数据流向: Node10 → Node9 → ... → Node1 → 主机
*   每个节点将下游数据 + 本节点数据拼接后转发上游
*   载荷排列: [Node_N+1 ... Node_10 数据][本节点数据]
*
* 用法:
*   Packet_t pkt;
*   Packet_Init(&pkt);
*   Packet_Parse(&pkt, rxFrame, len);        // 解析下游帧
*   Packet_Append(&pkt, myData, dataLen);    // 追加本节点数据
*   Packet_Build(&pkt, txFrame, &txLen);     // 组帧发送上游
**********************************************************/

#ifndef _packet_h_
#define _packet_h_

#include "sys.h"

/* ======== 用户配置（按节点修改） ======== */
#define PACKET_NODE_ID          1       /* 本节点编号 1~10                   */
#define PACKET_TOTAL_NODES      10      /* 总节点数                         */
#define PACKET_CH_PER_NODE      24      /* 每节点通道数                      */

/* 数据类型四选一 */
//#define PACKET_DATA_TYPE_FLOAT
#define PACKET_DATA_TYPE_INT32
//#define PACKET_DATA_TYPE_INT16
//#define PACKET_DATA_TYPE_UINT16

/* ======== 以下自动推导，无需修改 ======== */
#if defined(PACKET_DATA_TYPE_FLOAT)
    #define PACKET_DATA_SIZE  4         
     typedef float   packet_data_t;
#elif defined(PACKET_DATA_TYPE_INT32)
    #define PACKET_DATA_SIZE  4         
     typedef int32_t packet_data_t;
#elif defined(PACKET_DATA_TYPE_INT16)
    #define PACKET_DATA_SIZE  2         
     typedef int16_t packet_data_t;
#elif defined(PACKET_DATA_TYPE_UINT16)
    #define PACKET_DATA_SIZE  2         
     typedef uint16_t packet_data_t;
#else
    #error "启用一种数据类型"
#endif

#define PACKET_NODE_PAYLOAD_SIZE   (PACKET_CH_PER_NODE  * PACKET_DATA_SIZE)               /* 单节点载荷字节数               */
#define PACKET_DOWNSTREAM_NODES    (PACKET_TOTAL_NODES  - PACKET_NODE_ID)                 /* 本节点下游节点数               */
#define PACKET_DOWNSTREAM_PAYLOAD  (PACKET_DOWNSTREAM_NODES * PACKET_NODE_PAYLOAD_SIZE)   /* 下游节点载荷总字节数           */
#define PACKET_MY_NODE_COUNT       (PACKET_DOWNSTREAM_NODES + 1)                          /* 本节点管辖节点总数(下游+自己)   */
#define PACKET_MY_PAYLOAD_SIZE     (PACKET_MY_NODE_COUNT * PACKET_NODE_PAYLOAD_SIZE)      /* 本节点载荷上限                 */

#define PACKET_SYNC1              0xAA    /* 帧头同步字1                    */
#define PACKET_SYNC2              0x55    /* 帧头同步字2                    */
#define PACKET_END                0x5A    /* 帧尾                           */
#define PACKET_OVERHEAD           6       /* 帧开销: SYNC1+SYNC2+LEN_H+LEN_L+CRC8+END */

#define PACKET_RX_FRAME_SIZE      (PACKET_DOWNSTREAM_PAYLOAD + PACKET_OVERHEAD) /* 本节点接收帧缓冲区大小 */
#define PACKET_TX_FRAME_SIZE      (PACKET_MY_PAYLOAD_SIZE    + PACKET_OVERHEAD) /* 本节点发送帧缓冲区大小 */

typedef struct
{
    uint8_t  payload[PACKET_MY_PAYLOAD_SIZE]; /* 载荷缓冲区，存纯数据        */
    uint16_t payloadLen;                      /* 当前有效载荷字节数          */
    uint8_t  nodeId;                          /* 本节点编号                  */
} Packet_t;

/* 初始化: 清零载荷和长度，记录节点编号 */
void     Packet_Init(Packet_t *pkt);

/* 解析原始帧: 校验帧头→长度→CRC8→帧尾，提取载荷存入pkt */
uint8_t  Packet_Parse(Packet_t *pkt, uint8_t *raw, uint16_t len);

/* 追加数据: 向载荷末尾拼接本节点采集数据 */
uint8_t  Packet_Append(Packet_t *pkt, uint8_t *data, uint16_t len);

/* 组帧: 将载荷打包成完整帧（加帧头+长度+CRC8+帧尾），准备发送 */
uint8_t  Packet_Build(Packet_t *pkt, uint8_t *frame, uint16_t *frmLen);

/* 在字节流中查找帧头 SYNC1+SYNC2，返回索引，未找到返回0xFFFF */
//uint16_t Packet_FindHeader(uint8_t *buf, uint16_t len);

/* 计算CRC8校验值，多项式0x07 */
uint8_t  Packet_CRC8(uint8_t *data, uint16_t len);

/* 获取载荷指针（只读访问） */
void     Packet_GetPayload(Packet_t *pkt, uint8_t **pl, uint16_t *len);

/* 清空载荷，准备下一轮接收 */
void     Packet_Clear(Packet_t *pkt);

#endif
