#include "stm32f4xx.h"

int8_t x = 0;
int8_t y = 0;
int8_t z = 0;

SPI_InitTypeDef SPIinit;
GPIO_InitTypeDef GPIOinit;

void SPIConfig(void);
void GPIOConfig(void);

void SPIConfig(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	SPIinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // Clock speed of SPI , APB2 Bus hattini 64 e boluyor
	SPIinit.SPI_CPHA = SPI_CPHA_2Edge; // Defines edge for bit capture 2nd edge used
	SPIinit.SPI_CPOL = SPI_CPOL_High; // Clock polarity
	SPIinit.SPI_CRCPolynomial = 7;
	SPIinit.SPI_DataSize = SPI_DataSize_8b; // 8 bit or 16bit
	SPIinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // Unidirectional or bidirectional. We use bidirect.
	SPIinit.SPI_FirstBit = SPI_FirstBit_MSB; // Start reading with MSB
	SPIinit.SPI_Mode = SPI_Mode_Master; // stm32f4 will be master
	SPIinit.SPI_NSS = SPI_NSS_Soft; // Hardware or Software

	SPI_Init(SPI1, &SPIinit);
	SPI_Cmd(SPI1, ENABLE);

}

void GPIOConfig(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIOinit.GPIO_Mode = GPIO_Mode_AF;
	GPIOinit.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;	//SPI1_SCK , SPI1_MISO , SPI1_MOSI pinlerini ayarladik.
	GPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOinit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIOinit);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	GPIOinit.GPIO_Mode = GPIO_Mode_OUT;
	GPIOinit.GPIO_Pin = GPIO_Pin_3;		// CS_I2C/SPI olarak ayarladik
	GPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOinit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOE, &GPIOinit);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIOinit.GPIO_Mode = GPIO_Mode_OUT;
	GPIOinit.GPIO_OType = GPIO_OType_PP;
	GPIOinit.GPIO_Pin = GPIO_Pin_12;
	GPIOinit.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIOinit.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOD, &GPIOinit);

}

void SPI_Tx(uint8_t adress, uint8_t data)
{
	// adress:  Open the LIS302DL datasheet to find out the adress of a certain register.

  GPIO_ResetBits(GPIOE,GPIO_Pin_3); // LIS302DL sensorunu okuma ve yazmaya basladigimizda low konumuna cekmeliyiz.
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,adress);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);

  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
  SPI_I2S_SendData(SPI1,data);
  while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
  SPI_I2S_ReceiveData(SPI1);

  GPIO_SetBits(GPIOE,GPIO_Pin_3); // LIS302DL sensorunu okuma ve yazmaya bittiginde high konumuna cekmeliyiz.LIS302DL datasheet bakarak bulduk.

}

uint8_t SPI_Rx(uint8_t adress)
{
 GPIO_ResetBits(GPIOE,GPIO_Pin_3); //CS pin logic 0
 adress=(0x80) | (adress); //  this tells the sensor to read and not to write, that's where the (0x80 | adress) comes from.

 while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
 SPI_I2S_SendData(SPI1,adress);
 while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
 SPI_I2S_ReceiveData(SPI1);

 while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE));
 SPI_I2S_SendData(SPI1,0x00);
 while(!SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE));
 SPI_I2S_ReceiveData(SPI1);
 GPIO_SetBits(GPIOE,GPIO_Pin_3); //CS pin logic 1
 return SPI_I2S_ReceiveData(SPI1);
}

int main(void)
{
		GPIOConfig();
		SPIConfig();

	 	GPIO_SetBits(GPIOE,GPIO_Pin_3);
	 	SPI_Tx(0x20, 0x67); //100 Hz  ODR3 ODR2 ODR1 ODR0 BDU Zen Yen Xen  ,  CTRL_REG4 (20h)
	 						//			0	1	 1	   0   0   1   1   1
  while (1)
  {
		x = SPI_Rx(0x29);
		y = SPI_Rx(0x2B);
		z = SPI_Rx(0x2D);
  }
}



void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
