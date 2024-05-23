# SoraStar

- lckfb.com 天空星STM32F407VET6
- chixiaoshu update

## 版本更新

0. **SoraStar TEST 版本**
   - 继承 `1.0`和`1.1`版本的功能
   - 落后于 `1.2` 版本
   - 更新不定期
   - 更新时间: 2024-04-27 11:45

1. **SoraStar 1.0 版本 (已弃用)**
   - 更名为 `SoraStar_back`
   - 更新时间: 2024-04-20 13:06

2. **SoraStar 1.1 版本 (已弃用)**
   - 位于 `.\SoraStar_back\SoraStar`
   - 由梁老师更改过后的版本, 但仍存在上电问题, 因此被抛弃并但仍然值得学习
   - 上电问题解释: 上电第一遍会导致 `rec_index` 的初始化错乱, 也有可能是串口打开的时候中断同时运行导致, 问题现象为不管发送什么都不会接收发送成功, 解决方法是按 `RST` 复位。
   - 更新时间: 2024-04-24 10:37

3. **SoraStar 1.2 版本 (当前最新版本)**
   - 继承 1.0和1.1版本的功能
   - 修改并删除完整中断接收函数`HAL_UART_RxCpltCallback`, 更改为 `UART_IT+DMA`
   - 将 `UART_Process` 函数进行相应修改
   - 更新时间: 2024-04-28 03:45
   - 去除自定义的空闲中断DMA串口传输函数，使用HAL库本身的空闲中断DMA函数
   - 更新时间: 2024-04-29 19:20
   - 实现串口通信协议的初步制定与使用
   - 更新时间: 2024-05-04 10:30

## SoraStar 1.2 版本功能

1. **GPIO.LED (结构体) (`PB2`)**
   - 实现开关、翻转、循环、状态返回
   - 更新时间: 2024-04-24 11:17

2. **GPIO.KEY (`PA0`)**
   - 移植老师定制的比赛按键检测函数
   - 更新时间: 2024-04-24 11:30

3. **ADC (DMA) (`ADC_CHANNEL_TEMPSENSOR`, `ADC_CHANNEL_VREFINT`)**
   - 采集芯片内部温度 :
   `tempValue = ((tempValueOrigin / 4095.0) * 3300 - 760) / 2.5 + 25`
   - 更新时间: 2024-04-25 12:00
   - 采集内部参照电压 `Vrefint: 1.21V`
   - 更新时间: 2024-05-02 10:03

4. **TIM (中断) (`TIM12`)**
   - 使用定时器将时钟设置为 `83 (168/2-1)`, 实现 us 和 ms 延时
   - 更新时间: 2024-04-26 10:30

5. **UART (中断+DMA) (`USART1`)**
   - 目前实现了空闲中断接收, 但未配置相关返回
   - 更新时间: 2024-04-27 19:40
   - 实现了串口通信协议，DMA为Nomal模式:  
   格式为`sc..xy`
   - 修改结构体 `UART_msg` 新增 `ender_h` & `ender_l`

   ```C
   typedef struct {
      uint8_t header_h; // 起始位_高
      uint8_t header_l; // 起始位_低
      uint8_t ender_h;  // 结束位_高
      uint8_t ender_l;  // 结束位_低
      uint8_t len;  // 数据长度
      uint8_t cmd;  // 'R':Read, 'W':Write(Transmit) 仅在发送使用
      uint8_t flag; // 接收标志位
      uint8_t data[64]; // 实际数据
   }UART_msg;
   ```

   - 将再次开启DMA函数放到 `USART1_IRQHandler()`

   ```C
   void USART1_IRQHandler(void)
   {
      /* USER CODE BEGIN USART1_IRQn 1 */
      // 再次开启空闲中断接收，不然只会接收到1次
      HAL_UARTEx_ReceiveToIdle_DMA(&huart1, (uint8_t *)&dmaRx.data, REC_BUF_LEN);
      /* USER CODE END USART1_IRQn 1 */
   }
   ```

   - 更新时间: 2024-05-03 19:08

## 当前目标

1. ~~使用 `串口空闲中断+DMA` 将通信协议写好~~
2. 新建一个目录存放目前写好的代码, 作为模板例程 ~~soooon~~
3. ~~焊接排针，接到指定的底板~~
4. 等待更新

## 当前问题

1. 等待更新

## 当前进程

- 等待更新

## SoraStar 相关说明

1. 按键 使用 `PA0`, 高电平触发, 一个下拉电阻, 初始化默认为 Low 并配置下拉电阻
2. LED 使用 `PB2`, 高电平点亮, 一个下拉电阻, 初始化默认为 Low 并配置下拉电阻
3. CubeMX 默认生成的晶振速度是 `25MHz`, 需要手动将其修改为 `8MHz`

### Q&A

1. None.
