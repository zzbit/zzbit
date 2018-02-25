#include "_config.h"

void ComDeel(void)
{
   if(Uart_1.FlagFinish==1)
   {
      Uart_1.FlagFinish=0;
      
      Flag.SeeComing=0;
      
      #if GPS_MOUSE_EN==1
      StateC.SeeBDSing=0;
      #endif
      
      
      #if WIRELESS_ENABLE==1
      Pt.Wdog=0;//W95接收到正确数据
      #endif
      
      #if ((RS485_ENABLE==1)||(RF_REC_EN==1))//zz
      delay_ms(10);
      #endif
      
      #if ((PRINT_REAL_EN==1)||(PRINT_RT_LIST_EN==1)||(SEN_MODBUS_485_EN==1)||(L_REALOUT_PRINT_EN==1)||(CMD_CTRL_EN==1))
      StateC.PcLinkPt=10;//10秒内部能打印
      #endif
      
      
      #if WIFI_ENABLE==1
      if(WifiRcvDo())//接收到的是回文
      {
        ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
        return;
      }
      
      if((Uart_1.RxBuf[0]==0xAA)||(Uart_1.RxBuf[0]==StateB.Address))
      {
         Pt.WifiPt=0;//收到数据，清标志
         Flag.TC_WaitPt=TC_WAIT_T;// 如果正在与zz通讯，则不能发送TCP数据
      }
      #endif
      
      
      #if MODBUS_ENABLE==1
            if(Flag.Modbus==1)
            {
              ComSendH();
              CmdModbusDeel();
              ComSendL();
              WirelessEnd();
              ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
              return;
            }
      
            #if BSQ_YIDA_EN==1
            else if(Flag.BsqYIDA==1)
            {
               YIDA_Do();
               Flag.BsqYIDA=0;
               return;
            }
            #endif
      
      
            else
            {
                if(Download.Downloading==0) //接收到非下载数据包命令，波特率9600
                {
                  //接收到PC命令...................................................
                  ComSendH();
                  CmdHexDeel();
                  ComSendL();
                  WirelessEnd();
                  ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
                  return;
                }
                //接收到PC下载数据包命令，波特率115200..................................          
                ComSendH();
                CmdHexDownPag();
                ComSendL();
                WirelessEnd();
                ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
                return;
            }
      
      #else //L
            if(Download.Downloading==0) //接收到非下载数据包命令，波特率9600
            {
              //接收到PC命令...................................................
              ComSendH();
              CmdHexDeel();
              ComSendL();
              WirelessEnd();
              ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
              return;
            }
            //接收到PC下载数据包命令，波特率115200..................................     
            //_DINT();//zz
            ComSendH();
            CmdHexDownPag();
            ComSendL();
            //_EINT();//zz
            WirelessEnd();
            ClrBuf(Uart_1.RxBuf,Uart_1.RecLen);//zz
            return;
      #endif
   }
}

#if ((BSQ_YIDA_EN==1)&&(GPRS_ENABLE==0))
void YIDA_Do(void)
{
    u8 i,j,k;
    u16 dataX;
    int xx;
    u16 dd;
  
    switch(Uart_1.RxBuf[3])
    {
       case 0x03:  

                   k=Uart_1.RxBuf[5];//控制位
                   //dataX=U8_TO_U32(0,Uart_1.RxBuf[6],Uart_1.RxBuf[7],Uart_1.RxBuf[8]);//数据地址
                   
                   //Re
                   i=0;
                   Uart_1.RxBuf[i++]=0x1b;//包头
                   Uart_1.RxBuf[i++]=0x06;
                   
                   //校验从地址开始
                   Uart_1.RxBuf[i++]=StateB.Address;//地址
                   
                   Uart_1.RxBuf[i++]=0x03;//指令
                   
                   Uart_1.RxBuf[i++]=6+2*CH_NUM;//长度
                   

                       for(j=0;j<CH_NUM;j++)
                       {

                            if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
                            {
                                //触摸屏识别显示 最小值
                                xx=*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j)+(u16)256*((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x7f);
                                if(((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x80)==0x80)
                                {
                                  xx=(-1)*xx;
                                }
                            }
                            else
                            {
                                //正常数值
                                xx=StateC.Value[j];
                                if(0x01&(StateC.FuhaoBit>>j))
                                {
                                  xx=(-1)*xx;
                                }
                            }
                         
                            //if(*((char*)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES)==_T_T)//读取该通道类型
                            //{
                            //    xx=xx+400;
                            //}
                            
                            
                            if(xx<0)
                            {
                              dd=xx*(-1);
                              dd=0x8000|xx;
                            }
                            else
                            {
                               dd=xx; 
                            }
                            
                            Uart_1.RxBuf[i++]=0xff&((dd)>>8);//温度
                            Uart_1.RxBuf[i++]=0xff&(dd);
                            



                       }
                       
                       Uart_1.RxBuf[i++]=0x00;
                       
                       //当前位置，是否实时数据
                       Rtc.SS=ReadRtcD10();
                       Uart_1.RxBuf[i++]=Rtc.Year;
                       Uart_1.RxBuf[i++]=Rtc.Month;
                       Uart_1.RxBuf[i++]=Rtc.Day;
                       Uart_1.RxBuf[i++]=Rtc.Hour;
                       Uart_1.RxBuf[i++]=Rtc.Minute;
                       //Uart_1.RxBuf[i++]=Rtc.Second;

                   
                   
                    unsigned short crc16;
                    crc16=GetCrc16(&Uart_1.RxBuf[2],i-2);
                    Uart_1.RxBuf[i++]=0xff&(crc16>>8);
                    Uart_1.RxBuf[i++]=0xff&(crc16);
                    
                    
                    ComSendH();
                    U1SendBuf(Uart_1.RxBuf,i);
                    ComSendL();
         
                   break;
      
       default:    break;
      
    }
}

#endif

#if MODBUS_ENABLE==1
/*  MODBUS  RTU
01       03        01 04           00 02          84 36
地址   功能号     数据地址        读取数据个数    CRC校验

RE:
01       03        04             00 F8 01 5C     7A 6B
地址   功能号     数据字节个数    两个字节数据    CRC校验

例子，
PC:01 03 01 04 00 02 84 36
RE:01 03 04 00 F8 01 5C 7A 6B 

温度   00 F8   248*0.1   即24.8
湿度   01 5C   348*0.1   即34.8
*/ 
void RTU_ReadReal(void)//01 03 01 04 00 02 84 36     //寄存器地址 01 04   =  260+1=261
{
        //整理回复的数据
        u16 i=0;

        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        
        Uart_1.RxBuf[i++]=CH_NUM*2;//字节数
        
        for(u16 j=0;j<CH_NUM;j++)
        {
          int xx;
          if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
          {
              //触摸屏识别显示 最小值
              xx=*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j)+(u16)256*((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x7f);
              if(((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x80)==0x80)
              {
                xx=(-1)*xx;
              }
          }
          else
          {
              //正常数值
              xx=StateC.Value[j];
              if(0x01&(StateC.FuhaoBit>>j))
              {
                xx=(-1)*xx;
              }
          }
          Uart_1.RxBuf[i++]=0xff&(xx>>8);//触摸屏数值格式为: int型(包含正负)
          Uart_1.RxBuf[i++]=0xff&xx;
        }
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}
void MCGS_ReadReal(void)
{
        //整理回复的数据
        u16 i=0;

        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        
        Uart_1.RxBuf[i++]=CH_NUM;//寄存器数
        
        for(u16 j=0;j<CH_NUM;j++)
        {
          int xx;
          if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
          {
              //触摸屏识别显示 最小值
              xx=*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j)+(u16)256*((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x7f);
              if(((*((char*)CH1_V_L_ADDR+ONE_CH_CONFIG_BYTES*j+1))&0x80)==0x80)
              {
                xx=(-1)*xx;
              }
          }
          else
          {
              //正常数值
              xx=StateC.Value[j];
              if(0x01&(StateC.FuhaoBit>>j))
              {
                xx=(-1)*xx;
              }
          }
          Uart_1.RxBuf[i++]=0xff&(xx>>8);//触摸屏数值格式为: int型(包含正负)
          Uart_1.RxBuf[i++]=0xff&xx;
        }
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}



#if RF_MULTI_EN==1
/*  MODBUS  RTU
01       03        01 04           00 02          84 36
地址   功能号     数据地址        读取数据个数    CRC校验

RE:
01       03        04             00 F8 01 5C     7A 6B
地址   功能号     数据字节个数    两个字节数据    CRC校验

例子，
PC:01 03 01 04 00 02 84 36  //寄存器地址 01 04   =  260+1=261
RE:01 03 04 00 F8 01 5C 7A 6B 

温度   00 F8   248*0.1   即24.8
湿度   01 5C   348*0.1   即34.8


*/ 
         
void RTU_MULTI_ALL_ReadReal(void)
{
        u16 i;
        volatile u16 poit;

        poit=U8_TO_U16(Uart_1.RxBuf[4],Uart_1.RxBuf[5]);//读取的寄存器数
        

        //整理回复的数据
        i=0;
        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        
        Uart_1.RxBuf[i++]=poit*2;//字节数
        
        for(u16 j=0;j<poit;j++)
        {
            int xx;
            
            #if RF_MULTI_MNJ==1
            
                 xx=j;
            
            #else
            
                 
               if(Pt.RfData[j/2][j%2].value==SENIOR_NULL)
               {
                 if(j%2==0)
                   xx=-400;
                 else
                   xx=0;
               }
               else
               {
                  //正常数值
                  xx=0x7fff&Pt.RfData[j/2][j%2].value;
                  if(0x8000&(Pt.RfData[j/2][j%2].value))
                  {
                    xx=(-1)*xx;
                  }
               }
  
            #endif
                
            Uart_1.RxBuf[i++]=0xff&(xx>>8);//触摸屏数值格式为: int型(包含正负)
            Uart_1.RxBuf[i++]=0xff&xx;
        }
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}

//变送器， zz连接时，所有地址都回
void PC_MULTI_ReadReal(void)
{
        u16 i,j;
        volatile u8 addr,chn;//增加volatile
        volatile u8 TTflag;//2温度协议判断
        
        addr=Uart_1.RxBuf[0];//地址
        
        //通道数
#if RF_MULTI_MNJ==1
        chn=CH_NUM;  
#else
        chn=0x7f&Pt.RfChN[addr-1];
        if(chn<1||chn>2)
          return;
        
        TTflag=0;
        if(0x80&Pt.RfChN[addr-1])
          TTflag=1;
      
#endif
        
        
        
        //re:
        i=0;
        Uart_1.RxBuf[i++]=addr;
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=17+5*chn;//字节数
        
        //RTC
        RtcReadTime();
        u8* xx;
        xx=&Rtc.Year;
        for(j=0;j<6;j++)
          Uart_1.RxBuf[i++]=*xx++;
        RtcBcdToD10(&Rtc);
        
        //SN
        I2C_EE_BufferRead(&Uart_1.RxBuf[i],SERIAL_NO_ADDR,10,0);
        
        //读实时数据时，SN号前7位不变，后3位 定义为 传感器地址号
        Uart_1.RxBuf[i+7]=(addr/100)%10+0x30;
        Uart_1.RxBuf[i+8]=(addr/10)%10+0x30;
        Uart_1.RxBuf[i+9]=(addr/1)%10+0x30;
        
        
        i+=10;
        
        //通道数
        Uart_1.RxBuf[i++]=chn;
        
        
        for(j=0;j<chn;j++)
        {

#if RF_MULTI_MNJ==1

           Uart_1.RxBuf[i++]=*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);
           Uart_1.RxBuf[i++]=*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES);
           Uart_1.RxBuf[i++]=*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES);
          
          
           Uart_1.RxBuf[i++]=0xff&((u16)addr*10)>>8;
           Uart_1.RxBuf[i++]=0xff&((u16)addr*10);
#else
            if(j==0)//通道1默认是温度
            {
                Uart_1.RxBuf[i++]=0x01;
                Uart_1.RxBuf[i++]=0x01;
                Uart_1.RxBuf[i++]=0x02;
            }
            else if(j==1)//通道2默认是湿度,, 对于2温度卡片协议的，根据当前卡片地址，通道数中最高位判断
            {
                if(TTflag==1)
                {
                    Uart_1.RxBuf[i++]=0x01;//温度
                    Uart_1.RxBuf[i++]=0x01;
                    Uart_1.RxBuf[i++]=0x02;
                }
                else
                {
                    Uart_1.RxBuf[i++]=0x02;//湿度
                    Uart_1.RxBuf[i++]=0x02;
                    Uart_1.RxBuf[i++]=0x02;

                }
            }
            
            if(Pt.RfData[addr-1][j].value==SENIOR_NULL)
            {
                return;//NULL时，不回数据
            }
            Uart_1.RxBuf[i++]=0xff&(Pt.RfData[addr-1][j].value>>8);
            Uart_1.RxBuf[i++]=0xff&Pt.RfData[addr-1][j].value;
#endif
        }

        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}

#endif


void PC_ReadSMSData(void)
{

    if(Flag.RstFirstSample==0)
      return;
  
        u16 j;
        //整理回复的数据
        u16 i=0;

        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=0x00;
        Uart_1.RxBuf[i++]=0x0B;
        
        Uart_1.RxBuf[i++]=37+6*CH_NUM;//字节数
        
        //RTC
        RtcReadTime();
        u8* xx;
        xx=&Rtc.Year;
        for(j=0;j<6;j++)
          Uart_1.RxBuf[i++]=*xx++;
        RtcBcdToD10(&Rtc);
        
        //SN
        I2C_EE_BufferRead(&Uart_1.RxBuf[i],SERIAL_NO_ADDR,10,0);
        i+=10;
        
        //短信标题Uni
        I2C_EE_BufferRead(&Uart_1.RxBuf[i],SMS_TITLE_ADDR,20,0);
        i+=20;
        
        //通道数
        Uart_1.RxBuf[i++]=CH_NUM;
        
        //通道数据
        for(u16 j=0;j<CH_NUM;j++)
        {
            Uart_1.RxBuf[i++]=*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);
            Uart_1.RxBuf[i++]=*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES);
            Uart_1.RxBuf[i++]=*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES);
            
            if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
            {
              Uart_1.RxBuf[i++]=0xff&(StateC.Value[j]>>8);
              Uart_1.RxBuf[i++]=0xff&StateC.Value[j];
            }
            else
            {
                if(0x01&(StateC.FuhaoBit>>j))//负数
                {
                  Uart_1.RxBuf[i++]=0x80|(0xff&(StateC.Value[j]>>8));
                }
                else
                {
                  Uart_1.RxBuf[i++]=0xff&(StateC.Value[j]>>8);
                }
                Uart_1.RxBuf[i++]=0xff&StateC.Value[j];
            }
            
            //超标标志
            #if BSQ_T_AL_EN==1
            if(1&(Flag.AlarmB>>j))
            #else
            if(1&(Flag.AlarmR>>j))
            #endif
            {
              Uart_1.RxBuf[i++]=0x01;
            }
            else
            {
              Uart_1.RxBuf[i++]=0x00;
            }
        }
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}
void PC_ReadReal(void)
{
    if(Flag.RstFirstSample==0)
      return;
  
        u16 j;
        //整理回复的数据
        u16 i=0;

        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        
        Uart_1.RxBuf[i++]=17+5*CH_NUM;//字节数
        
        //RTC
        RtcReadTime();
        u8* xx;
        xx=&Rtc.Year;
        for(j=0;j<6;j++)
          Uart_1.RxBuf[i++]=*xx++;
        RtcBcdToD10(&Rtc);
        
        //SN
        I2C_EE_BufferRead(&Uart_1.RxBuf[i],SERIAL_NO_ADDR,10,0);
        i+=10;
        
        //通道数
        Uart_1.RxBuf[i++]=CH_NUM;
        
        //通道数据
        for(u16 j=0;j<CH_NUM;j++)
        {
          Uart_1.RxBuf[i++]=*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES);
          
          if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
          {
            Uart_1.RxBuf[i++]=0xff&(StateC.Value[j]>>8);
            Uart_1.RxBuf[i++]=0xff&StateC.Value[j];
          }
          else
          {
              if(0x01&(StateC.FuhaoBit>>j))//负数
              {
                Uart_1.RxBuf[i++]=0x80|(0xff&(StateC.Value[j]>>8));
              }
              else
              {
                Uart_1.RxBuf[i++]=0xff&(StateC.Value[j]>>8);
              }
              Uart_1.RxBuf[i++]=0xff&StateC.Value[j];
          }
        }
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}
void PC_ReadAlarm(void)
{
        u16 i=0;
        u16 ch;
        
        ch=Uart_1.RxBuf[4]-1;

        #if RF_MULTI_MNJ==1
          Uart_1.RxBuf[i++]= Uart_1.RxBuf[0];     
        #else
           Uart_1.RxBuf[i++]=StateB.Address;    
        #endif
        
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=0x00;
        Uart_1.RxBuf[i++]=0x0A;
        
        Uart_1.RxBuf[i++]=ch+1;
        Uart_1.RxBuf[i++]=*((char *)CH1_A_L_ADDR+1+ch*ONE_CH_CONFIG_BYTES);
        Uart_1.RxBuf[i++]=*((char *)CH1_A_L_ADDR+ch*ONE_CH_CONFIG_BYTES);
        Uart_1.RxBuf[i++]=*((char *)CH1_A_H_ADDR+1+ch*ONE_CH_CONFIG_BYTES);
        Uart_1.RxBuf[i++]=*((char *)CH1_A_H_ADDR+ch*ONE_CH_CONFIG_BYTES);
        Uart_1.RxBuf[i++]=*((char *)CH1_A_EN_ADDR+ch*ONE_CH_CONFIG_BYTES);

        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i); 
}
void PC_ReadState(void)
{
        u16 j;
        //整理回复的数据
        u16 i=0;

        #if RF_MULTI_MNJ==1
          Uart_1.RxBuf[i++]= Uart_1.RxBuf[0];     
        #else
           Uart_1.RxBuf[i++]=StateB.Address;    
        #endif
        
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=0x00;
        Uart_1.RxBuf[i++]=0x09;
        
        Uart_1.RxBuf[i++]=0xff&(0);
        Uart_1.RxBuf[i++]=0xff&(0);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.Rec16Num>>8);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.Rec16Num>>0);
        
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecCap>>24);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecCap>>16);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecCap>>8);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecCap>>0);
        
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecTime>>24);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecTime>>16);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecTime>>8);
        Uart_1.RxBuf[i++]=0xff&(PcBsq.RecTime>>0);
        
        for(j=0;j<30;j++)
          Uart_1.RxBuf[i++]=0x00;
        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);
}
void PC_SetRtc(void)
{
  u16 j;
  u16 i;
  u8* xx;
  
  xx=&Rtc.Year;
  for(j=0;j<6;j++)
    *xx++=Uart_1.RxBuf[4+j];
  RtcSetTime();
  RtcBcdToD10(&Rtc);
  LcdTime(Rtc.Hour,Rtc.Minute,1);
  
  i=0;
  
        #if RF_MULTI_MNJ==1
          Uart_1.RxBuf[i++]= Uart_1.RxBuf[0];     
        #else
           Uart_1.RxBuf[i++]=StateB.Address;    
        #endif
  

  Uart_1.RxBuf[i++]=0x06;
  Uart_1.RxBuf[i++]=0x00;
  Uart_1.RxBuf[i++]=0x02;
  
  unsigned short crc16;
  crc16=GetCrc16(Uart_1.RxBuf,i);
  Uart_1.RxBuf[i++]=0xff&(crc16>>8);
  Uart_1.RxBuf[i++]=0xff&(crc16);
  
  U1SendBuf(Uart_1.RxBuf,i);
}
void PC_SetChALH(void)
{
  u16 i;
  u8 buf[5];
  buf[0]=Uart_1.RxBuf[6];//下限L
  buf[1]=Uart_1.RxBuf[5];//下限H
  buf[2]=Uart_1.RxBuf[8];//上限L
  buf[3]=Uart_1.RxBuf[7];//上限H
  buf[4]=Uart_1.RxBuf[9];//开关
  alarm_buf_to_flash(buf,Uart_1.RxBuf[4]-1,1);
  
  i=0;
  
        #if RF_MULTI_MNJ==1
          Uart_1.RxBuf[i++]= Uart_1.RxBuf[0];     
        #else
           Uart_1.RxBuf[i++]=StateB.Address;    
        #endif
  

  Uart_1.RxBuf[i++]=0x06;
  Uart_1.RxBuf[i++]=0x00;
  Uart_1.RxBuf[i++]=0x03;
  
  unsigned short crc16;
  crc16=GetCrc16(Uart_1.RxBuf,i);
  Uart_1.RxBuf[i++]=0xff&(crc16>>8);
  Uart_1.RxBuf[i++]=0xff&(crc16);
  
  U1SendBuf(Uart_1.RxBuf,i);
}
void PC_SetRecordTT(void)
{
  u16 i;

  PcBsq.RecTime=U8_TO_U32(Uart_1.RxBuf[4],Uart_1.RxBuf[5],Uart_1.RxBuf[6],Uart_1.RxBuf[7]);
  
  i=0;
  
        #if RF_MULTI_MNJ==1
          Uart_1.RxBuf[i++]= Uart_1.RxBuf[0];     
        #else
           Uart_1.RxBuf[i++]=StateB.Address;    
        #endif
  

  Uart_1.RxBuf[i++]=0x06;
  Uart_1.RxBuf[i++]=0x00;
  Uart_1.RxBuf[i++]=0x04;
  
  unsigned short crc16;
  crc16=GetCrc16(Uart_1.RxBuf,i);
  Uart_1.RxBuf[i++]=0xff&(crc16>>8);
  Uart_1.RxBuf[i++]=0xff&(crc16);
  
  U1SendBuf(Uart_1.RxBuf,i);
  
  
  PCBSQ_RecTime_ToEE();
  
  //RestartClr();
}
void PC_SetRecordCap(void)
{
  u16 i;

  PcBsq.RecCap=U8_TO_U32(Uart_1.RxBuf[4],Uart_1.RxBuf[5],Uart_1.RxBuf[6],Uart_1.RxBuf[7]);
  
  i=0;
  Uart_1.RxBuf[i++]=StateB.Address;
  Uart_1.RxBuf[i++]=0x06;
  Uart_1.RxBuf[i++]=0x00;
  Uart_1.RxBuf[i++]=0x07;
  
  unsigned short crc16;
  crc16=GetCrc16(Uart_1.RxBuf,i);
  Uart_1.RxBuf[i++]=0xff&(crc16>>8);
  Uart_1.RxBuf[i++]=0xff&(crc16);
  
  U1SendBuf(Uart_1.RxBuf,i);
  
  PCBSQ_CapToEE();//设置记录容量
  
  PCBSQ_RecClr();//清空
}

void PC_SetRecordClr(void)
{
  u16 i;

  i=0;
  Uart_1.RxBuf[i++]=StateB.Address;
  Uart_1.RxBuf[i++]=0x06;
  Uart_1.RxBuf[i++]=0x00;
  Uart_1.RxBuf[i++]=0x08;
  
  unsigned short crc16;
  crc16=GetCrc16(Uart_1.RxBuf,i);
  Uart_1.RxBuf[i++]=0xff&(crc16>>8);
  Uart_1.RxBuf[i++]=0xff&(crc16);
  
  U1SendBuf(Uart_1.RxBuf,i);

  PCBSQ_RecClr();//清空
  
  RestartClr();
  

  
}
void PC_ReadDownInfor(void)
{
        u16 i;
        u8 RecByte;
        
        
        //取下载信息
#if ((GPRS_ENABLE==1)&&(PRINT_NEW_EN==1))

        //  if(Print.NewNum>PRINT_NEW_CAP)
        //  Print.NewNum=PRINT_NEW_CAP;
        
        Download.BagMaxRecordNum=(u16)PCBSQ_BAG_SIZE/PRINT_CH_BYTE;//每包最大条数
        Download.DownloadNum=Print.NewNum;//下载总条数
        if(Download.DownloadNum>=PRINT_NEW_CAP)//记录已满
          Download.DownloadNum=PRINT_NEW_CAP;
        Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;//总包数
        if(Download.DownloadNum%Download.BagMaxRecordNum!=0)
          Download.BagNum++;
        
         RecByte=PRINT_CH_BYTE;//每条数据字节数
        
#else
        

        if(PcBsq.Rec16Num>PcBsq.RecCap)
           PcBsq.Rec16Num=PcBsq.RecCap;
        
        
        RecByte=StateC.RecordBytes;//每条数据字节数
        
        
        #if BSQ_RTC4_EN==1
          Download.BagMaxRecordNum=(u16)PCBSQ_BAG_SIZE/(RecByte+2);//每包最大条数
        #else
          Download.BagMaxRecordNum=(u16)PCBSQ_BAG_SIZE/RecByte;//每包最大条数
        #endif
        
        Download.DownloadNum=PcBsq.Rec16Num;//下载时已记录条数

        if(Download.DownloadNum>=PcBsq.RecCap)//记录已满
        {
          Download.DownloadNum=PcBsq.RecCap;//下载总条数
        }
        Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;//总包数
        if(Download.DownloadNum%Download.BagMaxRecordNum!=0)
          Download.BagNum++;
        
#endif
        
        
        
        
#if RF_MULTI_MNJ==1// U1MAX 最多 100 
        
        RecByte=6+2*CH_NUM;//每条数据字节数
        
        Download.BagMaxRecordNum=(u16)90/RecByte;//每包最大条数
        Download.DownloadNum=1000;//下载时已记录条数


        Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;//总包数
        if(Download.DownloadNum%Download.BagMaxRecordNum!=0)
          Download.BagNum++;
        
        
        
        i=0;
        Uart_1.RxBuf[i++]=Uart_1.RxBuf[0];
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=10+3*CH_NUM;//字节数
        Uart_1.RxBuf[i++]=0xff&(Download.BagNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagNum);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>24);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>16);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum);
        
        Uart_1.RxBuf[i++]=RecByte;
        
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum);
        
        Uart_1.RxBuf[i++]=CH_NUM;
        
        u16 j;
        for(j=0;j<CH_NUM;j++)
        {
          Uart_1.RxBuf[i++]=*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES);
        }
        
        Pt.data=0;
        //Pt.rtc=86400*366*4;
        Pt.cc=100;
        
        
        Pt.rtc=ReadRtcD10();
        Pt.rtc=Pt.rtc-(u32)1001*60;
        Pt.rtc=(Pt.rtc/60)*60;
        
#elif ((RF_MULTI_EN==1)&&(RF_MULTI_MNJ==0))

        
        volatile u8 addr,TTflag;
        addr=Uart_1.RxBuf[0];//地址
        
        if(addr>Pt.KpN)//不能超过最大数量  zz
          return;
        

        TTflag=0;
        if(0x80&Pt.RfChN[addr-1])
          TTflag=1;
      
        i=0;
        Uart_1.RxBuf[i++]=addr;
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=10+3*CH_NUM;//字节数
        
        //下载总包数（2）+下载总条数（4）+每条字节数（1）+ 
        //下载包满包时最大条数（2）+通道数n(1)+通道1...n(类型(1)+单位(1)+数据格式(1))

        Uart_1.RxBuf[i++]=0xff&(Download.BagNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagNum);
        
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>24);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>16);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum);
        
        Uart_1.RxBuf[i++]=10;
        
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum);
        
        Uart_1.RxBuf[i++]=2;
        
        
        Uart_1.RxBuf[i++]=0x01;//温度，湿度
        Uart_1.RxBuf[i++]=0x01;
        Uart_1.RxBuf[i++]=0x02;
        
        if(TTflag==1)
        {
          Uart_1.RxBuf[i++]=0x01;//温度，湿度
          Uart_1.RxBuf[i++]=0x01;
          Uart_1.RxBuf[i++]=0x02;
        }
        else
        {
          Uart_1.RxBuf[i++]=0x02;
          Uart_1.RxBuf[i++]=0x02;
          Uart_1.RxBuf[i++]=0x02;
        }
        

#else
        

        i=0;
        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=10+3*CH_NUM;//字节数
        
        //下载总包数（2）+下载总条数（4）+每条字节数（1）+ 
        //下载包满包时最大条数（2）+通道数n(1)+通道1...n(类型(1)+单位(1)+数据格式(1))

        Uart_1.RxBuf[i++]=0xff&(Download.BagNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagNum);
        
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>24);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>16);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.DownloadNum);
        
        #if BSQ_RTC4_EN==1// 储存时,RTC(4)-通讯时自动转RTC(6)
            Uart_1.RxBuf[i++]=RecByte+2;
        #else
            Uart_1.RxBuf[i++]=RecByte;
        #endif
                
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagMaxRecordNum);
        
        Uart_1.RxBuf[i++]=CH_NUM;
  
        u16 j;
        for(j=0;j<CH_NUM;j++)
        {
          Uart_1.RxBuf[i++]=*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES);
          Uart_1.RxBuf[i++]=*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES);
        }
        
#endif

        
        unsigned short crc16;
        crc16=GetCrc16(Uart_1.RxBuf,i);
        Uart_1.RxBuf[i++]=0xff&(crc16>>8);
        Uart_1.RxBuf[i++]=0xff&(crc16);
        
        U1SendBuf(Uart_1.RxBuf,i);

}

void PC_ReadDownBag(void)
{
    u16 i;
    //u32 EE_32Pt;
    
    Download.BagPt=(u16)256*Uart_1.RxBuf[4]+Uart_1.RxBuf[5];//下载包序号
    if(Download.BagPt+1>Download.BagNum)return;//包序号错误
    
#if RF_MULTI_MNJ==1

    if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
    {
      Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
    }
    else
    {
      Download.BagRecordNum=Download.BagMaxRecordNum;//满包
    }
    Download.BagDataBytes=Download.BagRecordNum*(6+2*CH_NUM);
            
    i=0;
    Uart_1.RxBuf[i++]=Uart_1.RxBuf[0];
    Uart_1.RxBuf[i++]=0x03;
    Uart_1.RxBuf[i++]=Download.BagDataBytes+2;
    Uart_1.RxBuf[i++]=0xff&(Download.BagPt>>8);
    Uart_1.RxBuf[i++]=0xff&(Download.BagPt);
    
    for(u16 j=0;j<Download.BagRecordNum;j++)
    {
        
        if(Download.BagPt!=Pt.cc)
        {
          Pt.data++;
          Pt.rtc+=60;
        }
        

        StrcutRtc RR;
        SecondsToDate(Pt.rtc,&RR);
        RtcD10ToBcd(&RR);
      
        Uart_1.RxBuf[i++]=RR.Year;
        Uart_1.RxBuf[i++]=RR.Month;
        Uart_1.RxBuf[i++]=RR.Day;
        Uart_1.RxBuf[i++]=RR.Hour;
        Uart_1.RxBuf[i++]=RR.Minute;
        Uart_1.RxBuf[i++]=RR.Second;
        
        u16 k;
        for(k=0;k<CH_NUM;k++)
        {
            Uart_1.RxBuf[i++]=Pt.data>>8;
            Uart_1.RxBuf[i++]=Pt.data;
        }
        

        //温湿度
        //Uart_1.RxBuf[i++]=Pt.data>>8;
        //Uart_1.RxBuf[i++]=Pt.data;
        //Uart_1.RxBuf[i++]=(1000-Pt.data)>>8;
        //Uart_1.RxBuf[i++]=1000-Pt.data;
        
      
    }
    Pt.cc=Download.BagPt;
    
    
#elif ((RF_MULTI_EN==1)&&(RF_MULTI_MNJ==0))
    
    
        u32 EE_32Pt,j;
        u8 RecByte=10;//一条记录里总共StateC.RecordBytes字节，一个卡片占10个字节
        u8 KpAddr=Uart_1.RxBuf[0];
        
        
        if(KpAddr>Pt.KpN)//不能超过最大数量  zz
          return;

    
        if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
        {
          Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
        }
        else
        {
          Download.BagRecordNum=Download.BagMaxRecordNum;//满包
        }
        Download.BagDataBytes=Download.BagRecordNum*StateC.RecordBytes;
       
        i=0;
        Uart_1.RxBuf[i++]=KpAddr;
        Uart_1.RxBuf[i++]=0x03;
        Uart_1.RxBuf[i++]=Download.BagRecordNum*RecByte+2;
        Uart_1.RxBuf[i++]=0xff&(Download.BagPt>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagPt);
        
        if(Download.DownloadNum<PcBsq.RecCap)//记录未满
        {
           EE_32Pt=(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
           
           for(j=0;j<Download.BagRecordNum;j++)
           {
               I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+j*((u32)Pt.KpN*4+6),6,0);//时间
               i+=6;
               I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+6+(((u32)KpAddr-1)*4)+j*((u32)Pt.KpN*4+6),4,0);
               i+=4;
           }
        }
        else//记录已满
        {
          if(Download.BagPt==0)
            Download.Current32Pt=(u32)PcBsq.RecPt*StateC.RecordBytes;//第1条位置
          EE_32Pt=Download.Current32Pt+(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
          EE_32Pt=EE_32Pt%((u16)PcBsq.RecCap*StateC.RecordBytes);
          
          if(EE_32Pt+Download.BagDataBytes<=(u32)PcBsq.RecCap*StateC.RecordBytes)
          {
             
               for(j=0;j<Download.BagRecordNum;j++)
               {
                 I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+j*((u32)Pt.KpN*4+6),6,0);//时间
                 i+=6;
                 I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+6+(((u32)KpAddr-1)*4)+j*((u32)Pt.KpN*4+6),4,0);
                 i+=4;
               }
          }

          else//溢出
          {
                u32 TmpPt;
                TmpPt=(EE_32Pt+Download.BagDataBytes)%((u32)PcBsq.RecCap*StateC.RecordBytes);
                
                //I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes-TmpPt,0);
                
                u32 NN;
                NN=(Download.BagDataBytes-TmpPt)/StateC.RecordBytes;
                for(j=0;j<NN;j++)
                {
                   I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+j*((u32)Pt.KpN*4+6),6,0);//时间
                   i+=6;
                   I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt+6+(((u32)KpAddr-1)*4)+j*((u32)Pt.KpN*4+6),4,0);
                   i+=4;
                }
                
                //I2C_EE_BufferRead(&Uart_1.RxBuf[i+Download.BagDataBytes-TmpPt],0,TmpPt,0);
                NN=TmpPt/StateC.RecordBytes;
                for(j=0;j<NN;j++)
                {
                   I2C_EE_BufferRead(&Uart_1.RxBuf[i],(u32)0+j*((u32)Pt.KpN*4+6),6,0);//时间
                   i+=6;
                   I2C_EE_BufferRead(&Uart_1.RxBuf[i],(u32)0+6+(((u32)KpAddr-1)*4)+j*((u32)Pt.KpN*4+6),4,0);
                   i+=4;
                }
            
          }

        }
    

#else
    
    u32 EE_32Pt;
    #if ((GPRS_ENABLE==1)&&(PRINT_NEW_EN==1)&&(GPRS_TC_EN==0))
            //取每包数据
            if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
            {
              Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
            }
            else
            {
              Download.BagRecordNum=Download.BagMaxRecordNum;//满包
            }
            Download.BagDataBytes=Download.BagRecordNum*PRINT_CH_BYTE;
            
            i=0;
            Uart_1.RxBuf[i++]=StateB.Address;
            Uart_1.RxBuf[i++]=0x03;
            Uart_1.RxBuf[i++]=Download.BagDataBytes+2;
            Uart_1.RxBuf[i++]=0xff&(Download.BagPt>>8);
            Uart_1.RxBuf[i++]=0xff&(Download.BagPt);
            
            if(Download.DownloadNum<PRINT_NEW_CAP)//记录未满
            {
                 EE_32Pt=(u32)Download.BagPt*(Download.BagMaxRecordNum*PRINT_CH_BYTE);
                 EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                 I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes,0);
            }
            else//记录已满
            {
                if(Download.BagPt==0)
                  Download.Current32Pt=(u32)Print.NewPt*PRINT_CH_BYTE;//第1条位置
                EE_32Pt=Download.Current32Pt+(u32)Download.BagPt*(Download.BagMaxRecordNum*PRINT_CH_BYTE);
                EE_32Pt=EE_32Pt%((u16)PRINT_NEW_CAP*PRINT_CH_BYTE);
                
                if(EE_32Pt+Download.BagDataBytes<=(u32)PRINT_NEW_CAP*PRINT_CH_BYTE)
                {
                  EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes,0);
                }
                else//溢出
                {
                  u32 TmpPt;
                  TmpPt=(EE_32Pt+Download.BagDataBytes)%((u32)PRINT_NEW_CAP*PRINT_CH_BYTE);
                  
                  EE_32Pt=EE_32Pt+PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes-TmpPt,0);  
                  EE_32Pt=PRINT_CH1_ADDR;
                  I2C_EE_BufferRead(&Uart_1.RxBuf[i+Download.BagDataBytes-TmpPt],EE_32Pt,TmpPt,0);  
                }
            }
    #else
            


            
        if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
        {
          Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
        }
        else
        {
          Download.BagRecordNum=Download.BagMaxRecordNum;//满包
        }
        Download.BagDataBytes=Download.BagRecordNum*StateC.RecordBytes;
        
        
       
        i=0;
        Uart_1.RxBuf[i++]=StateB.Address;
        Uart_1.RxBuf[i++]=0x03;
        
        #if BSQ_RTC4_EN==1
           Uart_1.RxBuf[i++]=Download.BagRecordNum*(StateC.RecordBytes+2)+2;//记录和通讯时不一样，通讯时RTC(6)格式
        #else
           Uart_1.RxBuf[i++]=Download.BagDataBytes+2;
        #endif
        
        Uart_1.RxBuf[i++]=0xff&(Download.BagPt>>8);
        Uart_1.RxBuf[i++]=0xff&(Download.BagPt);
        
        if(Download.DownloadNum<PcBsq.RecCap)//记录未满
        {
           EE_32Pt=(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
           I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes,0);

        }
        else//记录已满
        {
          if(Download.BagPt==0)
            Download.Current32Pt=(u32)PcBsq.RecPt*StateC.RecordBytes;//第1条位置
          EE_32Pt=Download.Current32Pt+(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
          EE_32Pt=EE_32Pt%((u16)PcBsq.RecCap*StateC.RecordBytes);
          
          if(EE_32Pt+Download.BagDataBytes<=(u32)PcBsq.RecCap*StateC.RecordBytes)
          {
            I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes,0);
          }
          else//溢出
          {
            u32 TmpPt;
            TmpPt=(EE_32Pt+Download.BagDataBytes)%((u32)PcBsq.RecCap*StateC.RecordBytes);
            I2C_EE_BufferRead(&Uart_1.RxBuf[i],EE_32Pt,Download.BagDataBytes-TmpPt,0);  
            I2C_EE_BufferRead(&Uart_1.RxBuf[i+Download.BagDataBytes-TmpPt],0,TmpPt,0);  
          }
        }
        
        
        #if BSQ_RTC4_EN==1
        u16 j,k;
        u32 BB;
        StrcutRtc time_bcd;
        
        for(j=0;j<Download.BagDataBytes;j++)
        {
          Uart_0.RxBuf[j]=Uart_1.RxBuf[j+5];
        }
        
        for(j=0;j<Download.BagRecordNum;j++)
        {
             //RTC(4)-> RTC(6)
             BB=U8_TO_U32(Uart_0.RxBuf[j*StateC.RecordBytes+0],Uart_0.RxBuf[j*StateC.RecordBytes+1],Uart_0.RxBuf[j*StateC.RecordBytes+2],Uart_0.RxBuf[j*StateC.RecordBytes+3]);
             
             SecondsToDate(BB,&time_bcd);
             RtcD10ToBcd(&time_bcd);
             
             Uart_1.RxBuf[i++]=time_bcd.Year;
             Uart_1.RxBuf[i++]=time_bcd.Month;
             Uart_1.RxBuf[i++]=time_bcd.Day;
             Uart_1.RxBuf[i++]=time_bcd.Hour;
             Uart_1.RxBuf[i++]=time_bcd.Minute;
             Uart_1.RxBuf[i++]=time_bcd.Second;
             
             for(k=0;k<CH_NUM;k++)
             {
               Uart_1.RxBuf[i++]=Uart_0.RxBuf[j*StateC.RecordBytes+k*2+4];
               Uart_1.RxBuf[i++]=Uart_0.RxBuf[j*StateC.RecordBytes+k*2+5];
             }
        }
        #endif
        
        
    #endif
    
    
        
    #if BSQ_RTC4_EN==0
       i=i+Download.BagDataBytes;
    #endif
    
    
#endif
    
    unsigned short crc16;
    crc16=GetCrc16(Uart_1.RxBuf,i);
    Uart_1.RxBuf[i++]=0xff&(crc16>>8);
    Uart_1.RxBuf[i++]=0xff&(crc16);

    U1SendBuf(Uart_1.RxBuf,i);
}

void CmdModbusDeel(void)
{
  if(Uart_1.RxBuf[1]==0x03)//读---------------------------------------------
  {
    
    #if RF_MULTI_EN==1
           if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x01))//PC读实时数据 
           {
            _DINT();
            PC_MULTI_ReadReal();
            _EINT();
           }
           else if ((Uart_1.RxBuf[2]==0x01)&&(Uart_1.RxBuf[3]==0x04))//RTU读实时数据   
           {
              _DINT();
              RTU_MULTI_ALL_ReadReal();
              _EINT();
           }
           
           
           else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x05))//PC读下载信息
             PC_ReadDownInfor();
           else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x06))//PC读下载包
             PC_ReadDownBag();

          #if RF_MULTI_MNJ==1
              else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x0A))//PC读报警参数
                PC_ReadAlarm();
              else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x09))//PC读记录参数
                PC_ReadState();
          #endif

    #else
          if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x01))//PC读实时数据 
            PC_ReadReal();
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x0A))//PC读报警参数
            PC_ReadAlarm();
          
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x05))//PC读下载信息
            PC_ReadDownInfor();
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x06))//PC读下载包
            PC_ReadDownBag();
          
          #if GPRS_ENABLE==0
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x09))//PC读记录参数
            PC_ReadState();
          else if ((Uart_1.RxBuf[2]==0x01)&&(Uart_1.RxBuf[3]==0x04))//RTU读实时数据   
            RTU_ReadReal();
          
          #endif

          #if RS485_ENABLE==1
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x0B))
            PC_ReadSMSData();
          
          else if ((Uart_1.RxBuf[2]==0x01)&&(Uart_1.RxBuf[3]==0x40))//MCGS读实时数据   01 03 01 40 
            MCGS_ReadReal();
          
          #endif
         
    #endif
  }
  #if ((RF_MULTI_EN==0)||(RF_MULTI_MNJ==1))
  else if(Uart_1.RxBuf[1]==0x06)//写-----------------------------------------
  {
          if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x03))//PC设置超标上下限
            PC_SetChALH();
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x02))//PC写RTC
            PC_SetRtc();

          #if GPRS_ENABLE==0
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x04))//PC设置记录间隔
            PC_SetRecordTT();
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x08))//PC设置记录清空
            PC_SetRecordClr();
          else if((Uart_1.RxBuf[2]==0x00)&&(Uart_1.RxBuf[3]==0x07))//PC设置记录容量
            PC_SetRecordCap();
          #endif

  }
  #endif
}
#endif

void CmdHexDownPag(void)
{
      #if ((RECORD_ENABLE==1)||(GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))
      if(Uart_1.RxBuf[2]==READ_DOWNLOAD_BAG)
      {
          Download.TimeOut=0;
          
          #if ((GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))
              Read_GPRS_DATA_Bag();
          #else
              ReadDownloadBag();
          #endif
      }
      #endif
}

void CmdAsciiDownPag(void)
{
    #if RECORD_ENABLE==1
    if((Uart_1.RxBuf[0]=='T')&&(Uart_1.RxBuf[Uart_1.RecLen-1]=='K'))
    {
      if((Uart_1.RxBuf[3]=='1')&&(Uart_1.RxBuf[4]=='2'))//T011203PPPK
      {
          Download.TimeOut=0;
          ReadDownloadBag();
      }
    }
    #endif
}

void CmdHexDeel(void)
{
            if(Uart_1.RxBuf[2]==READ_LINK)
              ReadLink();
            

            else if(Uart_1.RxBuf[2]==READ_ADC)
              ReadAdc();
#if OPT_EN==0
            else if(Uart_1.RxBuf[2]==READ_REAL_VALUE)//开放该指令 zz
              ReadRealValue();
            else if(Uart_1.RxBuf[2]==SET_ADDRESS)
              SetAddress();
#endif
            
            
#if OPT_AD_EN==0
            else if(Uart_1.RxBuf[2]==SET_ADC_MODE)
              SetAdcMode();
#endif
            
            
            else if(Uart_1.RxBuf[2]==SET_RTC)
              SetRtc();
            else if(Uart_1.RxBuf[2]==SET_RE_START)
              SetReStart();

            else if(Uart_1.RxBuf[2]==READ_CURVE)
              ReadCurve();
            else if(Uart_1.RxBuf[2]==SET_CURVE)
              SetCurve();
            else if(Uart_1.RxBuf[2]==SET_MODEL_SERIAL_NUM)
              SetModelSerialNum();
            else if(Uart_1.RxBuf[2]==FACTORY_SETTING)
              SetFactorySetting();

            else if(Uart_1.RxBuf[2]==SET_STATEB)
              SetStateB();
            else if(Uart_1.RxBuf[2]==SET_SMS_TITLE)
              SetSmsTitle();
            else if(Uart_1.RxBuf[2]==READ_SMS_TITLE)
              ReadSmsTitle();
            
            #if BOOT_EN==1
            else if(Uart_1.RxBuf[2]==CMD_RESET)
            {
                //DogReset();//zz
                WDTCTL=0;
            }
            #endif
            
            #if ((RF_REC_EN==1)||(RF_RP_EN==1))
            else if(Uart_1.RxBuf[2]==CMD_RF_PAIR)
                RfPair();
            else if(Uart_1.RxBuf[2]==CMD_SET_FREQ)
                SetFreq();
            #elif RF_KP_EN==1
            else if(Uart_1.RxBuf[2]==CMD_SET_FREQ)
                SetFreq();
            #endif
            
            
            #if ((RF_MULTI_EN==1)||(RF_RP_EN==1))
            else if(Uart_1.RxBuf[2]==READ_MULTI_INFOR)
                Read_Multi_Infor();
            #endif

            #if RF_MULTI_EN==1
            else if(Uart_1.RxBuf[2]==CMD_SET_RF_KP)
                SetRfkp();
            else if(Uart_1.RxBuf[2]==CMD_READ_RF_KP)
                ReadRfkp();     
            #endif
            
            #if ZX_EN==1
            else if(Uart_1.RxBuf[2]==CMD_SET_CD)
                SetCd();
            else if(Uart_1.RxBuf[2]==CMD_READ_CD)
                ReadCd();     
            #endif            

            else if(Uart_1.RxBuf[2]==SET_ALARM)//开放该指令 zz
              SetAlarm();
            
            #if BSQ_ENABLE==0
            else if(Uart_1.RxBuf[2]==READ_BATTERY_CAPACITY)
              ReadBatteryCapacity();
            else if(Uart_1.RxBuf[2]==SET_STOP)
              SetStop();
            #endif

            #if RY_CONTROL_ENABLE==1
            else if(Uart_1.RxBuf[2]==SET_CONTROL)
              SetControl();
            else if(Uart_1.RxBuf[2]==READ_CONTROL)
              ReadControl();
            #endif
    
            #if LCD08_SD_SHIFT_EN==1
            else if(Uart_1.RxBuf[2]==SET_SD_SHIFT_EN)
              CmdSetSdShift();
            else if(Uart_1.RxBuf[2]==READ_SD_SHIFT_EN)
              CmdReadSdShift();
            #endif
            
            #if PRINT_REAL_EN==1
            else if(Uart_1.RxBuf[2]==SET_PRINT_XX)
              CmdSetPrintXX();
            else if(Uart_1.RxBuf[2]==READ_PRINT_XX)
              CmdReadPrintXX();
            #endif
            
            #if GPRS_ENABLE==1
            else if(Uart_1.RxBuf[2]==SET_GPRS_INFO)
              SetGprsInfo();
            else if(Uart_1.RxBuf[2]==READ_GPRS_INFO)
              ReadGprsInfo();
            #endif
            
            
            #if RF_MULTI_EN==1
            else if(Uart_1.RxBuf[2]==GSM_MONTIOR_SET)
              SetGsmMontior();          
            #endif

            
            #if GSM_ENABLE==1
            else if(Uart_1.RxBuf[2]==GSM_MONTIOR_SET)
              SetGsmMontior();
            else if(Uart_1.RxBuf[2]==SET_PHONE)
              SetPhone(); 
            else if(Uart_1.RxBuf[2]==READ_PHONE)
              ReadPhone();

            #endif
            
            #if SMS_INV_ENABLE==1
            else if(Uart_1.RxBuf[2]==SET_INV_TIME)
              SetInvInfor();
            else if(Uart_1.RxBuf[2]==READ_INV_TIME)
              ReadInvInfor();
            else if(Uart_1.RxBuf[2]==READ_BATT_TIME)
              ReadBattTime();
           #endif

            #if ((RECORD_ENABLE==1)||(GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))//zz
            else if(Uart_1.RxBuf[2]==READ_DOWNLOAD_INFOR)
            {
 
                  #if ((GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))
                     Download.TimeOut=0;
                     Read_GPRS_DATA_Infor();
                     
                      if(Download.DownloadNum>0)//zz
                      {
                        Download.Downloading=1;
                        Download.TimeOut=0;
                        Download.LastBagPt=0;
                        delay_us(2000);
                        
                        U1Init(BAUDRATE_DOWNLOAD,1);
                        
                      }
                  
                  #else
    
                      ReadDownloadInfor();
                      if(Download.DownloadNum>0)
                      {
                        Download.Downloading=1;
                        Download.TimeOut=0;
                        Download.LastBagPt=0;
                        delay_us(2000);
                        
                        U1Init(BAUDRATE_DOWNLOAD,1);
                        
                        #if WIRELESS_ENABLE==1
                        if(Uart_0.FlagFinish==1)//无线时波特率为9600
                        {
                          U0Init(BAUDRATE_RF_DOWNLOAD,1);//采用 SMCLK
                        }
                        #endif
                      }
                  
                  #endif
              
            }
            
            #endif
}



//-------------------------------------------------------------------------
u8 CmdSendStart(u8 CMD,u16 Len)
{

  u8 add=0;
  add+=U1SendByte(0xbb);
  
  #if RF_MULTI_MNJ==1
    add+=U1SendByte(Uart_1.RxBuf[1]);
    
 
    
  #else
    add+=U1SendByte(StateB.Address);
  #endif
  
  add+=U1SendByte(CMD);
  add+=U1SendByte(0xff&Len);
  add+=U1SendByte(0xff&(Len>>8));
  return add;


}
void CmdSendEnd(u8 add)
{
  U1SendByte(add);
  U1SendByte(0xdd);
}

void ReadLink(void)
{
  u16 j;
  u8 add=0;
  
  add=CmdSendStart(READ_LINK,126+12*CH_NUM);
  
  #if BSQ_ENABLE==1
      //记录条数
      #if ((GPRS_ENABLE==1)&&(PRINT_NEW_EN==1))//GPRS打印缓冲区
  
        #if GPRS_TC_EN==1
          add+=U1SendByte(0xff&PcBsq.Rec16Num);
          add+=U1SendByte(0xff&(PcBsq.Rec16Num>>8));
          add+=U1SendByte(0);
          add+=U1SendByte(0);
        #else
          add+=U1SendByte(0xff&Print.NewNum);
          add+=U1SendByte(0xff&(Print.NewNum>>8));
          add+=U1SendByte(0);
          add+=U1SendByte(0);
        #endif
          
      #else
          add+=U1SendByte(0xff&PcBsq.Rec16Num);
          add+=U1SendByte(0xff&(PcBsq.Rec16Num>>8));
          add+=U1SendByte(0);
          add+=U1SendByte(0);
      #endif
  
      //zz 是通过启动时间来判断是否是变送器还是记录仪
      for(j=4;j<23;j++)
      {
        if(j==9)//工作状态
        {
          #if GSM_KEY_SHUT_EN==1
            if(Sms.GsmKeyShut==1)//按钮关闭
            {
              add+=U1SendByte(0x00);
            }
            else
            {
              add+=U1SendByte(0x01);
            }
          #else
            add+=U1SendByte(0x01);
          #endif
        }
        #if ((GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))
        else if((j>=11)&&(j<=16))
        {
          add+=U1SendByte(0x01);//GPRS数据下载用AA协议标识
        }
        #endif
        else
        {
          add+=U1SendByte(0x00);
        }
      }

      for(j=0;j<84;j++)
      {

        if(j==4)
        {
          #if ((GPRS_ENABLE==1)&&(PRINT_NEW_EN==1))//GPRS打印缓冲区
          
              u32 time;
              #if GPRS_TC_EN==1
                 time=PcBsq.RecTime;
              #else
                 time=PRINT_JIANGE;       
              #endif
            
              add+=U1SendByte(0xff&time);
              add+=U1SendByte(0xff&(time>>8));
              add+=U1SendByte(0xff&(time>>16));
              add+=U1SendByte(0xff&(time>>24));
          #else
              add+=U1SendByte(0xff&PcBsq.RecTime);
              add+=U1SendByte(0xff&(PcBsq.RecTime>>8));
              add+=U1SendByte(0xff&(PcBsq.RecTime>>16));
              add+=U1SendByte(0xff&(PcBsq.RecTime>>24));
          #endif
          j=7;
        }
        else if(j==9)//停止方式，先进先出
        {
          add+=U1SendByte(STOP_MODE_FIFO);
        }
        else if(j==12)
        {
          
          #if RF_MULTI_MNJ==1
                  add+=i2c_ee_read_to_pc(USER_NAME_ADDR,57,0);//用户名+标题+型号+SN
                  
          
                  //读实时数据时，SN号前7位不变，后3位 定义为 传感器地址号
                  add+=U1SendByte((Uart_1.RxBuf[1]/100)%10+0x30);
                  add+=U1SendByte((Uart_1.RxBuf[1]/10)%10+0x30);
                  add+=U1SendByte((Uart_1.RxBuf[1]/1)%10+0x30);
          
                  j=71;
                  
          #else
                  add+=i2c_ee_read_to_pc(USER_NAME_ADDR,60,0);//用户名+标题+型号+SN
                  j=71;
                    
          #endif
          
        }
        else
        {
          add+=U1SendByte(0x01);
        }
      }
  #else
        //A1(6)
        add+=U1SendByte(0xff&StateA1.RecordNum);
        add+=U1SendByte(0xff&(StateA1.RecordNum>>8));
        add+=U1SendByte(0xff&(StateA1.RecordNum>>16));
        add+=U1SendByte(0xff&(StateA1.RecordNum>>24));
        add+=U1SendByte(0xff&StateA1.CurrentBufPt);
        add+=U1SendByte(0xff&(StateA1.CurrentBufPt>>8));
        
        //A2(17)
        add+=i2c_ee_read_to_pc(CURRENT_SAVE_PAGE_ADDR,3,0);
        
        //手动模式.假停时，工作状态返回为停止记录
        if((StateB.StartMode==START_MODE_HAND)\
           &&(StateB.StopMode==STOP_MODE_HAND)\
              &&(Flag.HandOff==1))
        {
          add+=U1SendByte(WORK_STATE_STOP);
          add+=U1SendByte(OFF_HAND);
        }
        else
        {
          add+=i2c_ee_read_to_pc(WORK_STATE_ADDR,1,0);
          add+=i2c_ee_read_to_pc(STOP_CAUSE_ADDR,1,0);
        }
        
        if(StateA2.StopCause==OFF_LATER_TIME_START)//定时启动模式时，启动时间改为定时时间
        {
          add+=i2c_ee_read_to_pc(LATER_START_TIME_ADDR,6,0);
        }
        else
        {
          add+=i2c_ee_read_to_pc(START_TIME_ADDR,6,0);
        }
        add+=i2c_ee_read_to_pc(STOP_TIME_ADDR,6,0);
        
        //工作参数B （84）
        add+=i2c_ee_read_to_pc(START_LATER_TIME_ADDR,84,0);
        
  #endif 
   
  //仪器信息变量 （16）
    #if BSQ_ENABLE==1
          #if ((GPRS_ENABLE==1)&&(PRINT_NEW_EN==1)&&(GPRS_TC_EN==0)&&(BSQ_REC_AA_EN==0))

            //打印条数即是 记录条数
            add+=U1SendByte(0xff&PRINT_NEW_CAP);
            add+=U1SendByte(0xff&(PRINT_NEW_CAP>>8));
            add+=U1SendByte(0xff&(PRINT_NEW_CAP>>16));
            add+=U1SendByte(0xff&(PRINT_NEW_CAP>>24));
            
          #else
            
            add+=U1SendByte(0xff&PcBsq.RecCap);
            add+=U1SendByte(0xff&(PcBsq.RecCap>>8));
            add+=U1SendByte(0xff&(PcBsq.RecCap>>16));
            add+=U1SendByte(0xff&(PcBsq.RecCap>>24));
          #endif
    #else
          add+=U1SendByte(0xff&StateC.RecordMaxNum);
          add+=U1SendByte(0xff&(StateC.RecordMaxNum>>8));
          add+=U1SendByte(0xff&(StateC.RecordMaxNum>>16));
          add+=U1SendByte(0xff&(StateC.RecordMaxNum>>24));
    #endif
  
  add+=U1SendByte(P_VER);
  add+=U1SendByte(C_VER);

  
  if(StateA2.WorkState==WORK_STATE_RECORDING)//3.4
  {
      #if BAT_AD_ENABLE==1
      if(((StateB.StartMode==START_MODE_HAND)||(StateB.StopMode==STOP_MODE_HAND))&&(Flag.HandOff==1))
      {
          add+=U1SendByte(0x00);//电池电压正常
      }
      else
      {
          if(StateC.BatV<BAT_LOW)//电池电压偏低
            add+=U1SendByte(0x01);
          else
            add+=U1SendByte(0x00);//电池电压正常
      }
      #else
         add+=U1SendByte(0x00);//电池电压正常
      #endif
  }
  else//停止记录
  {
    add+=U1SendByte(0x00);//电池电压正常
  }
  
  
  #if CH_PAIR_ENABLE==1
  add+=U1SendByte(0x03);//通道成对
  #else
  add+=U1SendByte(0x02);
  #endif
  
  
    #if WIRELESS_ENABLE==1
      if(Uart_0.FlagFinish==1)
      {
          add+=U1SendByte(0x01);//无线时为9600
      }
      else
      {
          //有线通讯
          #if BAUDRATE_DOWNLOAD==9600
            add+=U1SendByte(0x01);
          #elif BAUDRATE_DOWNLOAD==115200
            add+=U1SendByte(0x02);
          #endif
      }
    #else
          #if BAUDRATE_DOWNLOAD==9600
            add+=U1SendByte(0x01);
          #elif BAUDRATE_DOWNLOAD==115200
            add+=U1SendByte(0x02);
          #endif
    #endif
  
  #if BOOT_EN==1
     add+=U1SendByte(0x01);//可升级标志       
  #else
     add+=U1SendByte(0xb2);
  #endif
     
  #if RF_MULTI_EN==1
     add+=U1SendByte(0xA0);//无线接收机标志    
  #else
     add+=U1SendByte(0xc1);
  #endif
     
  #if SIM800C_EN==1
     #if M26_EN==1
       add+=U1SendByte(0x03);//S3
     #else
       add+=U1SendByte(0x01);//S2
     #endif
  #else
    add+=U1SendByte(0xc2);   
  #endif
    
    
  #if L_Stime_First_EN==1
    add+=U1SendByte(0x11);//L系列第一个记录点是否包含启动时间(1)   0x11包含启动时间,否则不包含  
  #else  
    add+=U1SendByte(0xc3);//预留  
  #endif  
    
  /* 8位标志
  (0)打印                 Pn  P1打印1路
  (1)基站定位返回经纬度   L
  (2)基站定位返回基站信息 A
  (3)GPS定位              G
  (4)门磁开关 一路        M1  
  (5)门磁开关 两路        M2 
  (6)报警延时             Y10   报警延时10分钟---------
  (7)F248                 采用F248芯片
    */
    
  u8 cc=0;
  
  #if ((PRINT_NEW_EN==1)&&(RRINT_NEW_N_EN==1)&&(RS232_ENABLE==1))//P
  cc|=BIT0;
  #endif
  
  //P后带打印通道数
  
  #if LBS_JWD_EN==1//L
  cc|=BIT1;  
  #endif
  
  #if LBS_LAC_EN==1//A
  cc|=BIT2;  
  #endif
  
  #if GPS_MOUSE_EN==1//S
  cc|=BIT3;
  #endif
  
  #if DOOR1_BJ_EN==1//M1
  cc|=BIT4;
  #endif
  
  #if DOOR2_BJ_EN==1//M2, 门磁数量，如果没有二号门，只有一号门显示 M1
  cc|=BIT5;
  #endif
  
  #if SMS_IN_AL_EN==1//报警延时----------------------
  cc|=BIT6;
  #endif
  
  
  #if F248_EN==1//F248标志
  cc|=BIT7;  
  #endif
  
  //0XC4
  
  
  add+=U1SendByte(cc);
  
  #if ((PRINT_NEW_EN==1)&&(RRINT_NEW_N_EN==1)&&(RS232_ENABLE==1))//打印的通道数
    /*
     1. 打印温度1
     2. 打印温度1温度2
     3. 打印全部温度
     4. 打印全部通道
     5. 打印温度1湿度1
     6. 分组打印温度
     7. 分组打印温度湿度
    */
     u8 pn=0;
     #if PRINT_ALL_CH_EN==1//打印全部通道
       pn=4;
     #elif RPTINT_ONLY_WD_EN==1//全部温度
       pn=3;
     #elif PRINT_2WD_EN==1//打印2路温度
       pn=2;
     #elif PRINT_1W1S_EN==1//打印1温1湿
       pn=5;
     #elif PRINT_FZ_EN==1//分组打印
       #if PRINT_FZ_WD_EN==1
       pn=6;
       #else
       pn=7;    
       #endif
     #else//打印1路温度
       pn=1;
     #endif
       
     
     add+=U1SendByte(pn);
  #else
     add+=U1SendByte(0xc5);
  #endif
  
     
    u8 gn2=0;
  #if TCP_SET_EN==1
    gn2|=BIT0;//双向控制
  #endif

  #if HTU_ENABLE==1  
    gn2|=BIT1;//HIH6130/SHT30探头
  #endif  
    
  #if SMS_TCP_EN==1  
    gn2|=BIT2;//短信平台发送
  #endif
  //gn2=0xc6;


  
  add+=U1SendByte(gn2);
  
  
  //通道配置
  add+=U1SendByte(CH_NUM);
  for(j=0;j<CH_NUM;j++)
  {
    add+=U1SendByte(*((char *)CH1_A_L_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_A_L_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_A_H_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_A_H_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_A_EN_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_V_L_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_V_L_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_V_H_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_V_H_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    
    add+=U1SendByte(*((char *)CH1_T_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_U_ADDR+j*ONE_CH_CONFIG_BYTES));
    
    //--------------------------------------------------------------------
    add+=U1SendByte(*((char *)CH1_S_ADDR+j*ONE_CH_CONFIG_BYTES));
  }
  

  
  CmdSendEnd(add);
  
  StateA1ToEE();//存入EE,确保连接时，得到最新的记录条数
  
  Flag.AdcMd=0;//退出AD调整模式 zz
  
  
}
void SetAdcMode(void)
{
  u8 add=0;
  
  if(Uart_1.RxBuf[5]==0x01)
  {
    Flag.AdcMd=1;//进入
  }
  else
  {
    Flag.AdcMd=0;//退出
  }

  add=CmdSendStart(SET_ADC_MODE,3);

  add+=U1SendByte(Flag.AdcMd);
  CmdSendEnd(add);

}
void SetFactorySetting(void)
{
  u8 add=0;
  
  add=CmdSendStart(FACTORY_SETTING,3);

  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  
  LoggerConfigFirst();//恢复出厂设置
}
void SetModelSerialNum(void)
{
  u8 add=0;
  

  I2C_EE_BufferWrite(&Uart_1.RxBuf[5],MODEL_NO_ADDR,10,0);
  I2C_EE_BufferWrite(&Uart_1.RxBuf[15],SERIAL_NO_ADDR,10,0);
  
  add=CmdSendStart(SET_MODEL_SERIAL_NUM,3);
  
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  
  #if ((RF_REC_EN==1)||(RF_KP_EN==1))
    GetRfSnFromEE();
  #endif

}
u16 GetAsc2IntZ(u8* buf)
{
  u16 aa;
  aa=1000*(buf[1]-0x30)+100*(buf[2]-0x30)+10*(buf[3]-0x30)+(buf[4]-0x30);
  if(buf[0]=='-')
  {
    aa=0x8000|aa;
  }
  return aa;
}
void SetAlarm(void)
{
  u8 add=0;
 
  if(Uart_1.RxBuf[5]>8)return;
  alarm_buf_to_flash(&Uart_1.RxBuf[6],Uart_1.RxBuf[5]-1,1);
  
  add=CmdSendStart(SET_ALARM,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  
  if(Uart_1.RxBuf[5]==CH_NUM)
  {
    LcdAlarmEn();
    LcdMode();
    LoggerAlarmAllSeek();//立即查询
    LcdValue();
  }
  Sms.IntervalPt=10;
}
void SetSmsTitle(void)
{
  u8 add=0;
  add=CmdSendStart(SET_SMS_TITLE,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  I2C_EE_BufferWrite(&Uart_1.RxBuf[5],SMS_TITLE_ADDR,20,0);//短信标题Uni
}
void ReadSmsTitle(void)
{
  u8 add=0;
  add=CmdSendStart(READ_SMS_TITLE,22);
  add+=i2c_ee_read_to_pc(SMS_TITLE_ADDR,20,0);//短信标题Uni
  CmdSendEnd(add);
}



#if ((RF_MULTI_EN==1)||(RF_RP_EN==1))


//读取无线接收机的连接数和已连接地址
void Read_Multi_Infor(void)
{
  u8 add;
  u16 nn=0;
  u16 j;
  
  #if RF_MULTI_EN==1
          
  for(j=0;j<RF_SEN_MAX;j++)
  {
    if(Pt.RfTimeout[j]>0)
    {
       if(Pt.RfData[j][0].value!=SENIOR_NULL)
       {
          nn++;
       }
    }
  }
  
  add=0;
  add=CmdSendStart(READ_MULTI_INFOR,nn*6+19);
  
  //接收机SN
  add+=i2c_ee_read_to_pc(SERIAL_NO_ADDR,10,0);
  
  //RTC
  RtcReadTime();
  u8* xx;
  xx=&Rtc.Year;
  for(j=0;j<6;j++)
    add+=U1SendByte(*xx++);
  RtcBcdToD10(&Rtc);
  
  add+=U1SendByte(nn);//连接数
  
  for(j=0;j<RF_SEN_MAX;j++)
  {
    if(Pt.RfTimeout[j]>0)
    {
       if(Pt.RfData[j][0].value!=SENIOR_NULL)
       {
          add+=U1SendByte(j+1);//连上的地址
          
          add+=U1SendByte(Pt.Bat[j]);//电量
          
          //温度
          add+=U1SendByte(0xff&(Pt.RfData[j][0].value>>8));
          add+=U1SendByte(0xff&(Pt.RfData[j][0].value));
          
          //湿度
          add+=U1SendByte(0xff&(Pt.RfData[j][1].value>>8));
          add+=U1SendByte(0xff&(Pt.RfData[j][1].value));

       }
    }
  }
  #elif RF_RP_EN==1
  for(j=0;j<RF_RP_MAX;j++)
  {
    if(Pt.HasTimeout[j]>0)
    {
       nn++;
    }
  }
  
  add=0;
  add=CmdSendStart(READ_MULTI_INFOR,nn+3);//zz
  add+=U1SendByte(nn);//连接数
  
  for(j=0;j<RF_RP_MAX;j++)
  {
    if(Pt.HasTimeout[j]>0)
    {
        add+=U1SendByte(j+1);//连接的个地址
    }
  }
  #endif
  
  CmdSendEnd(add);
  
  #if RF_MULTI_EN==1
    Pt.pcpt=0;//接收这条指令时清0
  #endif
}
#endif


void SetPhone(void)
{
  u8 add=0;
  u8 i,j;
  u8 k;
  u8 buf[34];
  
  i=0;
  buf[i++]=Uart_1.RxBuf[5];
  for(k=0;k<3;k++)
  {
    for(j=0;j<11;j++)
    {
      buf[i++]=Uart_1.RxBuf[11*k+6+j];
    }
  }
  SetPhoneToEE(buf);
  Sms.IntervalPt=10;
  add=CmdSendStart(SET_PHONE,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
}
void ReadPhone(void)
{
  u8 j,k;
  u8 add=0;
  u8 buf[34];

  I2C_EE_BufferRead(buf,PHONE_NUM_ADDR,34,0);
  
  add=CmdSendStart(READ_PHONE,36);
  
  add+=U1SendByte(buf[0]);
  
  for(k=0;k<3;k++)
  {
    for(j=0;j<11;j++)
    {
      add+=U1SendByte(buf[k*11+1+j]);
    }
  }
  CmdSendEnd(add);

}

void SetStop(void)
{
  u8 add=0;

  add=CmdSendStart(SET_STOP,3);
  
  
  if(StateB.StopMode==STOP_MODE_HAND)
  {
    add+=U1SendByte(0x00);//手动停机模式时，屏蔽上位机停止记录功能
  }
  else
  {
    LoggerStop();
    
    StateA2.StopCause=OFF_PC;
    StateA2ToEE();

    
    EEToStateA2();

    if(StateA2.WorkState==WORK_STATE_STOP)
    {
      add+=U1SendByte(0x01);
    }
    else
    {
      add+=U1SendByte(0x00);
    }
  }
  CmdSendEnd(add);

}
void SetReStart(void)
{
  u16 j;
  u8 add=0;
  
  add=CmdSendStart(SET_RE_START,3);
  
  add+=U1SendByte(0x01);
  CmdSendEnd(add);

  StateA1.RecordNum=0;
  StateA1.CurrentBufPt=0;
  StateA1ToEE();

  StateA2.CurrentSavePage=0;
  StateA2.RecordOverNum=0;
  if(StateB.StartMode==START_MODE_RIGHT)
  {
    StateA2.WorkState=WORK_STATE_RECORDING;
        
    StateA2.StopCause=OFF_NOT;
    Pt.RecordPt=0;
    Pt.RealSamplePt=0;
	
    Rtc.SS=ReadRtcD10();
  }
  else if(StateB.StartMode==START_MODE_LATER)
  {
    LoggerStop();
    StateA2.StopCause=OFF_LATER_TIME_START;
  }
  else if(StateB.StartMode==START_MODE_HAND) 
  {
    LoggerStop();
    StateA2.StopCause=OFF_HAND;
  }
  
  for(j=0;j<6;j++)
    StateA2.StartTime[j]=Uart_1.RxBuf[5+j];//重启时间

  Rtc.Year=StateA2.StartTime[0];
  Rtc.Month=StateA2.StartTime[1];
  Rtc.Day=StateA2.StartTime[2];
  Rtc.Hour=StateA2.StartTime[3];
  Rtc.Minute=StateA2.StartTime[4];
  Rtc.Second=StateA2.StartTime[5];
  RtcSetTime();
  RtcBcdToD10(&Rtc);
  
  
  Pt.RtcPt=1;

  for(j=0;j<6;j++)
    StateA2.bb[j]=0;
  StateA2ToEE();
  
  Flag.StartSample=START_SAMPLE_TIME;
  
  Flag.HandOff=1;

    u8 BufTmp[1];
    BufTmp[0]=Flag.HandOff;
    I2C_EE_BufferWrite(BufTmp,HANG_SS_ADDR,1,0);//保存标志

  
  
  
  Sms.IntervalPt=10;
    
  RestartClr();
  
  #if PRINT_NEW_EN==1
      #if GPRS_TC_EN==1
 
          PrintDueout_Clr();//打印数据清空
          
          //PCBSQ_RecClr();//变送器数据清空
          
          //Pt.GprsDueOut=0;
          //GprsDueOutToEE();//GPRS数据清空
          
      #else
          PrintNewClr();
      #endif
  #endif
          
          
  #if ((WIFI_ENABLE==1)||(GPRS_ENABLE==1))//zz
          PCBSQ_RecClr();//变送器数据清空
          
          Pt.GprsDueOut=0;
          GprsDueOutToEE();//GPRS或WIFI上传的数据清空
  #endif

}

void ReadGprsInfo(void)
{
  u8 add=0;
  add=CmdSendStart(READ_GPRS_INFO,47);
  add+=i2c_ee_read_to_pc(GPRS_IP_ADDR,45,0);
  CmdSendEnd(add);
  
  GprsSleepFromEE();//读取省电模式
  
  #if GPRS_UP_TIME_EN==1
  Gprs_UpTime_FromEE();//读发送间隔
  #endif
  
  #if GPRS_PRT_CAP_EN==1
  Gprs_PrtCAP_FromEE();//读打印条数容量
  #endif
  
  LcdMode();
}

void SetGprsInfo(void)//设置GPRS参数
{
  #if GPRS_ENABLE==1
  u8 add=0;
  add=CmdSendStart(SET_GPRS_INFO,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  
  I2C_EE_BufferWrite(&Uart_1.RxBuf[5],GPRS_IP_ADDR,45,0);
  
  //PcBsq.RecTime 记录间隔同步
  PcBsq.RecTime=U8_TO_U32(Uart_1.RxBuf[14],Uart_1.RxBuf[13],Uart_1.RxBuf[12],Uart_1.RxBuf[11]);
  PCBSQ_RecTime_ToEE();//zz
  

  GprsSleepFromEE();//读取省电模式
  
  
  #if GPRS_UP_TIME_EN==1
  Gprs_UpTime_FromEE();//读上传间隔
  #endif
  
  #if GPRS_PRT_CAP_EN==1
  Gprs_PrtCAP_FromEE();//读打印条数容量
  #endif
  

  #if GSM_KEY_SHUT_EN==1
  if(Sms.GsmKeyShut==0)
      GsmToPowOn();//设置后，重启模块
  #endif
  
  #endif

  
  LcdMode();
}

void ReadRealValue(void)
{
  u8 j;
  u8 add=0;
  
    if(Flag.RstFirstSample==0)
    return;
  
  add=CmdSendStart(READ_REAL_VALUE,CH_NUM*2+2);
  
  for(j=0;j<CH_NUM;j++)
  {
    add+=U1SendByte(0xff&StateC.Value[j]);
    if(0x01&(StateC.FuhaoBit>>j))//负数
    {
      add+=U1SendByte(0x80|(0xff&(StateC.Value[j]>>8)));
    }
    else
    {
      add+=U1SendByte(0xff&(StateC.Value[j]>>8));
    }
  }
  CmdSendEnd(add);
}
void ReadBatteryCapacity(void)
{
  u8 add=0;
  add=CmdSendStart(READ_BATTERY_CAPACITY,1+2);
  add+=U1SendByte(GetBatt());
  CmdSendEnd(add);
}
void ReadAdc(void)
{
  
  u8 add=0;
  u8 j;
  
  add=CmdSendStart(READ_ADC,CH_NUM*2+2);

  for(j=0;j<CH_NUM;j++)
  {
    add+=U1SendByte(0xff&StateC.real_adc[j]);
    add+=U1SendByte(0xff&(StateC.real_adc[j]>>8));
  }

  CmdSendEnd(add);


}

#if ZX_EN==1
void SetCd(void)
{
      u8 add=0;
      add=CmdSendStart(CMD_SET_CD,3);
      add+=U1SendByte(0x01);
      CmdSendEnd(add);
  
      I2C_EE_BufferWrite(&Uart_1.RxBuf[5],GPRS_ZXCH_ADDR,10,0);//层号
}
void ReadCd(void)
{
      u8 add=0;
      add=CmdSendStart(CMD_READ_CD,22);
      add+=i2c_ee_read_to_pc(GPRS_ZXCH_ADDR,10,0);
      for(u8 j=0;j<10;j++)
        add+=U1SendByte(0x00);//预留 10
      CmdSendEnd(add);
  
}
#endif

#if RF_MULTI_EN==1

//从EE取 卡片信息
void  KpInfor_FromEE(void)
{
    u8 buf[2];
    
    I2C_EE_BufferRead(buf,RF_N_ADDR,1,0);//卡片数量
    Pt.KpN=buf[0];
    
    I2C_EE_BufferRead(buf,RF_REC_T_ADDR,2,0);//记录间隔(分钟)
    Pt.KpRecT=U8_TO_U16(buf[1],buf[0]);
    Pt.KpRecT=Pt.KpRecT*60;//秒
    PcBsq.RecTime=Pt.KpRecT;
    
    I2C_EE_BufferRead(buf,RF_T_OUT_ADDR,2,0);//超时时间(分钟)
    Pt.KpTout=U8_TO_U16(buf[1],buf[0]);
    Pt.KpTout=Pt.KpTout*60;//分钟转成秒    
    
    //通过卡片数量计算得到记录容量
    //一个卡片占2个通道,即4字节
    //一条记录： RTC(6)+ n*ONE(4) ,  即 6+4*Pt.KpN  字节
    
    StateC.RecordBytes=Pt.KpN;
    StateC.RecordBytes=StateC.RecordBytes*4+6;
    
    StateC.E1_MaxMem=(((u32)STATE_FIRST_ADDR-RECORD_FIRST_ADDR)/StateC.RecordBytes)*StateC.RecordBytes;
    StateC.RecordMaxNum=StateC.E1_MaxMem/StateC.RecordBytes;
    
    PcBsq.RecCap=StateC.RecordMaxNum;
 
}


void SetRfkp(void)
{
      u8 add=0;
      add=CmdSendStart(CMD_SET_RF_KP,3);
      add+=U1SendByte(0x01);
      CmdSendEnd(add);
  
      I2C_EE_BufferWrite(&Uart_1.RxBuf[5],RF_N_ADDR,1,0);//卡片数量
      
      I2C_EE_BufferWrite(&Uart_1.RxBuf[6],RF_REC_T_ADDR,2,0);//记录间隔
      
      I2C_EE_BufferWrite(&Uart_1.RxBuf[8],RF_T_OUT_ADDR,2,0);//超时时间
      
      
      PCBSQ_RecClr();//设置后，记录条数自动清空
      
      //从EE取信息
      KpInfor_FromEE();
      
}
void ReadRfkp(void)
{
      //先从EE读取参数
      KpInfor_FromEE();
  
      u8 j,add=0;
      add=CmdSendStart(CMD_READ_RF_KP,27+2);
      
      
      add+=U1SendByte(Pt.KpN);//卡片数量
      
      add+=U1SendByte(0xff&(Pt.KpRecT/60));//记录间隔
      add+=U1SendByte(0xff&((Pt.KpRecT/60)>>8));
      
      add+=U1SendByte(0xff&(Pt.KpTout/60));//超时时间
      add+=U1SendByte(0xff&((Pt.KpTout/60)>>8));
      
      for(j=0;j<18;j++)//预留18
        add+=U1SendByte(0x00);
      
      //记录容量(4)
      add+=U1SendByte(0xff&StateC.RecordMaxNum);
      add+=U1SendByte(0xff&(StateC.RecordMaxNum>>8));
      add+=U1SendByte(0xff&(StateC.RecordMaxNum>>16));
      add+=U1SendByte(0xff&(StateC.RecordMaxNum>>24));
      
      CmdSendEnd(add);
}

#endif



#if ((RF_REC_EN==1)||(RF_RP_EN==1)||(RF_KP_EN==1))
//设置频率
void SetFreq(void)
{
      u8 add=0;
      add=CmdSendStart(CMD_SET_FREQ,3);
      add+=U1SendByte(0x01);
      CmdSendEnd(add);
  
  Pt.Fz=Uart_1.RxBuf[5];
  if(Pt.Fz>9)
    return;
  SetRfFzToEE();
  GetRfFzFromEE();//验证是否正确
  
  #if RF_U1_EN==1
     delay_ms(10);
     U1_TO_RF;//先切换到RF,如果正常读取后，自动回到PC, 如果超时，自动回到PC
     //Pt.PCpt=3;
      delay_ms(10);
  #endif
  
  
  RfSetF(Pt.Fz);//设置频率
  
  WDT_CLR;
  delay_ms(100);
  WDT_CLR;
  RF_IN_REC;//设置完成后，始终处于接收状态
  
  //设置频率后切换到PC，与PC通讯时用9600
  #if ((RF_U1_EN==1)&&(RF_REC_EN==1))
    delay_ms(10);
    U1_TO_PC;
    Pt.PCpt=0;
    #if RF_PP_115200_EN==1
    RfU_Init(9600,1);
    #endif
  #endif
  

}

//配对，RF发送配对指令
void RfPair(void)
{
#if RF_RP_EN==0
  u8 add=0;
  add=CmdSendStart(CMD_RF_PAIR,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  
  
  #if RF_U1_EN==1
      delay_ms(10);
      U1_TO_RF;//先切换到RF,如果正常读取后，自动回到PC, 如果超时，自动回到PC
      //Pt.PCpt=3;
      #if RF_PP_115200_EN==1
      RfU_Init(115200,1);
      #endif
      
      delay_ms(10);
  #endif
   
     
  SendSnToSenior();//Rf发送同步指令
  delay_ms(100);
  WDT_CLR;
  delay_ms(100);
  WDT_CLR;
  SendSnToSenior();//Rf发送同步指令
  
  //配对后面不需卡片返回数据，故一直9600，切换到PC
  #if RF_U1_EN==1
      delay_ms(10);
      U1_TO_PC;
      Pt.PCpt=0;
       
      delay_ms(10);
      #if RF_PP_115200_EN==1
      RfU_Init(9600,1);
      #endif
  #endif
  
#endif
}
#endif


void ReadCurve(void)
{
/*
  #if GPRS_TC_EN==1//zz
            //测试，记录立满
            PcBsq.RecPt=0;	  
            PcBsq.Rec16Num=PcBsq.RecCap;
            PCBSQ_RecNumPt_ToEE();
            
            //------------------------------------------------------------------------------------------------

            Pt.GprsDueOut=PcBsq.RecCap;
            GprsDueOutToEE();
            Pt.GprsRstErrPt=0;

            Pt.PrintDueout=PcBsq.RecCap;		  
            PrintDueOutToEE();

   #endif
*/
  
  
  
#if RF_REC_EN==1
  //转发指令到无线卡片
  
  #if RF_U1_EN==1

      U1_TO_RF;//先切换到RF,如果正常读取后，自动回到PC, 如果超时，自动回到PC
      Pt.PCpt=3;

      #if RF_PP_115200_EN==1
      RfU_Init(115200,1);
      #endif
      
      delay_ms(10);
  #endif
  
     
  RfU_SendBuf(Uart_1.RxBuf,Uart_1.RecLen);
  
#else
  StructCurve Curve;
  u8 add=0;
  u8 ChNo;
  u16 j;
  
  ChNo=Uart_1.RxBuf[5]-1;
  
  add=CmdSendStart(READ_CURVE,48);
  
  CurveFlashToBuf(&Curve,ChNo);
  add+=U1SendByte(ChNo+1);
  for(j=0;j<15;j++)
  {
    add+=U1SendByte(0xff&Curve.Point[j]);
    add+=U1SendByte(0xff&(Curve.Point[j]>>8));
  }
  for(j=0;j<14;j++)
  {
    add+=U1SendByte(Curve.Space[j]);
  }
  add+=U1SendByte(Curve.PointNum);
  CmdSendEnd(add);
  
#endif

}
void SetCurve(void)
{
#if RF_REC_EN==1
  //转发指令到无线卡片
  
  #if RF_U1_EN==1

      U1_TO_RF;//先切换到RF,如果正常读取后，自动回到PC, 如果超时，自动回到PC
      Pt.PCpt=3;

      #if RF_PP_115200_EN==1
      RfU_Init(115200,1);
      #endif
      
      delay_ms(10);
  #endif
     
  RfU_SendBuf(Uart_1.RxBuf,Uart_1.RecLen);
  
  //后面等待RF 中断数据进来,故一直在RF
  
  
#else
  
  u8 add=0;
  StructCurve Curve;
  u16 i,j;
  u8 ChNo;
  
  ChNo=Uart_1.RxBuf[5]-1;
  i=0;
  for(j=0;j<15;j++)
  {
    Curve.Point[j]=Uart_1.RxBuf[6+i+1]<<8;
    Curve.Point[j]|=Uart_1.RxBuf[6+i];
    i=i+2;
  }
  for(j=0;j<14;j++)
    Curve.Space[j]=Uart_1.RxBuf[36+j];
  Curve.PointNum=Uart_1.RxBuf[50];
  CurveBufToFlash(&Curve,ChNo);
  
  add=CmdSendStart(SET_CURVE,3);

  add+=U1SendByte(0x01);
  CmdSendEnd(add);
#endif
}
void SetAddress(void)
{
  u8 add=0;

    if(StateB.Address!=Uart_1.RxBuf[1])//地址错误
    {
      return;
    }
    StateB.Address=Uart_1.RxBuf[5];  

    I2C_EE_BufferWrite(&Uart_1.RxBuf[5],ADDRESS_ADDR,1,0);//设置地址


  add=CmdSendStart(SET_ADDRESS,3);

  add+=U1SendByte(0x01);
  CmdSendEnd(add); 
}
void SetStateB(void)
{
    u8 add=0;
    u16 j;
    
    add=CmdSendStart(SET_STATEB,3);
    
    add+=U1SendByte(0x01);
    CmdSendEnd(add);
  
    StateB.StartLaterTime=U8_TO_U32(Uart_1.RxBuf[8],Uart_1.RxBuf[7],Uart_1.RxBuf[6],Uart_1.RxBuf[5]);
    StateB.RecordTime=U8_TO_U32(Uart_1.RxBuf[12],Uart_1.RxBuf[11],Uart_1.RxBuf[10],Uart_1.RxBuf[9]);
    StateB.StartMode=Uart_1.RxBuf[13];
    StateB.StopMode=Uart_1.RxBuf[14];
    

    #if LED_WORK_ENABLE==1//无LCD显示，一直同步模式
    StateB.LcdRefreshMode=LCD_SYNCHRONOUS;
    #else
    StateB.LcdRefreshMode=Uart_1.RxBuf[15];     
    #endif
    
    
    StateB.Address=Uart_1.RxBuf[16];


    I2C_EE_BufferWrite(&Uart_1.RxBuf[17],USER_NAME_ADDR,40,0);

    
    for(j=0;j<6;j++)StateB.LaterStartTime[j]=Uart_1.RxBuf[77+j];
    for(j=0;j<6;j++)StateB.LaterStopTime[j]=Uart_1.RxBuf[83+j];

  
    StateBToEE();
    
    LoggerStop();
    if(StateB.StartMode==STOP_MODE_LATER)
      StateA2.StopCause=OFF_LATER_TIME_START;
    else if(StateB.StartMode==START_MODE_HAND)
      StateA2.StopCause=OFF_HAND;
    else
      StateA2.StopCause=OFF_PC;
  
    StateA2ToEE();

    
    Pt.RecordPt=0;
    Pt.RealSamplePt=0;
    
    LcdAlarmEn();
    LcdMode();
}
void ReadInvInfor(void)
{
  #if SMS_INV_ENABLE==1
  u8 add=0;
  
  GsmInvRead();
  
  add=CmdSendStart(READ_INV_TIME,10);

  add+=U1SendByte(0xff&Sms.InvTimePt);
  add+=U1SendByte(0xff&(Sms.InvTimePt>>8));
  add+=U1SendByte(0xff&(Sms.InvTimePt>>16));
  add+=U1SendByte(0xff&(Sms.InvTimePt>>24));
  add+=U1SendByte(0xff&Sms.InvPointPt);
  add+=U1SendByte(0xff&(Sms.InvPointPt>>8));
  add+=U1SendByte(0xff&(Sms.InvPointPt>>16));
  add+=U1SendByte(0xff&(Sms.InvPointPt>>24));
  CmdSendEnd(add);
  #endif
}
/*
根据电池电压推算电池使用剩余时间,不同的型号计算方法不一样
可充电锂电池总量按600mAh计算
1. SMS_INV_ENABLE模式：Gsm模块休眠时，电流约0.45mA

  
  模块直接断电时，模块0功耗
  //----------------------------------------------
  发送一次以100mA,(1/60)h,发送间隔t 计算
  1小时短信功耗: W1=  100mA *(1/60)h * (3600/t)=(6000/t)mAh

  记录间隔k
  1小时基本功耗：0.05mAh
  1小时采样功耗：0.1mA * (0.02/3600)h * (3600/2)=0.001
  1小时记录功耗：1mA*(0.1/3600)h*(3600/k)=0.1/k
  1小时参数存储功耗：1mA*(0.1/3600)h*(3600/300)=0.1/300=0.00034
  W2= 0.1/k + 0.051

  1小时 总W=W1+W2

  电池电量：C= 500mAh 或 1000mAh （到3.4v算用完）
  最大工作时间(时) ：T= (C/W) 时
  
  420      T
  340      0
  x        y

  (y-0)/(T-0)=(x-340)/(420-340)
  y=((x-340)/80)*T

*/
void ReadBattTime(void)
{
  #if SMS_INV_ENABLE==1
  float yy;
  u32 cc;
  float T;
  
  if(Sms.InvTimePt==0)
    T=0.1/StateB.RecordTime + 0.051;
  else
    T=6000.0/Sms.InvTimePt+0.1/StateB.RecordTime + 0.051;
  T=1000.0/T;//最大工作时间
  
  StateC.BatV=380;
  if(StateC.BatV>340)
  {
    yy=(float)StateC.BatV;
    yy=((yy-340)/80)*T;
    cc=(u32)yy;
    cc=cc*3600;//转换成秒
  }
  else
  {
    cc=0;
  }
  
  u8 add=0;
  add=CmdSendStart(READ_BATT_TIME,6);

  add+=U1SendByte(0xff&cc);
  add+=U1SendByte(0xff&(cc>>8));
  add+=U1SendByte(0xff&(cc>>16));
  add+=U1SendByte(0xff&(cc>>24));
  CmdSendEnd(add);
  #endif
}


void SetInvInfor(void)
{
  #if SMS_INV_ENABLE==1
  u8 add=0;
  u32 buf32[2];
  
  buf32[0]=U8_TO_U32(Uart_1.RxBuf[8],Uart_1.RxBuf[7],Uart_1.RxBuf[6],Uart_1.RxBuf[5]);
  buf32[1]=U8_TO_U32(Uart_1.RxBuf[12],Uart_1.RxBuf[11],Uart_1.RxBuf[10],Uart_1.RxBuf[9]);
  GsmInvSet(buf32);
  
  add=CmdSendStart(SET_INV_TIME,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
  #endif
}

void SetGsmMontior(void)
{
  if(Uart_1.RxBuf[5]==1)
  {
    Flag.SeeComing=1;
    
    #if GPS_MOUSE_EN==1
    StateC.SeeBDSing=1;
    #endif
    
    #if GSM_TXT_EN==1
    U1SendStrPC("\r\nMonitor start...link to exit\r\n");
    #else
    U1SendStrPC("\r\n监测打开，断电或连接时关闭\r\n");
    #endif
    
    //EM310_Rst();
  }
  else
  {
    Flag.SeeComing=1;
    U1SendStrPC("\r\n提示：监测关闭\r\n");
    Flag.SeeComing=0;
    
    #if GPS_MOUSE_EN==1
    StateC.SeeBDSing=0;
    #endif
  }
}



void ReadDownloadInfor(void)
{
  u8 add=0;

  #if SAVE_MEMORY==SAVE_EEPROM

  Download.CurrentNum=StateA1.RecordNum;//保存下载时已记录条数

  Download.DownloadNum=StateA1.RecordNum;//下载时已记录次数
  
  #if WIRELESS_ENABLE==1
  if(Uart_0.FlagFinish==1)//无线通讯
  {
    Download.BagMaxRecordNum=(u16)RF_LOAD_BAG_MAX_SIZE/StateC.RecordBytes;//每包最大条数
  }
  else//有线通讯
  {
    Download.BagMaxRecordNum=(u16)DOWNLOAD_BAG_MAX_BYTES/StateC.RecordBytes;//每包最大条数
  }
  #else
  Download.BagMaxRecordNum=(u16)DOWNLOAD_BAG_MAX_BYTES/StateC.RecordBytes;//每包最大条数
  #endif
  
  if(Download.DownloadNum<StateC.RecordMaxNum)//记录未满
  {
    Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;//总包数
    if(Download.DownloadNum%Download.BagMaxRecordNum!=0)
      Download.BagNum++;
  }
  else//记录已满
  {
    Download.BagNum=StateC.RecordMaxNum/Download.BagMaxRecordNum;//总包数
    if(StateC.RecordMaxNum%Download.BagMaxRecordNum!=0)
      Download.BagNum++;
    Download.DownloadNum=StateC.RecordMaxNum;//下载总条数
  }
  #elif SAVE_MEMORY==SAVE_SD
  Download.DownloadNum=StateA1.RecordNum;//下载总条数
  Download.BagMaxRecordNum=StateC.PageMaxRecordNum;
  if(Download.DownloadNum<StateC.RecordMaxNum)//记录未满
  {
    Download.BagNum=Download.DownloadNum/StateC.PageMaxRecordNum;//总包数
    if(Download.DownloadNum%StateC.PageMaxRecordNum!=0)
    {
      Download.BagNum++;
    }
  }
  else//记录已满
  {
    Download.CurrentBufPt=StateA1.CurrentBufPt;
    if(Download.CurrentBufPt==0)//刚好记满，数据全部从FF取
    {
      Download.BagNum=SD_PAGE_MAX;//总包数
    }
    else
    {
      //溢出后，第一包数据和最后一包数据需特殊处理
      Download.BagNum=SD_PAGE_MAX+1;//总包数
    }
    Download.DownloadNum=StateC.RecordMaxNum;//总条数
  }
  #elif SAVE_MEMORY==SAVE_FLASH
  Download.DownloadNum=StateA1.RecordNum;//下载总条数
  Download.BagMaxRecordNum=StateC.PageMaxRecordNum;
  if(Download.DownloadNum<StateC.RecordMaxNum)//记录未满
  {
    Download.BagNum=Download.DownloadNum/StateC.PageMaxRecordNum;//总包数
    if(Download.DownloadNum%StateC.PageMaxRecordNum!=0)
    {
      Download.BagNum++;
    }
  }
  else//记录已满
  {
    Download.CurrentBufPt=StateA1.CurrentBufPt;
    if(Download.CurrentBufPt==0)//刚好记满，数据全部从FF取
    {
      Download.BagNum=FLASH_PAGE_COUNT;//总包数
    }
    else
    {
      //溢出后，第一包数据和最后一包数据需特殊处理
      Download.BagNum=FLASH_PAGE_COUNT+1;//总包数
    }
    Download.DownloadNum=StateC.RecordMaxNum;//总条数
  }
  #endif
  
  add=CmdSendStart(READ_DOWNLOAD_INFOR,15);
  
  add+=U1SendByte(0xff&Download.BagNum);//下载总包数
  add+=U1SendByte(0xff&(Download.BagNum>>8));
  add+=U1SendByte(0xff&Download.DownloadNum);//下载总条数
  add+=U1SendByte(0xff&(Download.DownloadNum>>8));
  add+=U1SendByte(0xff&(Download.DownloadNum>>16));
  add+=U1SendByte(0xff&(Download.DownloadNum>>24));
  add+=U1SendByte(StateC.RecordBytes);//每条字节数
  add+=U1SendByte(0xff&Download.BagMaxRecordNum);//满包时最大条数
  add+=U1SendByte(0xff&(Download.BagMaxRecordNum>>8));
  add+=U1SendByte(0xff&StateA1.RecordNum);//下载时已记录次数
  add+=U1SendByte(0xff&(StateA1.RecordNum>>8));
  add+=U1SendByte(0xff&(StateA1.RecordNum>>16));
  add+=U1SendByte(0xff&(StateA1.RecordNum>>24));
  CmdSendEnd(add);
 
  #if SAVE_MEMORY==SAVE_FLASH//3.3
  if(Download.DownloadNum==StateC.RecordMaxNum)//记录已满
  {
    if(Download.CurrentBufPt!=0)//先进先出，有缓冲
    {
      I2C_EE_BufferRead(Uart_1.RxBuf,LAST_PAGE_BUF_ADDR,Download.CurrentBufPt,0);
      I2C_EE_BufferWrite(Uart_1.RxBuf,LAST_PAGE_TMP_ADDR,Download.CurrentBufPt,0);//缓冲数据临时存放
    }
  }
  #endif
}

void ReadDownloadBag(void)
{
  u8 add=0;
   Download.BagPt=256*Uart_1.RxBuf[6]+Uart_1.RxBuf[5];//下载包序号
 
  if(Download.BagPt+1>Download.BagNum)return;//包序号错误
  

    if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
    {
      Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
    }
    else
    {
      Download.BagRecordNum=Download.BagMaxRecordNum;//满包
    }
    Download.BagDataBytes=Download.BagRecordNum*StateC.RecordBytes;
   
    add=CmdSendStart(READ_DOWNLOAD_BAG,Download.BagDataBytes+4);
    
    add+=U1SendByte(0xff&Download.BagPt);//包序号
    add+=U1SendByte(0xff&(Download.BagPt>>8));
    
    u8 QuHao;//当前存储片区号
    u32 EE_32Pt;
  
    EE_32Pt=0;
    if(Download.CurrentNum>=StateC.RecordMaxNum)//已记满
    {
      EE_32Pt+=(Download.CurrentNum%StateC.RecordMaxNum)*StateC.RecordBytes;//记满时的存储偏量
    }

    EE_32Pt+=(u32)Download.BagPt*(Download.BagMaxRecordNum*StateC.RecordBytes);
    EE_32Pt=EE_32Pt%(StateC.RecordMaxNum*StateC.RecordBytes);
    if(EE_32Pt<=StateC.E1_MaxMem)//第一片存储尾地址
    {
      QuHao=0;
      if(EE_32Pt+Download.BagDataBytes>StateC.E1_MaxMem)
      {
        add+=i2c_ee_read_to_pc(EE_32Pt,StateC.E1_MaxMem-EE_32Pt,QuHao);
        
        if(I2cSeekEE()>1)//片数大于1
        {
          QuHao++;
        }
        add+=i2c_ee_read_to_pc(0,EE_32Pt+Download.BagDataBytes-StateC.E1_MaxMem,QuHao);
      }
      else
      {
        add+=i2c_ee_read_to_pc(EE_32Pt,Download.BagDataBytes,QuHao);
      }
    }
    else//其他片
    {
      QuHao=1+(EE_32Pt-StateC.E1_MaxMem)/StateC.E2_MaxMem;
      EE_32Pt=(EE_32Pt-StateC.E1_MaxMem)%StateC.E2_MaxMem;
      
      if(EE_32Pt+Download.BagDataBytes>StateC.E2_MaxMem)//其他片存储尾地址
      {
        add+=i2c_ee_read_to_pc(EE_32Pt,StateC.E2_MaxMem-EE_32Pt,QuHao);
        
        QuHao++;
        u8 EE_Num=I2cSeekEE();
        QuHao=QuHao%EE_Num;
        add+=i2c_ee_read_to_pc(0,EE_32Pt+Download.BagDataBytes-StateC.E2_MaxMem,QuHao);
      }
      else
      {
        add+=i2c_ee_read_to_pc(EE_32Pt,Download.BagDataBytes,QuHao);
      }
    }

  CmdSendEnd(add);
  
  
  if(Download.BagPt+1>=Download.BagNum)//最后一包
  {
    Download.LastBagPt=2;
  }
  else
  {
    Download.LastBagPt=0;
  }

}
void SetRtc(void)
{
  u16 j;
  u8 add=0;
  u8* xx;
  xx=&Rtc.Year;
  for(j=0;j<6;j++)
    *xx++=Uart_1.RxBuf[5+j];
  RtcSetTime();
  RtcBcdToD10(&Rtc);
  Pt.RtcPt=1;
  
  add=CmdSendStart(SET_RTC,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
}
void SetControl(void)
{
  u8 add=0;
 
  if(Uart_1.RxBuf[5]>8)return;
  
  alarm_buf_to_flash(&Uart_1.RxBuf[6],Uart_1.RxBuf[5]-1,2);

  add=CmdSendStart(SET_CONTROL,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);

  RyFirst();//初始状态
  
  LoggerAlarmAllSeek();//立即查询
  LcdValue();
}
void ReadControl(void)//读取控制参数...
{
  u8 add=0;
  u16 j;
  
  add=CmdSendStart(READ_CONTROL,3+5*CH_NUM);
  
  add+=U1SendByte(CH_NUM);
  for(j=0;j<CH_NUM;j++)
  {
    add+=U1SendByte(*((char *)CH1_K_L_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_K_L_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_K_H_ADDR+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_K_H_ADDR+1+j*ONE_CH_CONFIG_BYTES));
    add+=U1SendByte(*((char *)CH1_K_E_ADDR+j*ONE_CH_CONFIG_BYTES));
  }
  
  CmdSendEnd(add);
}

#if LCD08_SD_SHIFT_EN==1
void CmdSetSdShift(void)
{
  u8 add=0;
  
  SetSdShiftEnToEE(Uart_1.RxBuf[5]);
  
  add=CmdSendStart(SET_SD_SHIFT_EN,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
}
void CmdReadSdShift(void)
{
  u8 add=0;
  
  GetSdShiftEnFromEE();
  add=CmdSendStart(READ_SD_SHIFT_EN,3);
  add+=U1SendByte(Flag.LCD08_Sd_en);
  CmdSendEnd(add);
}
#endif

#if PRINT_REAL_EN==1
void CmdSetPrintXX(void)
{
  u8 add=0;
  u16 j;
  for(j=0;j<5;j++)
  {
    StateC.PrintXB[j]=U8_TO_U16(Uart_1.RxBuf[j*2+6],Uart_1.RxBuf[j*2+5]);//虚线点位
  }
  SetPrintXX();
  
  add=CmdSendStart(SET_PRINT_XX,3);
  add+=U1SendByte(0x01);
  CmdSendEnd(add);
}
void CmdReadPrintXX(void)
{
  u8 add=0;
  u16 j;
  
  GetPrintXX();
  
  add=CmdSendStart(READ_PRINT_XX,12);
  for(j=0;j<5;j++)
  {
    add+=U1SendByte(0xff&StateC.PrintXB[j]);
    add+=U1SendByte(0xff&(StateC.PrintXB[j]>>8));
  }
  CmdSendEnd(add);
}
#endif



#if ((GPRS_TC_EN==1)||(BSQ_REC_AA_EN==1))
//EE储存里的每条记录要转换为通讯时的每条记录 GPRS_WIFI_SIZE 
u8 Buf_To_PC(u8* buf)
{
  u8 add;
  u16 j;
  
  add=0;
  
  #if ((GPS_MOUSE_EN==1)||(LBS_LAC_EN==1)||(LBS_JWD_EN==1)) ////每条记录占用字节数， RTC(4)+BAT(1)+GPS(8)+2*CH_NUM
    for(j=0;j<GPRS_WIFI_SIZE;j++)//此时，每条记录，储存数与通讯数是相等的
    {
      add+=U1SendByte(buf[j]);
    }
  #else//不相当，把储存记录，转换为 通讯格式  //每条记录占用字节数， RTC(4)+BAT(1)+2*CH_NUM
    for(j=0;j<5;j++)
    {
      add+=U1SendByte(buf[j]);
    }
    
    for(j=0;j<8;j++)
    {
      add+=U1SendByte(0x00);//补回成通讯格式
    }
    
    for(j=0;j<2*CH_NUM;j++)
    {
      add+=U1SendByte(buf[5+j]);
    }
    
    
  #endif
  return add;
}


void Read_GPRS_DATA_Infor(void)
{
  u8 add=0;

  //当前储存状态, 必须先赋值，否则Gprs_SF_Send_Data 会出错
  Pt.GprsThisRec16Num=PcBsq.Rec16Num;
  Pt.GprsThisRecPt=PcBsq.RecPt;
  
  //Download.DownloadNum=Pt.PrintDueout;//下载时已记录次数
  Download.DownloadNum=PcBsq.Rec16Num;//下载时已记录次数zz
    
  //每包最大条数
  Download.BagMaxRecordNum=DOWNLOAD_BAG_MAX_BYTES/GPRS_WIFI_SIZE;
  
  //总包数
  Download.BagNum=Download.DownloadNum/Download.BagMaxRecordNum;
  if((Download.DownloadNum>0)&&(Download.BagNum==0))
    Download.BagNum++;
  else if(Download.DownloadNum%Download.BagMaxRecordNum!=0)
    Download.BagNum++;

  add=CmdSendStart(READ_DOWNLOAD_INFOR,15);
  
  add+=U1SendByte(0xff&Download.BagNum);//下载总包数
  add+=U1SendByte(0xff&(Download.BagNum>>8));
  
  add+=U1SendByte(0xff&Download.DownloadNum);//下载总条数
  add+=U1SendByte(0xff&(Download.DownloadNum>>8));
  add+=U1SendByte(0xff&(Download.DownloadNum>>16));
  add+=U1SendByte(0xff&(Download.DownloadNum>>24));
  
  add+=U1SendByte(GPRS_WIFI_SIZE);//每条字节数
  
  add+=U1SendByte(0xff&Download.BagMaxRecordNum);//满包时最大条数
  add+=U1SendByte(0xff&(Download.BagMaxRecordNum>>8));
  
  add+=U1SendByte(0xff&Download.DownloadNum);//下载时已记录次数
  add+=U1SendByte(0xff&(Download.DownloadNum>>8));
  add+=U1SendByte(0xff&(Download.DownloadNum>>16));
  add+=U1SendByte(0xff&(Download.DownloadNum>>24));
  
  CmdSendEnd(add);
  
  if(Download.DownloadNum>0)
    Download.Downloading=1;//进入下载状态
  
}



void Read_GPRS_DATA_Bag(void)
{
    u8 add=0;
    Download.BagPt=256*Uart_1.RxBuf[6]+Uart_1.RxBuf[5];//下载包序号
 
    if(Download.BagPt+1>Download.BagNum)return;//包序号错误
  

    //该包条数
    if((Download.BagPt+1==Download.BagNum)&&(Download.DownloadNum%Download.BagMaxRecordNum!=0))//最后一包并且不足一包
    {
      Download.BagRecordNum=Download.DownloadNum%Download.BagMaxRecordNum;
      Download.LastBagPt=3;
    }
    else
    {
      Download.BagRecordNum=Download.BagMaxRecordNum;//满包
      Download.LastBagPt=0;
    }
    
    //该包字节数
    Download.BagDataBytes=Download.BagRecordNum*GPRS_WIFI_SIZE;
    
   
    //开始发送数据头
    add=CmdSendStart(READ_DOWNLOAD_BAG,Download.BagDataBytes+4);
    
    
    add+=U1SendByte(0xff&Download.BagPt);//包序号
    add+=U1SendByte(0xff&(Download.BagPt>>8));
    
    
    u16 pt;
    pt=Download.DownloadNum-Download.BagPt*Download.BagMaxRecordNum;
    
    add+=Gprs_SF_Send_Data(pt,Download.BagRecordNum,3);
            

    CmdSendEnd(add);
  
}

#endif

