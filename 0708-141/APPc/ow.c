#include "_config.h"


#if OW_ENABLE==1



static unsigned char ROM_NO[8];//64位序列号,第1个字节是家族码0X28
static unsigned char LastDiscrepancy;
static unsigned char LastFamilyDiscrepancy;
static unsigned char LastDeviceFlag;
static unsigned char crc8;

int tt;

const static unsigned char dscrc_table[] = {
0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};
//one-----------------------------------------------------------------
uchar OWReset(void)//总线复位
{
   u8 i=0;
   do{
        DQ_H;
        NOP;
        NOP;
        DQ_L;
        delay_us(600);//480us--960us
        DQ_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OWWriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ_H;
     }
     else
     {
       DQ_L;
     }
     delay_us(60);
     DQ_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OWReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ_L;
      delay_us(5);
      DQ_DDR_L;
      delay_us(5);
      if(DQ_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void OneTstart(void)//温度转换需要2秒
{
   if(OWReset())
   {
     OWWriteByte(0xcc);
     OWWriteByte(0x44);
   }
}
int OneTread(void)
{   
   u8 i,crc8=0;
   u8 H8,L8;
   u8 BufR[9];
   int TT;
   
   if(OWReset())
   {
         OWWriteByte(0xcc);
         OWWriteByte(0xbe);
         
          crc8=0;
          for(i=0;i<9;i++)
          {
            BufR[i]=OWReadByte();//读9字节数据
            if(i!=8)
            {
              crc8 = dscrc_table[crc8 ^ BufR[i]];//计算crc8循环冗余检验码
            }
          }
          if(crc8==BufR[8])//crc8循环冗余检验码正确
          {
             L8=BufR[0];
             H8=BufR[1];
             TT=GetTemp(L8,H8);
             if(!(TT==850))//滤掉850，850可能为转换没完成就读取的数据或上电时的默认值
             {
               return TT;
             }
          }
   }
   TT=20000;
   return TT;
}
//=========================================================================
uchar OW2Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ2_H;
        NOP;
        NOP;
        DQ2_L;
        delay_us(600);//480us--960us
        DQ2_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ2_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW2WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ2_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ2_H;
     }
     else
     {
       DQ2_L;
     }
     delay_us(60);
     DQ2_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW2ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ2_L;
      delay_us(5);
      DQ2_DDR_L;
      delay_us(5);
      if(DQ2_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One2Tstart(void)//温度转换需要2秒
{
   if(OW2Reset())
   {
     OW2WriteByte(0xcc);
     OW2WriteByte(0x44);
   }
}
int One2Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW2Reset())
   {
         OW2WriteByte(0xcc);
         OW2WriteByte(0xbe);
         L8=OW2ReadByte();
         H8=OW2ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//==========================================================================
uchar OW3Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ3_H;
        NOP;
        NOP;
        DQ3_L;
        delay_us(600);//480us--960us
        DQ3_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ3_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW3WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ3_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ3_H;
     }
     else
     {
       DQ3_L;
     }
     delay_us(60);
     DQ3_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW3ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ3_L;
      delay_us(5);
      DQ3_DDR_L;
      delay_us(5);
      if(DQ3_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One3Tstart(void)//温度转换需要2秒
{
   if(OW3Reset())
   {
     OW3WriteByte(0xcc);
     OW3WriteByte(0x44);
   }
}
int One3Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW3Reset())
   {
         OW3WriteByte(0xcc);
         OW3WriteByte(0xbe);
         L8=OW3ReadByte();
         H8=OW3ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//=============================================================================
uchar OW4Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ4_H;
        NOP;
        NOP;
        DQ4_L;
        delay_us(600);//480us--960us
        DQ4_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ4_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW4WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ4_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ4_H;
     }
     else
     {
       DQ4_L;
     }
     delay_us(60);
     DQ4_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW4ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ4_L;
      delay_us(5);
      DQ4_DDR_L;
      delay_us(5);
      if(DQ4_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One4Tstart(void)//温度转换需要2秒
{
   if(OW4Reset())
   {
     OW4WriteByte(0xcc);
     OW4WriteByte(0x44);
   }
}
int One4Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW4Reset())
   {
         OW4WriteByte(0xcc);
         OW4WriteByte(0xbe);
         L8=OW4ReadByte();
         H8=OW4ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//=============================================================================
uchar OW5Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ5_H;
        NOP;
        NOP;
        DQ5_L;
        delay_us(600);//480us--960us
        DQ5_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ5_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW5WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ5_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ5_H;
     }
     else
     {
       DQ5_L;
     }
     delay_us(60);
     DQ5_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW5ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ5_L;
      delay_us(5);
      DQ5_DDR_L;
      delay_us(5);
      if(DQ5_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One5Tstart(void)//温度转换需要2秒
{
   if(OW5Reset())
   {
     OW5WriteByte(0xcc);
     OW5WriteByte(0x44);
   }
}
int One5Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW5Reset())
   {
         OW5WriteByte(0xcc);
         OW5WriteByte(0xbe);
         L8=OW5ReadByte();
         H8=OW5ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//=========================================================================
uchar OW6Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ6_H;
        NOP;
        NOP;
        DQ6_L;
        delay_us(600);//480us--960us
        DQ6_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ6_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW6WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ6_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ6_H;
     }
     else
     {
       DQ6_L;
     }
     delay_us(60);
     DQ6_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW6ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ6_L;
      delay_us(5);
      DQ6_DDR_L;
      delay_us(5);
      if(DQ6_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One6Tstart(void)//温度转换需要2秒
{
   if(OW6Reset())
   {
     OW6WriteByte(0xcc);
     OW6WriteByte(0x44);
   }
}
int One6Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW6Reset())
   {
         OW6WriteByte(0xcc);
         OW6WriteByte(0xbe);
         L8=OW6ReadByte();
         H8=OW6ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//===========================================================================
uchar OW7Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ7_H;
        NOP;
        NOP;
        DQ7_L;
        delay_us(600);//480us--960us
        DQ7_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ7_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW7WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ7_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ7_H;
     }
     else
     {
       DQ7_L;
     }
     delay_us(60);
     DQ7_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW7ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ7_L;
      delay_us(5);
      DQ7_DDR_L;
      delay_us(5);
      if(DQ7_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One7Tstart(void)//温度转换需要2秒
{
   if(OW7Reset())
   {
     OW7WriteByte(0xcc);
     OW7WriteByte(0x44);
   }
}
int One7Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW7Reset())
   {
         OW7WriteByte(0xcc);
         OW7WriteByte(0xbe);
         L8=OW7ReadByte();
         H8=OW7ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//=========================================================================
uchar OW8Reset(void)//总线复位
{
   u8 i=0;
   do{
        DQ8_H;
        NOP;
        NOP;
        DQ8_L;
        delay_us(600);//480us--960us
        DQ8_DDR_L;
        delay_us(100);//释放总线，等待>60us
        i++;
        if(i>=16)
        {
          return 0;
        }
   }while((DQ8_IN));//1:没有回应
   delay_us(600);
   return 1;
}
void OW8WriteByte(uchar data)//写一个字节
{
   uchar i=0;
   _DINT();
   for(i=0;i<8;i++)
   {
     DQ8_L;
     delay_us(5);
     if(data&0x01)
     {
       DQ8_H;
     }
     else
     {
       DQ8_L;
     }
     delay_us(60);
     DQ8_DDR_L;
     data>>=1;
   }
   _EINT();
}
uchar OW8ReadByte(void)//读一字节
{
   uchar i=0;
   uchar temp_byte=0;

   _DINT();
   for(i=0;i<8;i++)
   {
      temp_byte=temp_byte>>1;
      DQ8_L;
      delay_us(5);
      DQ8_DDR_L;
      delay_us(5);
      if(DQ8_IN)
      {
        temp_byte=temp_byte|0x80;
      }
      delay_us(60);
   }
   _EINT();
   return temp_byte;
}

void One8Tstart(void)//温度转换需要2秒
{
   if(OW8Reset())
   {
     OW8WriteByte(0xcc);
     OW8WriteByte(0x44);
   }
}
int One8Tread(void)
{   
   u8 H8,L8;
   int TT;
   
   if(OW8Reset())
   {
         OW8WriteByte(0xcc);
         OW8WriteByte(0xbe);
         L8=OW8ReadByte();
         H8=OW8ReadByte();
         TT=GetTemp(L8,H8);
         return TT;
   }
   TT=20000;
   return TT;
}
//--------------------------------------------------------------------------
int GetTemp(u8 LL,u8 HH)
{
         u8 H8,L8;
         u8 F0;
         u32 T32;
         u16 T16;
         int TT;
         
         L8=LL;
         H8=HH;
         if((H8&0xf0)==0xf0)//负数
         {
           L8=~L8;
           H8=~H8;
           T16=(u16)H8;
           T16=((T16<<8)|L8)+1;
           F0=1;
         }
         else//正数
         {
           T16=(u16)H8;
           T16=(T16<<8)|L8;
           F0=0;
         }
         T32=(u32)T16;
         T32=T32*100;//X100
         T32=T32>>4;
         if((T32%10)>4)T32=T32+10;
         T32=T32/10;//X10
         
         if(F0==1)TT=(int)T32*(-1);
         else TT=(int)T32;
         return TT;
}

//----------------------------------------------------------------------------
void OWWriteBit(unsigned char data)//写一位
{
     _DINT();
     DQ_L;
     delay_us(5);
     if(data)
     {
       DQ_H;
     }
     else
     {
       DQ_L;
     }
     delay_us(60);
     DQ_DDR_L;
     _EINT();
}
unsigned char OWReadBit(void)//读一位
{
    uchar bit;
    _DINT();
    DQ_L;
    delay_us(5);
    DQ_DDR_L;
    delay_us(5);
    if(DQ_IN)
    {
      bit=1;
    }
    else
    {
      bit=0;
    }
    delay_us(60);
    _EINT();
    return bit;
}

//--------------------------------------------------------------------------
// Find the 'first' devices on the 1-Wire bus
// Return TRUE : device found, ROM number in ROM_NO buffer
// FALSE : no device present
int OWFirst(void)
{
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = 0;
  LastFamilyDiscrepancy = 0;
  return OWSearch();
}
int OWNext(void)
{
  // leave the search state alone
  return OWSearch();
}
//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE : device found, ROM number in ROM_NO buffer
// FALSE : device not found, end of search
unsigned char OWSearch(void)
{
  unsigned char id_bit_number;
  unsigned char last_zero, rom_byte_number, search_result;
  unsigned char id_bit, cmp_id_bit;
  unsigned char rom_byte_mask, search_direction;
  // initialize for search
  id_bit_number = 1;
  last_zero = 0;
  rom_byte_number = 0;
  rom_byte_mask = 1;
  search_result = 0;
  crc8 = 0;
  // if the last call was not the last one
  if (!LastDeviceFlag)
  {
    // 1-Wire reset
    if (!OWReset())
    {
      // reset the search
      LastDiscrepancy = 0;
      LastDeviceFlag = 0;
      LastFamilyDiscrepancy = 0;
      return 0;
    }
    // issue the search command
    OWWriteByte(0xf0);
    // loop to do the search
   do
   {
     // read a bit and its complement
     id_bit = OWReadBit();
     cmp_id_bit = OWReadBit();
     // check for no devices on 1-Wire
     if ((id_bit == 1) && (cmp_id_bit == 1))
       break;
     else
     {
        // all devices coupled have 0 or 1
        if (id_bit != cmp_id_bit)
          search_direction = id_bit; // bit write value for search
        else
        {
          // if this discrepancy if before the Last Discrepancy
          // on a previous next then pick the same as last time
          if (id_bit_number < LastDiscrepancy)
            search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
          else
          // if equal to last pick 1, if not then pick 0
           search_direction = (id_bit_number == LastDiscrepancy);
          // if 0 was picked then record its position in LastZero
          if (search_direction == 0)
          {
            last_zero = id_bit_number;
             // check for Last discrepancy in family
            if (last_zero < 9)
              LastFamilyDiscrepancy = last_zero;
          }
        }
        // set or clear the bit in the ROM byte rom_byte_number
        // with mask rom_byte_mask
        if (search_direction == 1)
          ROM_NO[rom_byte_number] |= rom_byte_mask;
        else
          ROM_NO[rom_byte_number] &= ~rom_byte_mask;
        // serial number search direction write bit
        OWWriteBit(search_direction);
        // increment the byte counter id_bit_number
        // and shift the mask rom_byte_mask
        id_bit_number++;
        rom_byte_mask <<= 1;
        // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
        if (rom_byte_mask == 0)
        {
          docrc8(ROM_NO[rom_byte_number]); // accumulate the CRC
          rom_byte_number++;
          rom_byte_mask = 1;
        }
     }
    }while(rom_byte_number < 8); // loop until through all ROM bytes 0-7
    // if the search was successful then
    if (!((id_bit_number < 65) || (crc8 != 0)))
    {
      // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
      LastDiscrepancy = last_zero;
      // check for last device
      if (LastDiscrepancy == 0)
        LastDeviceFlag = 1;
      // check for last family group
      if (LastFamilyDiscrepancy == LastDiscrepancy)
        LastFamilyDiscrepancy = 0;
      search_result = 1;
    }
  }
  // if no device found then reset counters so next 'search' will be like a first
  if (!search_result || !ROM_NO[0])
  {
    LastDiscrepancy = 0;
    LastDeviceFlag = 0;
    LastFamilyDiscrepancy = 0;
    search_result = 0;
  }
  return search_result;
}
//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current
// global 'crc8' value.
// Returns current global crc8 value
//
unsigned char docrc8(unsigned char value)
{
  // See Application Note 27
  // TEST BUILD
  crc8 = dscrc_table[crc8 ^ value];
  return crc8;
}

//功能：读取总线上ID,搜索到的ID号全部存储到DS18B20_ID_NUM_ADDR
//返回：总线上有器件总数
u16 OWSearchID(void)
{
    u16 i;
    u16 IDNum=0;
    if(OWFirst())
    {
      for(i=0;i<8;i++)
      {
        Uart_1.RxBuf[IDNum*8+i]=ROM_NO[i];
      }
      IDNum++;
      while(OWNext())
      {
        for(i=0;i<8;i++)
        {
          Uart_1.RxBuf[IDNum*8+i]=ROM_NO[i];
        }
        IDNum++;
      }
    }

    return IDNum;
}
//功能;总线上所有器件温度转换开始
void OWConvertStart(void)
{
   if(OWReset())
   {
     OWWriteByte(0xcc);
     OWWriteByte(0x44);
   }
}
//读取总线上某1个ID的温度值，BufID为8字节ID号，ChNo为写入的通道号
int OWReadIDTemp(u8* BufID)
{  
   u16 i;
   u8 BufR[9];
   u8 crc8;//循环冗余检验码
   u8 H8,L8;
   u8 F0;
   u32 T32;
   u16 T16;
   int TT;
   
   if(OWReset())
   {
      OWWriteByte(0x55);//match ROM ID
      for(i=0;i<8;i++)//8字节ID
      {
        OWWriteByte(BufID[i]);
      }
      OWWriteByte(0xbe);
     
      crc8=0;
      for(i=0;i<9;i++)
      {
	BufR[i]=OWReadByte();//读9字节数据
	if(i!=8)
        {
          //计算crc8循环冗余检验码
          crc8 = dscrc_table[crc8 ^ BufR[i]];
        }
      }
      if(crc8==BufR[8])//crc8循环冗余检验码正确
      {
         L8=BufR[0];
         H8=BufR[1];
         if((H8&0xf0)==0xf0)//负数
         {
           L8=~L8;
           H8=~H8;
           T16=(u16)H8;
           T16=((T16<<8)|L8)+1;
           F0=1;
         }
         else//正数
         {
           T16=(u16)H8;
           T16=(T16<<8)|L8;
           F0=0;
         }
         T32=(u32)T16;
         T32=T32*100;//X100
         T32=T32>>4;
         if((T32%10)>4)T32=T32+10;
         T32=T32/10;//X10
         if(F0==1)TT=(int)T32*(-1);
         else TT=(int)T32;
         return TT;
      }
   }
   return 20000;
}

//根据通道号开始测量温度
void OneChTstart(u8 ch)
{
  if(ch==0)
  {
    OneTstart();
  }
  else if(ch==1)
  {
    One2Tstart();
  }
  else if(ch==2)
  {
    One3Tstart();
  }
  else if(ch==3)
  {
    One4Tstart();
  }
  else if(ch==4)
  {
    One5Tstart();
  }
  else if(ch==5)
  {
    One6Tstart();
  }
  else if(ch==6)
  {
    One7Tstart();
  }
  else if(ch==7)
  {
    One8Tstart(); 
  }
}


//根据通道号读取温度值
int OneChTread(u8 ch)
{
  int tt;
  if(ch==0)
  {
    tt=OneTread();
  }
  else if(ch==1)
  {
    tt=One2Tread();
  }
  else if(ch==2)
  {
    tt=One3Tread();
  }
  else if(ch==3)
  {
    tt=One4Tread();
  }
  else if(ch==4)
  {
    tt=One5Tread();
  }
  else if(ch==5)
  {
    tt=One6Tread();
  }
  else if(ch==6)
  {
    tt=One7Tread();
  }
  else if(ch==7)
  {
    tt=One8Tread();
  }
  
  return tt;
}
//读取某一路的数据
int OneChData(u8 ch)
{   
   u8 i,crc8=0;
   u8 H8,L8;
   u8 BufR[9];
   u8 reset;
   int TT;
   
   //复位
   if(ch==0)
   {
      reset=OWReset();
   }
   else if(ch==1)
   {
      reset=OW2Reset();
   }
   else if(ch==2)
   {
      reset=OW3Reset();
   }
   else if(ch==3)
   {
      reset=OW4Reset();
   }
   else if(ch==4)
   {
      reset=OW5Reset();
   }
   else if(ch==5)
   {
      reset=OW6Reset();
   }
   else if(ch==6)
   {
      reset=OW7Reset();
   }
   else if(ch==7)
   {
      reset=OW8Reset();
   }
   
   if(reset)//复位正常后，进入
   {
         //开始读取数据
         if(ch==0)
         {
           OWWriteByte(0xcc);
           OWWriteByte(0xbe);
         }
         else if(ch==1)
         {
           OW2WriteByte(0xcc);
           OW2WriteByte(0xbe);
         }
         else if(ch==2)
         {
           OW3WriteByte(0xcc);
           OW3WriteByte(0xbe);
         }
         else if(ch==3)
         {
           OW4WriteByte(0xcc);
           OW4WriteByte(0xbe);
         }
         else if(ch==4)
         {
           OW5WriteByte(0xcc);
           OW5WriteByte(0xbe);
         }
         else if(ch==5)
         {
           OW6WriteByte(0xcc);
           OW6WriteByte(0xbe);
         }
         else if(ch==6)
         {
           OW7WriteByte(0xcc);
           OW7WriteByte(0xbe);
         }
         else if(ch==7)
         {
           OW8WriteByte(0xcc);
           OW8WriteByte(0xbe);
         }
         
          //读取9字节数据，包括CRC
          crc8=0;
          for(i=0;i<9;i++)
          {
             if(ch==0)
             {
               BufR[i]=OWReadByte();//读9字节数据
             }
             else if(ch==1)
             {
               BufR[i]=OW2ReadByte();//读9字节数据
             }
             else if(ch==2)
             {
               BufR[i]=OW3ReadByte();//读9字节数据
             }
             else if(ch==3)
             {
               BufR[i]=OW4ReadByte();//读9字节数据
             }
             else if(ch==4)
             {
               BufR[i]=OW5ReadByte();//读9字节数据
             }
             else if(ch==5)
             {
               BufR[i]=OW6ReadByte();//读9字节数据
             }
             else if(ch==6)
             {
               BufR[i]=OW7ReadByte();//读9字节数据
             }
             else if(ch==7)
             {
               BufR[i]=OW8ReadByte();//读9字节数据
             }

              
             if(i!=8)
             {
               crc8 = dscrc_table[crc8 ^ BufR[i]];//计算crc8循环冗余检验码
             }
          }
          
          //校验CRC
          if(crc8==BufR[8])
          {
             L8=BufR[0];
             H8=BufR[1];
             TT=GetTemp(L8,H8);
             
             if((TT>(-550))&&(TT<1250)&&(TT!=850))//滤掉850，850可能为转换没完成就读取的数据或上电时的默认值
             {
               return TT;
             }
          }
   }
   TT=20000;
   return TT;
}

void Ds18b20Test(void)
{
  OneTstart();
  tt=OneTread();
  NOP;
  /*
  u8 i,j;
  u8 BufID[8];
  OWConvertStart();
  delay_ms(1000);
  for(j=0;j<*((char *)DS18B20_ID_NUM_ADDR);j++)
  {
    for(i=0;i<8;i++)
      BufID[i]=*((char *)DS18B20_ID1_ADDR+j*8+i);
    //StateC.Value[j]=OWReadIDTemp(BufID,j);
  }
  delay_ms(1000);*/
}

#endif

