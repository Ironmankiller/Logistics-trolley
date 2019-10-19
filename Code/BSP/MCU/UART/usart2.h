#include "stm32f4xx.h"
#include "sys.h"

void usart2_init(u32 bound);
void u2_printf(char* fmt,...);
void u2_putbuff(u8 *buff, u32 len);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendBytes( USART_TypeDef * pUSART, uint8_t * buf, uint8_t length);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);


#define USART2_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART2_RX_EN 			1					//0,不接收;1,接收.
