#ifndef __I2C_EE_H
#define __I2C_EE_H
//...........................................................................
#define EEPROM 512
//...........................................................................
#if EEPROM==256
	#define PAGE_SIZE 0x3f//64
	#define EE_SIZE   0x7fff//32768
#elif EEPROM==512
	#define PAGE_SIZE 0x7f//128
	#define EE_SIZE  0xffff//65536
#endif

#define   EE_PAGE_MAX           PAGE_SIZE
#define   I2C1_SLAVE_ADDRESS7   0xA0
#define   IIC_DELAY             32//15//40//AT24C512B-SH25 下载 12为下载包500临界 


#define   SDA             BIT6
#define   SCL             BIT5

#define   I2C_PSEL        P2SEL
#define   I2C_PDIR        P2DIR
#define   I2C_POUT        P2OUT
#define   SDA_PIN         P2IN
#define   I2C_PREN        P2REN

#define   SDA_DIR_H       I2C_PDIR|=SDA
#define   SDA_DIR_L       I2C_PDIR&=~SDA

#define   SDA_H           I2C_PDIR&=~SDA;I2C_PREN|=SDA;I2C_POUT|=SDA
#define   SDA_L           I2C_PDIR|=SDA;I2C_PREN&=~SDA;I2C_POUT&=~SDA
#define   SCL_H           I2C_PDIR&=~SCL;I2C_PREN|=SCL;I2C_POUT|=SCL
#define   SCL_L           I2C_PDIR|=SCL;I2C_PREN&=~SCL;I2C_POUT&=~SCL

#define   I2C_PORT_INIT   I2C_PSEL&=~(SDA+SCL);I2C_PDIR&=~(SCL+SDA);\
                          I2C_PREN|=SCL+SDA;I2C_POUT|=SCL+SDA;

#define   WP         BIT7
#define   WP_H       I2C_PSEL&=~WP;I2C_PDIR&=~WP;I2C_PREN|=WP;I2C_POUT|=WP//开启写保护
#define   WP_L       I2C_PSEL&=~WP;I2C_PDIR|=WP;I2C_PREN&=~WP;I2C_POUT&=~WP//关闭写保护


void I2cInit(void);
void I2C_EE_BufferWrite(u8* pBuffer, u16 WriteAddr, u16 NumByteToWrite,u8 No);
void I2C_EE_BufferRead(u8* pBuffer, u16 ReadAddr, u16 NumByteToRead,u8 No);
u8 i2c_ee_read_to_pc(u16 ReadAddr, u16 NumByteToRead,u8 No);
u8 i2c_SF_ee_read_to_u0(u16 ReadAddr, u16 NumByteToRead,u8 No);

void I2cStart(void);
void I2cStop(void);
void I2cAck(unsigned char aa);
unsigned char I2cSendByte(unsigned char cc);
unsigned char I2cRecByte(void);

u8 I2cSeekEE(void);
u8 i2c_ee_read_to_u0(u16 ReadAddr, u16 NumByteToRead,u8 No);


void EETest(void);



#endif

