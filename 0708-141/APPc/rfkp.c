#include "_config.h"

#if RF_KP_EN==1


//设置无线传感器 目标SN
void SetKpSnToEE(void)
{
   I2C_EE_BufferWrite(Pt.KpSn,RF_SN_ADDR,2,0);//保存标志
}
//读取无线传感器 目的SN
void GetKpSnFromEE(void)
{
  I2C_EE_BufferRead(Pt.KpSn,RF_SN_ADDR,2,0);
}

//固定8字节协议: A0(1)  ADDR(1)  SN(2)  TH(3)   CRC(1) 
        
// TH(3): 温湿度共用3个字节, 即24位，温度占14位（23-10），湿度占10位（9-0）
// 温度14位最高位1为负，0为正。其余13位数值.  范围（-8191，8191）
// 湿度10位，范围(0-3FF)， 即(0,1023)

//带电量指示----------------------------------------
//B0(1)  ADDR(1)  SN_BAT(2)  TH(3)  ADD(1)
//其他与A0指令一样，区别：
//1.  A0变成了B0
//2.  SN(2) 分成了2部分 SN_BAT(2)
//    AAAB   AAA为原先SN(2)的前12位， B为电量(0-9)
/*
【3】 支持2个温度  170818  需要主机升级
B0(1)  ADR(1)  SN_BAT(2)  TH(3)  ADD(1)
TH(3) ：分为2个通道，前12位为通道1，后12位为通道2
       最高位1为负，正为负。其余11位数值，范围 7FF( -2047, 2047)
ADR(1): 最高位1为 2温度卡片协议，最高位0为单温度或温湿度卡片协议。
*/
void SendRf_Kp8(u8 addr)
{
        u8 TT;//2路温度卡片兼容
        u8 bb;
        u8 add=0;
        u16 i,j,tt,hh;
        u32 th;
        u8 buf[8];
        
        
        TT=addr;
        #if JLY_MODEL==_2_NTC
        TT|=0x80|addr;
        #endif
        
        bb=GetBatt();//电池电量
        
        
        i=0;
        add+=buf[i++]=0xB0;
        add+=buf[i++]=TT;
        add+=buf[i++]=Pt.KpSn[0];
        bb=(Pt.KpSn[1]&0xf0)|((bb/10)&0x0f);
        add+=buf[i++]=bb;
            
     #if JLY_MODEL==_2_NTC

        th=0;
        for(j=0;j<CH_NUM;j++)
        {
            if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
            {
               tt=0xfff;//12位
            }
            else
            {
                tt=0x7ff&StateC.Value[j];//数值
                if(0x01&(StateC.FuhaoBit>>j))//负数
                {
                   tt|=0x800;
                }
            }
            
            if(j==0)
              th=tt;
            else if(j==1)
              th=(th<<12)|tt;
        }
        
        
     #else


        j=0;//温度
        if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
        {
           tt=0x3fff;
        }
        else
        {
            tt=0x1fff&StateC.Value[j];//数值
            if(0x01&(StateC.FuhaoBit>>j))//负数
            {
               tt|=0x2000;
            }
        }
                
        //湿度
        if(CH_NUM==2)
        {
            j=1;//温度
            if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
            {
               hh=0x3ff;
            }
            else
            {
               hh=0x3ff&StateC.Value[j];
            }
        }
        else
        {
            hh=0x3ff;
        }
                
         //温度和湿度合成3个字节
         th=tt;
         th=(th<<10)|hh;
      #endif
                
         
         
         add+=buf[i++]=0xff&(th>>16);
         add+=buf[i++]=0xff&(th>>8);
         add+=buf[i++]=0xff&(th>>0);
                
         //校验和
         buf[i++]=add;
         
         //发送
         U0Init(115200,1);
         
         VDC33_ON;
         RF_IN_REC;
         delay_ms(55);//开电源后，必须延时  至少50
         
         delay_ms(6);
         
         RfU_SendBuf(buf,i);
         
         //RF_IN_SLP;
         delay_us(500);//必须延时
         U0Init(9600,0);
         
         //关闭电源
         delay_ms(10);//必须延时至少6
         VDC33_OFF;
         RF_IN_REC;

}

void RfKp1Sec(void)
{
    if(Pt.snpt>0)
    {
      Pt.snpt--;
      if(Pt.snpt==0)//超时时间到自动退出HHHH状态
      {
          RF_IN_SLP;
          if(StateA2.WorkState!=WORK_STATE_RECORDING)
          {
             StateA2.WorkState=WORK_STATE_RECORDING;//开机
          }
      }
      
      
    }
}



//6ms发送
u8 RfKpHave(void)
{
    if(++Pt.RfInvPt>=RF_SEN_TIME)
    {
        //随机数
        Pt.Random=TAR;
        Pt.Random+=TBR;
        Pt.Random+=StateC.Adc[3];
        Pt.Random+=StateC.Adc[4];
        Pt.Random+=StateC.Adc[5];
        Pt.Random+=StateC.Adc[6];
        Pt.Random=Pt.Random%RF_RADOM_N;
   
        Pt.RfInvPt=(u16)Pt.Random;//zz  从（0-1999）或（0-99）重新开始
      
        return 1;
    }

    
    return 0;
  
}


void RfKpSendDeel(void)
{
  if(Pt.RfTing==1)
  {
      Pt.RfTing=0;
    
      if(StateA2.WorkState==WORK_STATE_RECORDING)
      {
          
          SendRf_Kp8(StateB.Address);
          
          Tshan();
      }
           

  }
}


#endif

