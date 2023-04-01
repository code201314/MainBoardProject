/*
 * @Author: code201314 1162782792@qq.com
 * @Date: 2023-02-26 23:18:19
 * @LastEditors: code201314 1162782792@qq.com
 * @LastEditTime: 2023-03-12 23:09:59
 * @FilePath: \MainBoardProject\SYSTEM\usart\usart.h
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
uint32_t usart_write(USART_TypeDef* USARTx, const uint8_t* buf, uint32_t len);
#endif


