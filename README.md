# AD7768 多通道数据采集系统

基于 STM32F103 + AD7768 的 8 通道 24 位高精度 ADC 数据采集系统，支持菊花链多节点组网。

## 硬件

| 模块 | 型号 |
|------|------|
| MCU | STM32F103 (Cortex-M3) |
| ADC | AD7768 8 通道 24bit Σ-Δ |
| 通信 | UART 菊花链 |

### 引脚连接

**寄存器 SPI（GPIO 模拟）**

| 信号 | 引脚 |
|------|------|
| SCK | PB13 |
| DIN | PB15 |
| CS | PB12 |
| DOUT | PB14 |
| RESET | PA5 |
| START | PA2 |
| SYNC_OUT | PA1 |
| SYNC_IN | PA3 |

**数据 SPI**

| 信号 | 引脚 |
|------|------|
| DCLK | PA7 |
| DRDY | PA6 |
| DOUT0 | PA4 |

## 目录结构

```
├── AD7768/              # AD7768 驱动
│   ├── ad7768.h         #   寄存器定义
│   ├── AD7768.c         #   初始化配置
│   ├── mcp_myspi.h/c   #   SPI GPIO 模拟
│   ├── Communication.h/c # SPI 通信接口
│   └── demo7768.h/c    #   数据采集与电压转换
├── SYSTEM/
│   ├── ringbuffer/       # 环形缓冲区（DMA 中断 <-> 主循环）
│   ├── packet/           # 数据包组帧/解帧（菊花链协议）
│   ├── sys/              # 系统初始化
│   └── usart/            # 串口驱动
├── STM32_FWLib/          # STM32 标准外设库
├── CMSIS/ + CM3/         # Cortex-M3 核心支持
└── User/                 # main.c + Keil 工程文件
```

## 菊花链数据流

```
Node10 → Node9 → ... → Node1 → 主机
```

每个节点收到下游数据后追加本节点数据，转发上游。最终 Node1 发出完整数据包：

```
帧 = [0xAA][0x55][LEN_H][LEN_L][Node10...Node1数据][CRC8][0x5A]
```

## 配置

`SYSTEM/packet/packet.h` 顶部，不同节点只需改 3 个宏：

```c
#define PACKET_NODE_ID      1      // 节点编号 1~10
#define PACKET_TOTAL_NODES  10     // 总节点数
#define PACKET_CH_PER_NODE  24     // 每节点通道数

// 数据类型四选一
#define PACKET_DATA_TYPE_INT32    // float / int32 / int16 / uint16
```

其余宏自动推导。

## 单节点主循环逻辑

```c
Packet_t pkt;
packet_data_t myCh[PACKET_CH_PER_NODE];    // 本节点 24 通道数据
uint8_t rxFrame[PACKET_RX_FRAME_SIZE];     // 接收帧缓冲
uint8_t txFrame[PACKET_TX_FRAME_SIZE];     // 发送帧缓冲

Packet_Init(&pkt);

while (1) {
    ad7768_collect(myCh);                                 // 1. 采集

    uint16_t n = RingBuffer_Read(&dma_rb, rxFrame, ...);  // 2. 读下游帧
    if (Packet_Parse(&pkt, rxFrame, n) == 0) {
        Packet_Append(&pkt, (uint8_t*)myCh, ...);         // 3. 追加本节点
        Packet_Build(&pkt, txFrame, &txLen);              // 4. 组帧
        UART_DMA_Send(txFrame, txLen);                    // 5. 发送上游
        Packet_Clear(&pkt);
    }
}
```

## 环形缓冲区 ISR 安全

单生产者/单消费者模型，无需关中断：

| 函数 | 调用位置 | 修改字段 |
|------|----------|----------|
| `RingBuffer_Write` | DMA 中断 | `head` |
| `RingBuffer_Read` | 主循环 | `tail` |

`head` 和 `tail` 互不交叠，保留 1 字节空位区分空/满。
