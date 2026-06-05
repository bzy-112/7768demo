#include "packet.h"
#include <string.h>

/*******************************************************************************
 * 初始化: 清零载荷和长度，记录本节点编号
*******************************************************************************/
void Packet_Init(Packet_t *pkt)
{
    if (pkt == NULL) return;

    memset(pkt->payload, 0, sizeof(pkt->payload));
    pkt->payloadLen = 0;
    pkt->nodeId     = PACKET_NODE_ID;
}

/*******************************************************************************
 * CRC8校验: 多项式 0x07 (x^8 + x^2 + x + 1), 初始值0x00
*******************************************************************************/
uint8_t Packet_CRC8(uint8_t *data, uint16_t len)
{
    uint8_t  crc = 0x00;
    uint16_t i;
    uint8_t  j;

    for (i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (j = 0; j < 8; j++)
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1);
    }
    return crc;
}

/*******************************************************************************
 * 解帧: 校验帧头→长度→CRC8→帧尾，全部通过后提取载荷到 pkt->payload
 * 返回值: 0成功 1帧头错 2长度异常 3CRC8错 4帧尾错
*******************************************************************************/
uint8_t Packet_Parse(Packet_t *pkt, uint8_t *raw, uint16_t len)
{
    uint16_t payloadLen;
    uint8_t  crcCalc, crcRecv;

    if ((pkt == NULL) || (raw == NULL)) return 1;
    if (len < PACKET_OVERHEAD) return 1;

    /* 帧头 */
    if ((raw[0] != PACKET_SYNC1) || (raw[1] != PACKET_SYNC2)) return 1;

    /* 载荷长度：必须精确匹配下游节点数×单节点载荷 */
    payloadLen = ((uint16_t)raw[2] << 8) | raw[3];
    if ((payloadLen != PACKET_DOWNSTREAM_PAYLOAD) ||
        (PACKET_OVERHEAD + payloadLen != len)) return 2;

    /* CRC8: 对 [LEN_H, LEN_L, PAYLOAD] */
    crcCalc = Packet_CRC8(&raw[2], 2 + payloadLen);
    crcRecv = raw[4 + payloadLen];
    if (crcCalc != crcRecv) return 3;

    /* 帧尾 */
    if (raw[5 + payloadLen] != PACKET_END) return 4;

    /* 提取载荷 */
    pkt->payloadLen = payloadLen;
    memcpy(pkt->payload, &raw[4], payloadLen);
    return 0;
}

/*******************************************************************************
 * 追加数据: 向载荷末尾拼接数据，超出 PACKET_MY_PAYLOAD_SIZE 时拒绝
*******************************************************************************/
uint8_t Packet_Append(Packet_t *pkt, uint8_t *data, uint16_t len)
{
    if ((pkt == NULL) || (data == NULL)) return 1;
    if (pkt->payloadLen + len > PACKET_MY_PAYLOAD_SIZE) return 1;

    memcpy(&pkt->payload[pkt->payloadLen], data, len);
    pkt->payloadLen += len;
    return 0;
}

/*******************************************************************************
 * 组帧: 将 pkt->payload 打包，添加帧头+长度+CRC8+帧尾，输出到 frame
 * 前置条件: payloadLen 必须 == PACKET_MY_PAYLOAD_SIZE，数据不完整拒绝组帧
*******************************************************************************/
uint8_t Packet_Build(Packet_t *pkt, uint8_t *frame, uint16_t *frmLen)
{
    uint16_t totalLen;
    uint8_t  crc;

    if ((pkt == NULL) || (frame == NULL) || (frmLen == NULL)) return 1;
    if (pkt->payloadLen != PACKET_MY_PAYLOAD_SIZE) return 1;

    totalLen = PACKET_OVERHEAD + pkt->payloadLen;

    frame[0] = PACKET_SYNC1;
    frame[1] = PACKET_SYNC2;
    frame[2] = (uint8_t)(pkt->payloadLen >> 8);
    frame[3] = (uint8_t)(pkt->payloadLen);
    if (pkt->payloadLen > 0)
        memcpy(&frame[4], pkt->payload, pkt->payloadLen);
    crc = Packet_CRC8(&frame[2], 2 + pkt->payloadLen);
    frame[4 + pkt->payloadLen] = crc;
    frame[5 + pkt->payloadLen] = PACKET_END;

    *frmLen = totalLen;
    return 0;
}

/*******************************************************************************
 * 查找帧头: 在字节流中滑窗匹配 SYNC1+SYNC2，返回索引，未找到返回0xFFFF
*******************************************************************************/
uint16_t Packet_FindHeader(uint8_t *buf, uint16_t len)
{
    uint16_t i;

    if (len < 2) return 0xFFFF;
    for (i = 0; i < len - 1; i++)
        if ((buf[i] == PACKET_SYNC1) && (buf[i + 1] == PACKET_SYNC2))
            return i;
    return 0xFFFF;
}

/*******************************************************************************
 * 获取载荷指针（只读访问），通过出参返回
*******************************************************************************/
void Packet_GetPayload(Packet_t *pkt, uint8_t **pl, uint16_t *len)
{
    if (pkt != NULL)
    {
        *pl  = pkt->payload;
        *len = pkt->payloadLen;
    }
}

/*******************************************************************************
 * 清空载荷，准备下一轮接收
*******************************************************************************/
void Packet_Clear(Packet_t *pkt)
{
    if (pkt != NULL)
    {
        pkt->payloadLen = 0;
        memset(pkt->payload, 0, sizeof(pkt->payload));
    }
}
