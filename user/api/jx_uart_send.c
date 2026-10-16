#include "jx_uart_send.h"

// 串口通信消息头
const unsigned char g_uart_send_head[] = {
  0xaa, 0x55
};

// 串口通信消息尾
const unsigned char g_uart_send_foot[] = {
  0x55, 0xaa
};
#include "su03.h"
// 串口发送函数实现
void _uart_send_impl(unsigned char* buff, int len) {
  // TODO: 调用项目实际的串口发送函数
  /*
  int i = 0;
  unsigned char c;
  for (i = 0; i < len; i++) {
    c = buff[i];
    printf("%02X ", c);
  }
  printf("\n");
  */
	UART4_SendNData(buff,len);
}

// action: play_t2
void _uart_play_t2(int temp) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_play_t2;
  param.d_int = temp;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 7] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 9);
}

// action: play_h2
void _uart_play_h2(int hum) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_play_h2;
  param.d_int = hum;
  _int16_to_int32(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 7] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 9);
}

// action: play2_co2
void _uart_play2_co2(double co2) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_play2_co2;
  param.d_double = co2;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: play2_ch2o
void _uart_play2_ch2o(double ch2o) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_play2_ch2o;
  param.d_double = ch2o;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

// action: play2_voc
void _uart_play2_voc(double voc) {
  uart_param_t param;
  int i = 0;
  unsigned char buff[UART_SEND_MAX] = {0};
  for (i = 0; i < UART_MSG_HEAD_LEN; i++) {
      buff[i + 0] = g_uart_send_head[i];
  }
  buff[2] = U_MSG_play2_voc;
  param.d_double = voc;
  _float_to_double(&param);
  buff[3] = param.d_ucs[0];
  buff[4] = param.d_ucs[1];
  buff[5] = param.d_ucs[2];
  buff[6] = param.d_ucs[3];
  buff[7] = param.d_ucs[4];
  buff[8] = param.d_ucs[5];
  buff[9] = param.d_ucs[6];
  buff[10] = param.d_ucs[7];
  for (i = 0; i < UART_MSG_FOOT_LEN; i++) {
      buff[i + 11] = g_uart_send_foot[i];
  }
  _uart_send_impl(buff, 13);
}

