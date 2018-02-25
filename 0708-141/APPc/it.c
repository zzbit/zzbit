#include "_config.h"


//�ж�...........................................................................
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
   
   WDT_CLR;

   #if ((RF_RP_EN==1)||(RF_MULTI_EN==1))
   
       if(Pt.RpOff==1)//�ر�ʱ��������
         return;
   #endif
       
   
   #if (U0U1_ENABLE==1)
   if(Flag.U0U1ing==1)//U0RX->U1TX
   {
      ComSendH();
      TB4_START;//��ʱ��ִ��ComSendL()
      
      UCA1TXBUF = UCA0RXBUF;
      while (!(UC1IFG&UCA1TXIFG));//USCI_A1 TX buffer ready?
      UC1IFG&=~UCA1TXIFG;
      
      #if ((GSM_ENABLE==1)||(RF_REC_EN==1))
        return;
      #endif
      //return;  ���߽���W95ʱ���Կ���ִ������AAָ��
   }
   #endif
   
   
   #if (RP_SEND_EN==1)
       if(Pt.RpOff==1)//�ر�ʱ��������
         return;
   #endif
  
  NOP;
  #if ((GSM_ENABLE==1)||(SEN_UART_EN==1))

  if(Uart_0.RecPt>=U0MAX)
  {
    TIME_U0_STOP;
    Uart_0.RecPt=0;
  }
  else
  {
    u16 xx=Uart_0.RecPt;
    Uart_0.RxBuf[xx]=UCA0RXBUF;
    Uart_0.RecPt++;
    TIME_U0_START;
  }
  IFG2&=~UCA0RXIFG;//���жϱ�־ zz
  return;
  
  #elif (SEN_U0_PM25_EN==1)//�����ٶȺܿ죬���Խ��շ�����ͬ �� SEN_UART_EN
    if(Uart_0.FlagHeadOk==0)
    {
        if(UCA0RXBUF==0xaa)//ͷ
        {
          Uart_0.FlagHeadOk=1;
          Uart_0.FlagFinish=0;
          Uart_0.RecPt=0;
          
          Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
          TIME_U0_START;
          
          Uart_0.AddCrc=0;
        }
        else//֡ͷ����
        {
          U0_RecErr();
        }
        return;
    }
  
    TIME_U0_CLR;
    Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;

    if(Uart_0.RecPt>7)
    {
        U0_RecErr();
        return;
    }
    
    if(Uart_0.RecPt<6)//У���ֻ���� 1-4 ��4���ֽ�
    {
      Uart_0.AddCrc+=UCA0RXBUF;
    }
    
    if(Uart_0.RecPt==7)
    {
        if((Uart_0.RxBuf[Uart_0.RecPt-1]==0xff)&&(Uart_0.AddCrc==Uart_0.RxBuf[Uart_0.RecPt-2]))//β
        {
            Uart_0.FlagFinish=1;
            Uart_0.RecLen=Uart_0.RecPt;
            Uart_0.FlagHeadOk=0;
            
            //��õ���voutֵ (mv)
            StateC.sen_vv=U8_TO_U16(Uart_0.RxBuf[1],Uart_0.RxBuf[2]);
            
            float ft;
            ft=4.8828125*StateC.sen_vv;
            StateC.sen_vv=(u16)ft;
 
            StateC.sen_Pt=0;// ����ȷ���ݣ���0����������
            
            return;
    
        }
        else
        {
            U0_RecErr();
            return;
        }
    }
    
  #elif RF_KP_EN==1
    
    if(Uart_0.FlagHeadOk==0)
    {
        if(UCA0RXBUF==0xAF)//ͷ
        {
          Uart_0.FlagHeadOk=1;
          Uart_0.FlagFinish=0;
          Uart_0.RecPt=0;
          
          Uart_0.AddCrc=0;
          Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
          TIME_U0_START;
          
          
        }
        else//֡ͷ����
        {
          U0_RecErr();
        }
        return;
    }
    TIME_U0_CLR;
    Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
    
    if(Uart_0.RecPt>8)
    {
        U0_RecErr();
        return;
    }
    
    
    if(Uart_0.RecPt==8)
    {
              if(get_add(Uart_0.RxBuf,7)==Uart_0.RxBuf[7])
              {
                  Pt.KpSn[0]=Uart_0.RxBuf[1];
                  Pt.KpSn[1]=Uart_0.RxBuf[2];
                  
                  SetKpSnToEE();
                  GetKpSnFromEE();
                
                  if(CheckBuf(Pt.KpSn,&Uart_0.RxBuf[1],2))
                  {
                    //���óɹ�
                    Pt.snpt=0;
                    RF_IN_SLP;
                    StateA2.WorkState=WORK_STATE_RECORDING;//����
    
                    IE2 &=~ UCA0RXIE;//ͬ����ɺ󣬹رս���
                    
                    
                    Pt.RfInvPt=RF_SEN_TIME-500-Pt.Random;
                    
                  }
              }
              else
              {
                U0_RecErr();
              }
    }
            
    return;
    
    
  #elif (((RF_REC_EN==1)||(RF_RP_EN==1))&&(RF_U1_EN==0))
    
    #if RF_CC_EN==1
    
        if(Uart_0.FlagHeadOk==0)
        {
            
            
            #if RF_MULTI_EN==1
            if(((UCA0RXBUF>=0xA0)&&(UCA0RXBUF<0xAA))||((UCA0RXBUF>=0xB0)&&(UCA0RXBUF<0xBA))\
              ||((UCA0RXBUF>=0xC0)&&(UCA0RXBUF<0xCA)&&(StateB.Address<(0x0f&UCA0RXBUF))))//���ߴ�����A0��B0,�м����ߵ�ַCx, �͵�ַ�����գ�������
            #else
            if(((UCA0RXBUF>=0xA0)&&(UCA0RXBUF<0xAA))||((UCA0RXBUF>=0xB0)&&(UCA0RXBUF<0xBA)))
            #endif
            {
                Uart_0.FlagHeadOk=1;
                Uart_0.FlagFinish=0;
                Uart_0.RecPt=0;
                
                Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
                TIME_U0_START;
                
                
                #if RF_MULTI_EN==1
                if((UCA0RXBUF>=0xC0)&&(UCA0RXBUF<0xCA))
                {
                  Pt.RecRpBag=1;
                }
                else
                {
                  Pt.RecRpBag=0;
                }
                #endif
                
                Pt.FlagBB=0;
              
            }
            else if(UCA0RXBUF==0xBB)//��Ƭ������ָ��
            {
                Uart_0.FlagHeadOk=1;
                Uart_0.FlagFinish=0;
                Uart_0.RecPt=0;
                
                Uart_0.AddCrc=0;
                
                Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
                TIME_U0_START;
                
                Pt.FlagBB=1;
            }
            else//֡ͷ����
            {
              U0_RecErr();
            }
            return;
        }
    
        TIME_U0_CLR;
        Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
        
        //��Ƭ����zzָ�� AD���ߵ���
        if(Pt.FlagBB==1)
        {
              if(Uart_0.RecPt>U0MAX)
              {
                U0_RecErr();
                return;
              }
              //�жϵ�ַ
              /*
              if(Uart_0.RecPt==2)//�豸��ַ
              {
                if(Uart_0.RxBuf[1]!=StateB.Address&&Uart_0.RxBuf[1]!=0x00)//��ַ����
                {
                  U0_RecErr();
                }
                return;
              }*/
              //�ж��ֽ���
              if(Uart_0.RecPt==5)
              {
                 Uart_0.RecLen=Uart_0.RxBuf[4]<<8|Uart_0.RxBuf[3];//֡���ȣ�2��
                 Uart_0.RecLen+=5;//��Ҫ���յ������ܳ���
                 if(Uart_0.RecLen>U0MAX)//���ȴ���
                 {
                   U0_RecErr();
                 }
                 return;
              }
              if(Uart_0.RecPt>5&&Uart_0.RecPt==Uart_0.RecLen)//���һ���ֽ�
              {
                 if(Uart_0.RxBuf[Uart_0.RecLen-1]==0xDD)//֡β��ȷ
                 {
                   Uart_0.AddCrc=Uart_0.AddCrc-0xDD-Uart_0.RxBuf[Uart_0.RecLen-2];
                   if(Uart_0.RxBuf[Uart_0.RecLen-2]==Uart_0.AddCrc)
                   {
                        TIME_U0_STOP;
                        Uart_0.FlagHeadOk=0;
                        
                        Uart_0.FlagFinish=1;//�������,��¼�����Э��
                        
                        //������ɺ�ת����PC
                        //U0����RF��ֻ����JLY  BSQ  H1,��������GW
                        
                        ComSendH();
                        U1SendBuf(Uart_0.RxBuf,Uart_0.RecLen);
                        ComSendL();

                    }
                    else//���������
                    {
                      U0_RecErr();
                    }
                 }
                 else//֡β����
                 {
                   U0_RecErr();
                 }
                 return;
              }
              return;
        }
        
        
        #if RF_MULTI_EN==1
        if(Pt.RecRpBag==0)//A0 B0 ���߿�Ƭָ��
        {
        #endif
            if(Uart_0.RecPt>8)
            {
              U0_RecErr();
              return;
            }
        
            if(Uart_0.RecPt==8)
            {
               #if RF_RP_EN==1
               if(get_add(Uart_0.RxBuf,7)==Uart_0.RxBuf[7])
                 
               #else//���ջ��������Ͷ����������У�����ȷ��������ȷ��SN-CRC16��ȷ
               if((get_add(Uart_0.RxBuf,7)==Uart_0.RxBuf[7])&&
                  ((((Uart_0.RxBuf[0]&0xF0)==0xA0)&&(Pt.SnCRC16==U8_TO_U16(Uart_0.RxBuf[2],Uart_0.RxBuf[3])))
                ||(((Uart_0.RxBuf[0]&0xF0)==0xB0)&&((Pt.SnCRC16&0xFFF0)==(0xFFF0&(U8_TO_U16(Uart_0.RxBuf[2],Uart_0.RxBuf[3])))))))
               #endif
               {
                   TIME_U0_STOP;
                   Uart_0.RecLen=8;
                   
                    #if RF_REC_EN==1
                      Rf_Rec_Yq(Uart_0.RxBuf,Uart_0.RecLen); 
                      
                    #elif RF_RP_EN==1
                      Rf_Rec_Rp(1);//���յ����ݻ���
                      
                    #endif
                   
                   Uart_0.FlagHeadOk=0;//�������
               }
               else
               {
                 U0_RecErr();
               }
            }
            return;
            
        #if RF_MULTI_EN==1    
        }
        else//�м���������ݣ�Cn
        {
            if(Uart_0.RecPt>U0MAX)
            {
              U0_RecErr();
              return;
            }
            if(Uart_0.RecPt==4)
            {
              Uart_0.RecLen=(u16)6*Uart_0.RxBuf[3]+6;//֡���ȣ�2��
              if(Uart_0.RecLen>U0MAX)
              {
                U0_RecErr();
                return;
              }
              return;
            }
            
            if((Uart_0.RecPt>=10)&&(Uart_0.RecPt==Uart_0.RecLen))//���һ���ֽ�
            {
              //��Cnָ���ж�SN CRC\У�����ȷ
              if((get_add(Uart_0.RxBuf,Uart_0.RecLen-2)==Uart_0.RxBuf[Uart_0.RecLen-2])\
                &&(Pt.SnCRC16==U8_TO_U16(Uart_0.RxBuf[1],Uart_0.RxBuf[2])))
              {
                TIME_U0_STOP;
                
                RecRPData(Uart_0.RxBuf,Uart_0.RecLen);//��������ݴ���
                
                Uart_0.FlagHeadOk=0;//�������
              }
              else
              {
                U0_RecErr();
              }
            }
            
            return;
            
        }
        #endif

    
    #else
        
        if(Uart_0.FlagHeadOk==0)
        {
            if(UCA0RXBUF>=0xf0)//���ߴ�����F0,�����м���F1-FF
            {
              Uart_0.FlagHeadOk=1;
              Uart_0.FlagFinish=0;
              Uart_0.RecPt=0;
              
              Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
              TIME_U0_START;
            }
            else//֡ͷ����
            {
              U0_RecErr();
            }
            return;
        }
      
        TIME_U0_CLR;
        Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
        if(Uart_0.RecPt>RF_CMD_MAX)
        {
          U0_RecErr();
          return;
        }

        if(Uart_0.RecPt==3)//�ֽ���,����3��ʱ��Uart_0.RxBuf[2]�ոս���
        {
           Uart_0.RecLen=Uart_0.RxBuf[2]+5;//֡���ȣ�2��
           if(Uart_0.RecLen>RF_CMD_MAX)//���ȴ��� 
           {
             U0_RecErr();
           }
           return;
        }
        if((Uart_0.RecPt>17)&&(Uart_0.RecPt==Uart_0.RecLen))//���һ���ֽ�
        {
          #if RF_RP_EN==1
          if(GetCrc16(Uart_0.RxBuf,Uart_0.RecLen-2)==U8_TO_U16(Uart_0.RxBuf[Uart_0.RecLen-2],Uart_0.RxBuf[Uart_0.RecLen-1]))//CRCУ��
          #else
          //CRC��ȷ, ����SN��һ��
          if((GetCrc16(Uart_0.RxBuf,Uart_0.RecLen-2)==U8_TO_U16(Uart_0.RxBuf[Uart_0.RecLen-2],Uart_0.RxBuf[Uart_0.RecLen-1]))\
            &&(CheckBuf(Pt.Sn,&Uart_0.RxBuf[3],10)))
          #endif
          {
                //IE2 &=~ UCA0RXIE;//
                TIME_U0_STOP;
                
                #if RF_REC_EN==1
                
                  Rf_Rec_Yq(Uart_0.RxBuf,Uart_0.RecLen); 
                      
                #elif RF_RP_EN==1
                  
                  Rf_Rec_Rp(1);//���յ����ݻ���

                  
                #endif
                
                
                //LED_OFF;
                Uart_0.FlagHeadOk=0;//�������
               // IE2 |= UCA0RXIE;//���´��ж�  
          }
          else
          {
            U0_RecErr();
          }
          return;
        }
    #endif
  
  #elif WIRELESS_ENABLE==1
    if(Uart_0.FlagHeadOk==0)
    {
      if((UCA0RXBUF==0Xaa))
      {
        Flag.Modbus=0;
        
        Uart_0.FlagHeadOk=1;
        Uart_0.FlagFinish=0;
        Uart_0.RecPt=0;
        
        Uart_0.AddCrc=0;
        Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
        TIME_U0_CLR;
        TIME_U0_START;
      }
      else if(UCA0RXBUF==StateB.Address||UCA0RXBUF==0x00)//modbus��ַ��ȷ
      {
          Flag.Modbus=1;
          
          Uart_0.FlagHeadOk=1;
          Uart_0.FlagFinish=0;
          Uart_0.RecPt=0;
          
          Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
          TIME_U0_CLR;
          TIME_U0_START;
      }
      else//֡ͷ����
      {
        U0_RecErr();
      }
      return;
    }
    
    if(Flag.Modbus==1)//modbus��ʽ����
    {
      if(Uart_0.RecPt>=U0MAX)
      {
         U0_RecErr();
      }
      else
      {
        Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
        TIME_U0_CLR;
        TIME_U0_START;
      }
      return;
    }
    
    
    TIME_U0_CLR;
    Uart_0.AddCrc+=Uart_0.RxBuf[Uart_0.RecPt++]=UCA0RXBUF;
    if(Uart_0.RecPt>U0MAX)
    {
      U0_RecErr();
      return;
    }
    //�жϵ�ַ
    if(Uart_0.RecPt==2)//�豸��ַ
    {
      if(Uart_0.RxBuf[1]!=StateB.Address&&Uart_0.RxBuf[1]!=0x00)//��ַ����
      {
        U0_RecErr();
      }
      return;
    }
    //�ж��ֽ���
    if(Uart_0.RecPt==5)
    {
       Uart_0.RecLen=Uart_0.RxBuf[4]<<8|Uart_0.RxBuf[3];//֡���ȣ�2��
       Uart_0.RecLen+=5;//��Ҫ���յ������ܳ���
       if(Uart_0.RecLen>U0MAX)//���ȴ���
       {
         U0_RecErr();
       }
       return;
    }
    if(Uart_0.RecPt>5&&Uart_0.RecPt==Uart_0.RecLen)//���һ���ֽ�
    {
       if(Uart_0.RxBuf[Uart_0.RecLen-1]==0xcc)//֡β��ȷ
       {
         Uart_0.AddCrc=Uart_0.AddCrc-0xcc-Uart_0.RxBuf[Uart_0.RecLen-2];
         if(Uart_0.RxBuf[Uart_0.RecLen-2]==Uart_0.AddCrc)
         {
            TIME_U0_STOP;
            Uart_0.FlagHeadOk=0;
            
            Uart_0.FlagFinish=1;//�������,��¼�����Э��
            //LED_ON;
            
            //������ɺ�ת�Ƶ�U1����
            Uart_1.FlagFinish=1;
            Uart_1.RecLen=Uart_0.RecLen;
            for(u16 j=0;j<Uart_1.RecLen;j++)
              Uart_1.RxBuf[j]=Uart_0.RxBuf[j];
            
            __bic_SR_register_on_exit(LPM3_bits); 
          }
          else//���������
          {
            U0_RecErr();
          }
       }
       else//֡β����
       {
         U0_RecErr();
       }
       return;
    }
    return;
  
  #endif
}
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCI1RX_ISR(void)
{
   WDT_CLR;
   
   UC1IFG&=~UCA1RXIFG;//zz
  
#if WIFI_ENABLE==1
   
    if(Uart_1.RecPt>=U1MAX)
    {
          TIME_U1_STOP;
          Uart_1.RecPt=0;
    }
    else
    {
          Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
          TIME_U1_START;
    }
#else
    

  
#if RF_MULTI_EN==1

   #if U0U1_ENABLE==1
   if(Flag.U0U1ing==1)//U1RX->U0TX
   {
     U0SendByte(UCA1RXBUF);
     return;
   }
   #endif
  
    if(Uart_1.RecPt>=U1MAX)
    {
          TIME_U1_STOP;
          Uart_1.RecPt=0;
    }
    else
    {
          Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
          TIME_U1_START;
    }

  
#else
    
   #if U0U1_ENABLE==1
   if(Flag.U0U1ing==1)//U1RX->U0TX
   {
     U0SendByte(UCA1RXBUF);
     return;
   }
   #endif

   
   //������  �� ��¼��GSM-RF��CRC  ��GPRS-��TTL������
   #if ((MODBUS_ENABLE==1)\
         ||((RECORD_ENABLE==1)&&(RF_U1_EN==1))\
           ||(SEN_U1_EN==1))
   
    //�м�P ֻ��U0, ����U1
    #if (((RF_REC_EN==1)&&(RF_U1_EN==1))  ||  (SEN_U1_EN==1))//U1��RF������ �� TTL����������CO2
   
   
     #if SEN_U1_EN==1
     if(Pt.U1toPC==0)
     #else
     if((Pt.U1toPC==0)||(Pt.PCpt>0))//���յ��������ߴ�����,����տ�ƬBBָ��ʱ
     #endif
    
     {
        #if SEN_U1_EN==1//U1�ӵ���TTL������������CO2 TTL
      
          if(Uart_1.RecPt>=100)
          {
            TIME_U1_STOP;
            Uart_1.RecPt=0;
          }
          else
          {
            Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
            TIME_U1_START;
          }
          return;
      
      
        #elif RF_CC_EN==1//CCЭ��
        
            
            if(Uart_1.FlagHeadOk==0)
            {
                if(((UCA1RXBUF>=0xA0)&&(UCA1RXBUF<0xAA))||((UCA1RXBUF>=0xB0)&&(UCA1RXBUF<0xBA)))//���ߴ�����A0,�����м���A1-A9
                {
                  Uart_1.FlagHeadOk=1;
                  Uart_1.FlagFinish=0;
                  Uart_1.RecPt=0;
                  
                  Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                  TIME_U1_START;
                  
                  Pt.FlagBB=0;
                  
                }
                else if(UCA1RXBUF==0xBB)//��Ƭ������ָ��
                {
                    Uart_1.FlagHeadOk=1;
                    Uart_1.FlagFinish=0;
                    Uart_1.RecPt=0;
                    
                    Uart_1.AddCrc=0;
                    
                    Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                    TIME_U1_START;
                    
                    Pt.FlagBB=1;
                }
                
                else//֡ͷ����
                {
                  U1_RecErr();
                }
                return;
            }
        
            TIME_U1_CLR;
            Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
            
        //��Ƭ����zzָ�� AD���ߵ���
        if(Pt.FlagBB==1)
        {
              if(Uart_1.RecPt>U1MAX)
              {
                U1_RecErr();
                return;
              }
              //�жϵ�ַ
              /*
              if(Uart_1.RecPt==2)//�豸��ַ
              {
                if(Uart_1.RxBuf[1]!=StateB.Address&&Uart_1.RxBuf[1]!=0x00)//��ַ����
                {
                  U1_RecErr();
                }
                return;
              }*/
              //�ж��ֽ���
              if(Uart_1.RecPt==5)
              {
                 Uart_1.RecLen=Uart_1.RxBuf[4]<<8|Uart_1.RxBuf[3];//֡���ȣ�2��
                 Uart_1.RecLen+=5;//��Ҫ���յ������ܳ���
                 if(Uart_1.RecLen>U1MAX)//���ȴ���
                 {
                   U1_RecErr();
                 }
                 return;
              }
              if(Uart_1.RecPt>5&&Uart_1.RecPt==Uart_1.RecLen)//���һ���ֽ�
              {
                 if(Uart_1.RxBuf[Uart_1.RecLen-1]==0xDD)//֡β��ȷ
                 {
                   Uart_1.AddCrc=Uart_1.AddCrc-0xDD-Uart_1.RxBuf[Uart_1.RecLen-2];
                   if(Uart_1.RxBuf[Uart_1.RecLen-2]==Uart_1.AddCrc)
                   {
                        TIME_U1_STOP;
                        Uart_1.FlagHeadOk=0;
                        
                        //Uart_1.FlagFinish=1;//�������,��¼�����Э��
                        
                        //������ɺ�ת����PC
                        #if RF_U1_EN==1
                        
                          U1_TO_PC;//������ȡ���Զ��ص�PC
                          
                          #if RF_PP_115200_EN==1
                          RfU_Init(9600,1);
                          #endif
                        
                          Pt.PCpt=0;
                          delay_ms(10);
                          
                          
                        #endif
                        
                        ComSendH();
                        U1SendBuf(Uart_1.RxBuf,Uart_1.RecLen);
                        ComSendL();

                    }
                    else//���������
                    {
                      U1_RecErr();
                    }
                 }
                 else//֡β����
                 {
                   U1_RecErr();
                 }
                 return;
              }
              return;
        }
            
            
            //��Ƭ����
            if(Uart_1.RecPt>8)
            {
              U1_RecErr();
              return;
            }
        
            if(Uart_1.RecPt==8)
            {
              
               if((get_add(Uart_1.RxBuf,7)==Uart_1.RxBuf[7])&&
                ((((Uart_1.RxBuf[0]&0xF0)==0xA0)&&(Pt.SnCRC16==U8_TO_U16(Uart_1.RxBuf[2],Uart_1.RxBuf[3])))
                ||(((Uart_1.RxBuf[0]&0xF0)==0xB0)&&((Pt.SnCRC16&0xFFF0)==(0xFFF0&(U8_TO_U16(Uart_1.RxBuf[2],Uart_1.RxBuf[3])))))))
               {
                   TIME_U1_STOP;
                   Uart_1.RecLen=8;
                   Rf_Rec_Yq(Uart_1.RxBuf,Uart_1.RecLen); 
                   Uart_1.FlagHeadOk=0;//�������
               }
               else
               {
                 U1_RecErr();
               }
            }
            return;
            
         #else// FFЭ��
      
          if(Uart_1.FlagHeadOk==0)
          {
            if(UCA1RXBUF>=0xf0)//���ߴ�����F0,�����м���F1-FF
            {
              Uart_1.FlagHeadOk=1;
              Uart_1.FlagFinish=0;
              Uart_1.RecPt=0;
              
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_START;
            }
            else//֡ͷ����
            {
              U1_RecErr();
            }
            return;
          }
        
          TIME_U1_CLR;
          Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
          if(Uart_1.RecPt>RF_CMD_MAX)
          {
            U1_RecErr();
            return;
          }
          
          if(Uart_1.RecPt==3)//�ֽ���,����3��ʱ��Uart_0.RxBuf[2]�ոս���
          {
             Uart_1.RecLen=Uart_1.RxBuf[2]+5;//֡���ȣ�2��
             if(Uart_1.RecLen>RF_CMD_MAX)//���ȴ��� 
             {
               U1_RecErr();
             }
             return;
          }
          if((Uart_1.RecPt>17)&&(Uart_1.RecPt==Uart_1.RecLen))//���һ���ֽ�
          {
            //SN�ű�����ȷ + CRCУ��
            if((CheckBuf(Pt.Sn,&Uart_1.RxBuf[3],10))\
              &&(GetCrc16(Uart_1.RxBuf,Uart_1.RecLen-2)==U8_TO_U16(Uart_1.RxBuf[Uart_1.RecLen-2],Uart_1.RxBuf[Uart_1.RecLen-1])))//CRCУ��
            {
                  //UC1IE &= ~UCA1RXIE;
                  TIME_U1_STOP;
                  Rf_Rec_Yq(Uart_1.RxBuf,Uart_1.RecLen);
                  //LED_OFF;
                  Uart_1.FlagHeadOk=0;//�������
                  //UC1IE |= UCA1RXIE;//���´��ж�
            }
            else
            {
              U1_RecErr();
            }
            return;
          }//U1��RF������ end
        #endif
    }
    else//PCָ��
    {
          if(Uart_1.FlagHeadOk==0)
          {
              if(UCA1RXBUF==0Xaa)
              {
                Flag.Modbus=0;
                
                Uart_1.FlagHeadOk=1;
                Uart_1.FlagFinish=0;
                Uart_1.RecPt=0;
                
                Uart_1.AddCrc=0;
                Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                
                TIME_U1_CLR;
                TIME_U1_START;
              }
              else if(UCA1RXBUF==StateB.Address||UCA1RXBUF==0x00)//modbus��ַ��ȷ
              {
                Flag.Modbus=1;
                
                Uart_1.FlagHeadOk=1;
                Uart_1.FlagFinish=0;
                Uart_1.RecPt=0;
                
                Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                TIME_U1_CLR;
                TIME_U1_START;
              }
              else//֡ͷ����
              {
                U1_RecErr();
              }
              return;
          }
 
          if(Flag.Modbus==1)//modbus��ʽ����
          {
            if(Uart_1.RecPt>=U1MAX)
            {
               U1_RecErr();
            }
            else
            {
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_CLR;
              TIME_U1_START;
            }
            return;
          }
          
          
          TIME_U1_CLR;
          Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
          if(Uart_1.RecPt>U1MAX)
          {
            U1_RecErr();
            return;
          }
          //�жϵ�ַ
          if(Uart_1.RecPt==2)//�豸��ַ
          {
            if(Uart_1.RxBuf[1]!=StateB.Address&&Uart_1.RxBuf[1]!=0x00)//��ַ����
            {
              U1_RecErr();
            }
            return;
          }
          //�ж��ֽ���
          if(Uart_1.RecPt==5)
          {
             Uart_1.RecLen=Uart_1.RxBuf[4]<<8|Uart_1.RxBuf[3];//֡���ȣ�2��
             Uart_1.RecLen+=5;//��Ҫ���յ������ܳ���
             if(Uart_1.RecLen>U1MAX)//���ȴ���
             {
               U1_RecErr();
             }
             return;
          }
          if(Uart_1.RecPt>5&&Uart_1.RecPt==Uart_1.RecLen)//���һ���ֽ�
          {
             if(Uart_1.RxBuf[Uart_1.RecLen-1]==0xcc)//֡β��ȷ
             {
               Uart_1.AddCrc=Uart_1.AddCrc-0xcc-Uart_1.RxBuf[Uart_1.RecLen-2];
               if(Uart_1.RxBuf[Uart_1.RecLen-2]==Uart_1.AddCrc)
               {
                  TIME_U1_STOP;
                  Uart_1.FlagHeadOk=0;
                  Uart_1.FlagFinish=1;//�������
                  __bic_SR_register_on_exit(LPM3_bits); 
                }
                else//���������
                {
                  U1_RecErr();
                }
             }
             else//֡β����
             {
               U1_RecErr();
             }
             return;
          }
          return;
    }
    //((RF_REC_EN==1)&&(RF_U1_EN==1))//U1��RF������ �� TTL����������CO2  else
    #else

          if(Uart_1.FlagHeadOk==0)
          {
              if(UCA1RXBUF==0Xaa)
              {
                Flag.Modbus=0;
                
                Uart_1.FlagHeadOk=1;
                Uart_1.FlagFinish=0;
                Uart_1.RecPt=0;
                
                Uart_1.AddCrc=0;
                Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                
                TIME_U1_CLR;
                TIME_U1_START;
              }
              
              else if(UCA1RXBUF==StateB.Address||UCA1RXBUF==0x00)//modbus��ַ��ȷ
              {
                  Flag.Modbus=1;
                  
                  Uart_1.FlagHeadOk=1;
                  Uart_1.FlagFinish=0;
                  Uart_1.RecPt=0;
                  
                  Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                  TIME_U1_CLR;
                  TIME_U1_START;
              }
              
              #if BSQ_YIDA_EN==1
              else if(UCA1RXBUF==0x1B)
              {
                  Flag.Modbus=0;
                  Flag.BsqYIDA=1;
                  
                  Uart_1.FlagHeadOk=1;
                  Uart_1.FlagFinish=0;
                  Uart_1.RecPt=0;
                  
                  Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                  TIME_U1_CLR;
                  TIME_U1_START;
                  
              }
              
              #endif
              
              
              
              #if ((GPS_MOUSE_EN==1)&&(LG30_EN==0))
              //$GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53
              //$GPRMC,090854.000,A,3018.3243,N,12021.9969,E,0.00,,160415,,,0*05
              else if(UCA1RXBUF=='$')
              {
                  if(StateC.GpsRecWait>0)//GPS_REC_JG_TIME ��Ž���һ�ζ�λ����
                  {
                      U1_RecErr();
                  }
                  else
                  {
                      Flag.Gps=1;
                            
                      Uart_1.FlagHeadOk=1;
                      Uart_1.RecPt=0;
                      //Uart_1.FlagFinish=0;
                            
                      Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                      TIME_U1_CLR;
                      TIME_U1_START;
                  }
              }
              #endif
              
              
              
              else//֡ͷ����
              {
                U1_RecErr();
              }
              return;
          }
   
            #if ((GPS_MOUSE_EN==1)&&(LG30_EN==0))
            if(Flag.Gps==1)//GPS����
            {
                    if(Uart_1.RecPt>=81)
                    {
                       U1_RecErr();
                    }
                    else
                    {
                      Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                      TIME_U1_CLR;
                      TIME_U1_START;
                      
                      if(Uart_1.RecPt>6)
                      {
                        //�ٴμ���ǲ���  $GPRMC  �� $GNRMC ���� ,�޳���������
                        /*
                            $GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53
                            $GPGSA,A,3,31,19,23,11,04,,,,,,,,6.0,4.0,4.4*3F
                            $GPGSV,3,1,10,27,73,027,14,19,71,222,34,23,39,236,34,11,17,194,32*70
                            $GPGSV,3,2,10,04,16,174,31,31,11,132,34,16,47,036,,09,34,276,14*76
                            $GPGSV,3,3,10,07,22,318,,21,06,041,*75
                            $GPGLL,3018.3243,N,12021.9969,E,090854.000,A,0*2B
                            $GPRMC,090854.000,A,3018.3243,N,12021.9969,E,0.00,,160415,,,0*05
                            $GPVTG,,T,,M,0.00,N,0.0,K,0*62
                        
                            //$GPRMC,074110.000,V,3018.2827,N,12022.0094,E,,,081214,,,0*0E(GPS-VK2828A8G5-4λС��)
                            //$GNRMC,020213.000,A,3018.3040,N,12022.0080,E,0.06,137.15,200515,,,D*75(����-VK2828M3G5-4λС��)
                        */
                        
                         if((Uart_1.RxBuf[3]=='R')&&(Uart_1.RxBuf[4]=='M')&&(Uart_1.RxBuf[5]=='C'))//RMC
                         //if((Uart_1.RxBuf[3]=='G')&&(Uart_1.RxBuf[4]=='G')&&(Uart_1.RxBuf[5]=='A'))//GGA
                         {
                             if(((Uart_1.RxBuf[Uart_1.RecPt-2]==0x0d)&&(Uart_1.RxBuf[Uart_1.RecPt-1]==0x0a))||(Uart_1.RecPt>=80))//����
                             {
                               if(StateC.GpsRecWait==0)
                               {
                                  StateC.GpsRecWait=GPS_REC_JG_TIME;//MCU ����GPS������С���
                                 
                                  TIME_U1_STOP;
                                  Uart_1.RecLen=Uart_1.RecPt;
                                  Uart_1.RecPt=0;
                                  Uart_1.FlagHeadOk=0;
                                  
                                  
                                  //����,�鿴GPS���ص�����---------zz
        
                                  if(1==StateC.SeeBDSing)
                                  {
                                      U1_TO_PC;
                                      ComSendH();
                                      U1SendBuf(Uart_1.RxBuf,Uart_1.RecLen);
                                      ComSendL();
                                      U1_TO_RF; 
                                  }
       
                                  //-----------------------------------
        
                                  
                                  #if ((SIM68_EN==1)&&(SIM68_SLP_EN==1))
                                  u8 tosleep=0;
                                  if(Pt.GprsSleep==1)
                                  {
                                        
                                        StateC.AwakeWait=SIM68_SLP_TIME;//���Ѽ��
                                        
                                        if(Get_GpsPosition())//��Ч���ݣ����Ѻ���20s���ܽ��յ���Ч����
                                        {
                                              tosleep=1;
                                              
                                              //����SLEEP  320uA, �����κ��ַ��ɻ���
                                              SIM68_to_sleep();
                                              
                                        }
                                  }
                                  else
                                  {
                                     Get_GpsPosition();  
                                  }
                                  
                                  //����ǹػ�״̬�������ߣ����ٻ���
                                  if(Sms.GsmKeyShut==1)
                                  {
                                    if(tosleep==0)
                                    {
                                      SIM68_to_sleep();
                                    }
                                    StateC.AwakeWait=0;
                                  }
                                  
        
                                  #else
                                  
                                  Get_GpsPosition();
        
                                  
                                  #endif
                                  
                               }
                             }
                           
                         }
                         else
                         {
                           U1_RecErr();//���� GPRMC �� GNRMC ���ݣ��޳�
                         }
        
                      }
                      
                    }
                    return;
            }
            #endif
          
    

          if(Flag.Modbus==1)//modbus��ʽ����
          {
            if(Uart_1.RecPt>=U1MAX)
            {
               U1_RecErr();
            }
            else
            {
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_CLR;
              TIME_U1_START;
            }
            return;
          }
    
          #if BSQ_YIDA_EN==1
          else if(Flag.BsqYIDA==1)
          {

                if(Uart_1.RecPt>=40)
                {
                   U1_RecErr();
                }
                else
                {
                  Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
                  TIME_U1_CLR;
                  TIME_U1_START;
                }
                
                
                if(Uart_1.RecPt==3)
                {
                   if(!((Uart_1.RxBuf[0]==0x1B)&&(Uart_1.RxBuf[1]==0x06)))//��ͷ�ж�
                   {
                     U1_RecErr();
                   }
                  
                   //if(StateB.Address!=U8_TO_U16(Uart_1.RxBuf[2],Uart_1.RxBuf[3]))//��ַ����ȷ
                   if(StateB.Address!=Uart_1.RxBuf[2])
                   {
                       U1_RecErr();
                   }
                   
                }
                
                if(Uart_1.RecPt==10)
                {

                      //����CRC
                      unsigned short crc16;
                      crc16=GetCrc16(&Uart_1.RxBuf[2],6);//��ʼ1B 06 ������CRC
                      if(crc16==U8_TO_U16(Uart_1.RxBuf[8],Uart_1.RxBuf[9]))//CRC У����ȷ
                      {
                          Uart_1.RecLen=Uart_1.RecPt;

                          TIME_U1_STOP;
                          Uart_1.FlagHeadOk=0;
                          Uart_1.FlagFinish=1;//�������
                          __bic_SR_register_on_exit(LPM3_bits); 
       
                      }
                      else
                      {
                          U1_RecErr();//�������
                          
                      }

                  
                }
                
                return;
          }
          #endif

          
          
          TIME_U1_CLR;
          Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
          if(Uart_1.RecPt>U1MAX)
          {
            U1_RecErr();
            return;
          }
          //�жϵ�ַ
          if(Uart_1.RecPt==2)//�豸��ַ
          {
            if(Uart_1.RxBuf[1]!=StateB.Address&&Uart_1.RxBuf[1]!=0x00)//��ַ����
            {
              U1_RecErr();
            }
            return;
          }
          //�ж��ֽ���
          if(Uart_1.RecPt==5)
          {
             Uart_1.RecLen=Uart_1.RxBuf[4]<<8|Uart_1.RxBuf[3];//֡���ȣ�2��
             Uart_1.RecLen+=5;//��Ҫ���յ������ܳ���
             if(Uart_1.RecLen>U1MAX)//���ȴ���
             {
               U1_RecErr();
             }
             return;
          }
          if(Uart_1.RecPt>5&&Uart_1.RecPt==Uart_1.RecLen)//���һ���ֽ�
          {
             if(Uart_1.RxBuf[Uart_1.RecLen-1]==0xcc)//֡β��ȷ
             {
               Uart_1.AddCrc=Uart_1.AddCrc-0xcc-Uart_1.RxBuf[Uart_1.RecLen-2];
               if(Uart_1.RxBuf[Uart_1.RecLen-2]==Uart_1.AddCrc)
               {
                  TIME_U1_STOP;
                  Uart_1.FlagHeadOk=0;
                  Uart_1.FlagFinish=1;//�������
                  __bic_SR_register_on_exit(LPM3_bits); 
                }
                else//���������
                {
                  U1_RecErr();
                }
             }
             else//֡β����
             {
               U1_RecErr();
             }
             return;
          }
          return;

    #endif
    //(((RF_REC_EN==1)||(RF_RP_EN==1))&&(RF_U1_EN==1))//U1��RF������  end    
          
   //MODBUS_ENABLE==1  else
   #else//-----------------------------------------------------------------------
  
    if(Uart_1.FlagHeadOk==0)
    {
      if((UCA1RXBUF==0Xaa))
      {
        Uart_1.FlagHeadOk=1;
        Uart_1.FlagFinish=0;
        Uart_1.RecPt=0;
        
        Uart_1.AddCrc=0;
        Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
        TIME_U1_CLR;
        TIME_U1_START;
      }
      
      #if ((GPS_MOUSE_EN==1)&&(LG30_EN==0))
      //$GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53
      //$GPRMC,090854.000,A,3018.3243,N,12021.9969,E,0.00,,160415,,,0*05
      else if(UCA1RXBUF=='$')
      {
          if(StateC.GpsRecWait>0)//GPS_REC_JG_TIME��Ž���һ�ζ�λ����
          {
              U1_RecErr();
          }
          else
          {
              Flag.Gps=1;
                    
              Uart_1.FlagHeadOk=1;
              Uart_1.RecPt=0;
              //Uart_1.FlagFinish=0;
                    
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_CLR;
              TIME_U1_START;
          }
      }
      #endif
      
      
      
      #if SEN_MODBUS_485_EN==1
      else if((UCA1RXBUF==0x01)||(UCA1RXBUF==0x02))//1�Ż�2�Ŵ�����
      {
            StateC.sen_refg=1;
        
            Uart_1.FlagHeadOk=1;
            Uart_1.FlagFinish=0;
            Uart_1.RecPt=0;
            Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
            TIME_U1_CLR;
            TIME_U1_START;
      }
      
      
      #endif
      
      
      else//֡ͷ����
      {
        U1_RecErr();
      }
      return;
    }
    
    #if SEN_MODBUS_485_EN==1
    if(StateC.sen_refg==1)
    {
            if(Uart_1.RecPt>=40)
            {
               U1_RecErr();
            }
            else
            {
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_CLR;
              TIME_U1_START;
            }
            
            
            if(Uart_1.RecPt==3)
            {
               Uart_1.RecLen=Uart_1.RxBuf[2]+5;
               if(Uart_1.RecLen>40)
               {
                 U1_RecErr();
               }
            }
            
            if(Uart_1.RecPt>10)
            {
               if(Uart_1.RecPt==Uart_1.RecLen)//���һ���ֽ�
               {
                  //����CRC
                  unsigned short crc16;
                  crc16=GetCrc16(Uart_1.RxBuf,Uart_1.RecLen-2);
                  if(crc16==U8_TO_U16(Uart_1.RxBuf[Uart_1.RecLen-2],Uart_1.RxBuf[Uart_1.RecLen-1]))//CRC У����ȷ
                  {
                      Get485VV(Uart_1.RxBuf,Uart_1.RecLen);
                      U1_RecErr();//�������
   
                  }
                 
               }
              
              
              
            }
            
            
      
            return;
    }
    #endif
    

    #if ((GPS_MOUSE_EN==1)&&(LG30_EN==0))
    if(Flag.Gps==1)//GPS����
    {
            if(Uart_1.RecPt>=81)
            {
               U1_RecErr();
            }
            else
            {
              Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
              TIME_U1_CLR;
              TIME_U1_START;
              
              if(Uart_1.RecPt>6)
              {
                //�ٴμ���ǲ���  $GPRMC  �� $GNRMC ���� ,�޳���������
                /*
                    $GPGGA,090854.000,3018.3243,N,12021.9969,E,1,05,4.0,56.8,M,7.1,M,,0000*53
                    $GPGSA,A,3,31,19,23,11,04,,,,,,,,6.0,4.0,4.4*3F
                    $GPGSV,3,1,10,27,73,027,14,19,71,222,34,23,39,236,34,11,17,194,32*70
                    $GPGSV,3,2,10,04,16,174,31,31,11,132,34,16,47,036,,09,34,276,14*76
                    $GPGSV,3,3,10,07,22,318,,21,06,041,*75
                    $GPGLL,3018.3243,N,12021.9969,E,090854.000,A,0*2B
                    $GPRMC,090854.000,A,3018.3243,N,12021.9969,E,0.00,,160415,,,0*05
                    $GPVTG,,T,,M,0.00,N,0.0,K,0*62
                
                    //$GPRMC,074110.000,V,3018.2827,N,12022.0094,E,,,081214,,,0*0E(GPS-VK2828A8G5-4λС��)
                    //$GNRMC,020213.000,A,3018.3040,N,12022.0080,E,0.06,137.15,200515,,,D*75(����-VK2828M3G5-4λС��)
                
                   //$GPRMC,021543.020,V,,,,,0.00,0.00,060180,,,N*41    (N10)
                */
                
                 //if((Uart_1.RxBuf[3]=='G')&&(Uart_1.RxBuf[4]=='S')&&(Uart_1.RxBuf[5]=='V'))//GSV
                 if((Uart_1.RxBuf[3]=='R')&&(Uart_1.RxBuf[4]=='M')&&(Uart_1.RxBuf[5]=='C'))//RMC
                 //if((Uart_1.RxBuf[3]=='G')&&(Uart_1.RxBuf[4]=='G')&&(Uart_1.RxBuf[5]=='A'))//GGA
                 {
                     if(((Uart_1.RxBuf[Uart_1.RecPt-2]==0x0d)&&(Uart_1.RxBuf[Uart_1.RecPt-1]==0x0a))||(Uart_1.RecPt>=80))//����
                     {
                       if(StateC.GpsRecWait==0)
                       {
                          StateC.GpsRecWait=GPS_REC_JG_TIME;//MCU ����GPS������С���
                         
                          TIME_U1_STOP;
                          Uart_1.RecLen=Uart_1.RecPt;
                          Uart_1.RecPt=0;
                          Uart_1.FlagHeadOk=0;
                          
                          
                          //����,�鿴GPS���ص�����---------zz
                          #if SIM68_EN==1
                          if(1==StateC.SeeBDSing)
                          {
                              U1_TO_PC;
                              ComSendH();
                              U1SendBuf(Uart_1.RxBuf,Uart_1.RecLen);
                              ComSendL();
                              U1_TO_RF; 
                          }
                          #endif
                          //-----------------------------------

                          
                          #if ((SIM68_EN==1)&&(SIM68_SLP_EN==1))
                          u8 tosleep=0;
                          if(Pt.GprsSleep==1)
                          {
                                StateC.AwakeWait=SIM68_SLP_TIME;
                            
                                if(Get_GpsPosition())//��Ч���ݣ����Ѻ���20s���ܽ��յ���Ч����
                                {
                                    //����SLEEP  320uA, �����κ��ַ��ɻ���
                                    SIM68_to_sleep();
                                    
                                    tosleep=1;
                                }

                          }
                          else
                          {
                             Get_GpsPosition();  
                          }
                          
                          //����ǹػ�״̬�������ߣ����ٻ���
                          if(Sms.GsmKeyShut==1)
                          {
                            if(tosleep==0)
                            {
                              SIM68_to_sleep();
                            }
                            StateC.AwakeWait=0;
                          }
                          

                          #else
                          
                          Get_GpsPosition();

                          
                          #endif
                          
                       }
                     }
                   
                 }
                 else
                 {
                   U1_RecErr();//���� GPRMC �� GNRMC ���ݣ��޳�
                 }

              }
              
            }
            return;
    }
    #endif
    
      
    
    TIME_U1_CLR;
    Uart_1.AddCrc+=Uart_1.RxBuf[Uart_1.RecPt++]=UCA1RXBUF;
    if(Uart_1.RecPt>U1MAX)
    {
      U1_RecErr();
      return;
    }
    //�жϵ�ַ
    if(Uart_1.RecPt==2)//�豸��ַ
    {
      if(Uart_1.RxBuf[1]!=StateB.Address&&Uart_1.RxBuf[1]!=0x00)//��ַ����
      {
        U1_RecErr();
      }
      return;
    }
    //�ж��ֽ���
    if(Uart_1.RecPt==5)
    {
       Uart_1.RecLen=Uart_1.RxBuf[4]<<8|Uart_1.RxBuf[3];//֡���ȣ�2��
       Uart_1.RecLen+=5;//��Ҫ���յ������ܳ���
       if(Uart_1.RecLen>U1MAX)//���ȴ���
       {
         U1_RecErr();
       }
       return;
    }
    if(Uart_1.RecPt>5)
    {
       if(Uart_1.RecPt==Uart_1.RecLen)//���һ���ֽ�
       {
           if(Uart_1.RxBuf[Uart_1.RecLen-1]==0xcc)//֡β��ȷ
           {
             Uart_1.AddCrc=Uart_1.AddCrc-0xcc-Uart_1.RxBuf[Uart_1.RecLen-2];
             if(Uart_1.RxBuf[Uart_1.RecLen-2]==Uart_1.AddCrc)
             {
                TIME_U1_STOP;
                Uart_1.FlagHeadOk=0;
                Uart_1.FlagFinish=1;//�������
                __bic_SR_register_on_exit(LPM3_bits); 
              }
              else//���������
              {
                U1_RecErr();
              }
           }
           else//֡β����
           {
             U1_RecErr();
           }
           return;
       }
    }
    return;
    
    #endif  //MODBUS_ENABLE==1  end
    
#endif // RF_MULTI_EN==1   end

    
#endif //WIFI_ENABLE
}

#pragma vector=TIMERA1_VECTOR// Timer_A3 Interrupt Vector (TAIV) handler
__interrupt void Timer_A(void)
{
  //_EINT();//zz
  switch( TAIV )
  {
    case  2:  
              #if ((GSM_ENABLE==1)||(SEN_UART_EN==1))
      
                  TIME_U0_STOP;
                  Uart_0.FlagFinish=1;
                  Uart_0.RecLen=Uart_0.RecPt;
                  Uart_0.RecPt=0;
                  

                  #if ((SEN_UART_EN==1)&&(SEN_U1_EN==0))
                  StateC.sen_vv=Get_SenTTL_Value(Uart_0.RxBuf,Uart_0.RecLen);
                  #endif
                  
                  __bic_SR_register_on_exit(LPM3_bits);
                  
              #elif  ((RF_REC_EN==1)||(RF_RP_EN==1))
                  U0_RecErr();
                  
              #elif  WIRELESS_ENABLE==1
                #if MODBUS_ENABLE==1
                if(Flag.Modbus==1)
                {
                  TIME_U0_STOP;
                  Uart_0.FlagHeadOk=0;
                  Uart_0.RecLen=Uart_0.RecPt;
                  Uart_0.RecPt=0;
                                 
                  //����������ɣ�����CRCУ��
                  if(GetCrc16(Uart_0.RxBuf,Uart_0.RecLen-2)\
                    ==U8_TO_U16(Uart_0.RxBuf[Uart_0.RecLen-2],Uart_0.RxBuf[Uart_0.RecLen-1]))
                  {
                    Uart_0.FlagFinish=1;//������ɣ�������Э��
                    //LED_ON;
                    
                    //������ɺ�ת�Ƶ�U1����
                    Uart_1.FlagFinish=1;
                    Uart_1.RecLen=Uart_0.RecLen;
                    for(u16 j=0;j<Uart_1.RecLen;j++)
                      Uart_1.RxBuf[j]=Uart_0.RxBuf[j];
                    
                    __bic_SR_register_on_exit(LPM3_bits); 
                  }
                }
                else
                {
                  U0_RecErr();
                }
                #else
                U0_RecErr();
                #endif
              #endif
              break;
    case  4:  
#if RF_MULTI_EN==1
                    TIME_U1_STOP;
                    Uart_1.RecLen=Uart_1.RecPt;
                    Uart_1.RecPt=0;
                    //����������ɣ�����CRCУ��
                    if(GetCrc16(Uart_1.RxBuf,Uart_1.RecLen-2)==U8_TO_U16(Uart_1.RxBuf[Uart_1.RecLen-2],Uart_1.RxBuf[Uart_1.RecLen-1]))
                    {
                      Flag.Modbus=1;
                      Uart_1.FlagFinish=1;
                      __bic_SR_register_on_exit(LPM3_bits); 
                    }
                    else if(get_add(Uart_1.RxBuf,Uart_1.RecLen-2)==Uart_1.RxBuf[Uart_1.RecLen-2])
                    {
                      Flag.Modbus=0;
                      Uart_1.FlagFinish=1;
                      __bic_SR_register_on_exit(LPM3_bits); 
                    }
                    else
                    {
                      U1_RecErr();
                    }
#elif WIFI_ENABLE==1
                    TIME_U1_STOP;
                    Uart_1.RecLen=Uart_1.RecPt;
                    Uart_1.RecPt=0;    
                    
                    //����������ɣ�����CRCУ��
                    if(GetCrc16(Uart_1.RxBuf,Uart_1.RecLen-2)==U8_TO_U16(Uart_1.RxBuf[Uart_1.RecLen-2],Uart_1.RxBuf[Uart_1.RecLen-1]))
                    {
                       Flag.Modbus=1;
                    }
                    else
                    {
                       Flag.Modbus=0;
                    }
                    
                    
                     Uart_1.FlagFinish=1;
                     __bic_SR_register_on_exit(LPM3_bits); 
                    
#else
      
              #if SEN_U1_EN==1
                    
               if(Pt.U1toPC==0)
               {
                  TIME_U1_STOP;
                  Uart_1.FlagFinish=1;
                  Uart_1.RecLen=Uart_1.RecPt;
                  Uart_1.RecPt=0;
                  
                  #if ((SEN_UART_EN==1)&&(SEN_U1_EN==1))
                  StateC.sen_vv=Get_SenTTL_Value(Uart_1.RxBuf,Uart_1.RecLen);
                  #endif
                  
                  //__bic_SR_register_on_exit(LPM3_bits);
                  return;
               }
              #endif
                    
                    
              #if MODBUS_ENABLE==1
                  if(Flag.Modbus==1)
                  {
                    TIME_U1_STOP;
                    Uart_1.FlagHeadOk=0;
                    Uart_1.RecLen=Uart_1.RecPt;
                    Uart_1.RecPt=0;
                                   
                    //����������ɣ�����CRCУ��
                    if((Uart_1.RecLen>2)\
                      &&(GetCrc16(Uart_1.RxBuf,Uart_1.RecLen-2)==U8_TO_U16(Uart_1.RxBuf[Uart_1.RecLen-2],Uart_1.RxBuf[Uart_1.RecLen-1])))
                    {
                      Uart_1.FlagFinish=1;
                      __bic_SR_register_on_exit(LPM3_bits); 
                    }
                    else
                    {
                      U1_RecErr();
                    }
                  }
                  else
                  {
                    U1_RecErr();
                  }
              #else
                  U1_RecErr();   
              #endif
                  
#endif
              break;
    case 10:  break;
              
  }
}
#pragma vector=TIMERB0_VECTOR
__interrupt void timerb0_isr(void)
{
  TB0_STOP;
  ADC_START;
}
#pragma vector=TIMERB1_VECTOR//Timer_B7 Interrupt Vector (TBIV) handler
__interrupt void Timer_B(void)
{
   //_EINT();//zz
   switch( TBIV )
   {
     case  2:  
              Flag.Loop=1;
              TBCCR1+=TB1_DELAY;
              __bic_SR_register_on_exit(LPM3_bits);
              break;
     case  4: 
              if(KeyXiaoDou())
              {
                #if LCD_NO==7
                BellOne();
                #endif
                __bic_SR_register_on_exit(LPM3_bits);//������ʱ��,�а������� 
              }
              break;
     case  6:
              TBCCR3+=TB3_DELAY;//0.5S

              __bic_SR_register_on_exit(LPM3_bits);
              break;
     case 8: 
#if (JLY_MODEL==_1_PM25_PWM_1000)
              
              // y = 56100 * (x^3) - 6920 * (x^2) + 1560* (x)
              //���У�x=Lo(Average)ռ�ȣ�y=ug/m3
       
              if(!(P5IN&BIT1))//�͵�ƽռ�ձ�
                Pt.pwm_L++;
              Pt.pwm_A++;
              
              if(Pt.pwm_A>35000-1)//1ms*x
              {
                float ft=Pt.pwm_L;
                ft=ft/Pt.pwm_A;
                ft=56100*ft*ft*ft-6920*ft*ft+1560*ft;
                Pt.pwm_V=(u16)ft;

                Pt.pwm_L=Pt.pwm_A=0;
              }
              TBCCR4+=TB4_PMW_DELAY;
              
#elif PT_MAX31865_EN==1
              
              //��ȡ����ͨ���¶�
              for(u8 j=0;j<CH_NUM;j++)
                 Pt.t_temp[j]=max31865_temp(j);
              
              //�ص�Դ��ʡ��
              NTC_OFF;
              rtd_io_all_l();
              
              TB4_STOP;
#elif RF_KP_EN==1
              
              TBCCR4+=TB4_KP_DELAY;
              if(RfKpHave())
              {
                Pt.RfTing=1;
                __bic_SR_register_on_exit(LPM3_bits);
              }
              
#else
       
              #if LED_WORK_ENABLE==1 
              LED1_OFF;
              #endif
              
              #if U0U1_ENABLE==1
              ComSendL();
              #endif
              
              TB4_STOP;
#endif
              break;
     
     case 10:
              Flag.KeyLcdLoop=1;
              TBCCR5+=TB5_DELAY;
              __bic_SR_register_on_exit(LPM3_bits);
              break;
     
     case 12: break;
     case 14: break;
   }
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  if(Pt.AdcPt<=ADC_CONSEQ_N)
  {
      StateC.Asum[0] += ADC12MEM0;
      StateC.Asum[1] += ADC12MEM1;
      StateC.Asum[2] += ADC12MEM2;
      StateC.Asum[3] += ADC12MEM3;
      StateC.Asum[4] += ADC12MEM4;
      StateC.Asum[5] += ADC12MEM5;
      StateC.Asum[6] += ADC12MEM6;
      StateC.Asum[7] += ADC12MEM7;
        
      Pt.AdcPt++;
      if(Pt.AdcPt==ADC_CONSEQ_N)
      {
          Pt.AdcPt=0;
          for(u8 j=0;j<8;j++)
          {
            StateC.Adc[j]=StateC.Asum[j]/ADC_CONSEQ_N;
            StateC.Asum[j]=0;
          }
          ADC_OFF;
          ADC_REF_OFF;
          
          
          
          #if PT_MAX31865_EN==1
            NTC_ON;//��Դ����
            
            //����ͨ��,��ʼת��
            for(u8 j=0;j<CH_NUM;j++)
              max31865_start(j);
  
            TB4_RTD_START;//�ȴ�70ms�󣬶�ȡ�¶�
          
         
          
          #elif K_MAX31855_EN==1
          
              NTC_ON;//��Դ����
          
              //��ȡ����ͨ���¶�
              int out[2];
              for(u8 j=0;j<CH_NUM;j++)
              {
                 Get_K_Temp(out,j);
                 Pt.t_temp[j]=out[0];
              }
              
              //�ص�Դ��ʡ��
              NTC_OFF;
              k_io_all_l();
              
          #endif
          
          
          #if ((POWER_TIME_ENABLE==1)||(PT_MAX31865_EN==1))//��ǰ�򿪵�Դ�������ʵʱ�ɼ�ʱ����Դ���أ�ʡ��ʱ��¼�����Ź�
          #else
            VT_CTL_OFF;
          #endif
          
          VPP_CTL_OFF;
          Flag.SampPt=2;//AD������������ʼ��������ֵ
  
          #if BAT_AD_ENABLE==1
          
              #if BAT_FIL_EN==1
                 #if DIGITAL_FILTER_EN==1//�˲�
                 GetChFilterAd(7);
                 #endif
              #endif
          
          
              u32 Long;
              Long=StateC.Adc[7];
              Long=Long*500;//10K/10K
              Long=Long/4095;
              StateC.BatV=(u16)Long;//Vbat
    
              #if LCD_NO==7
                    #if GPRS_ENABLE==1
                      if(GetBatt()<10)//zz
                      {
                        LcdBatOn(1);
                      }
                      else
                      {
                        LcdBatOn(0);
                      }
          
                    #else
                    
                      if(HavePow())//�����
                      {
                          LcdBatOn(0);
                      }
                      else
                      {
                          if(StateC.BatV<=BAT_LOW)//��ص�ѹ����
                          {
                            LcdBatOn(1);
                          }
                          else
                          {
                            LcdBatOn(0);
                          }
                      }
                    #endif
                #endif
          #endif
      }
      else
      {
          ADC_START;
      }
  }
}
#pragma vector=NMI_VECTOR
__interrupt void nmi_ (void)
{
}
#pragma vector=WDT_VECTOR
__interrupt void wdt_isr(void)
{
}
#pragma vector=TIMERA0_VECTOR
__interrupt void timera0_isr(void)
{
}

#pragma vector=COMPARATORA_VECTOR
__interrupt void comaratora__isr(void)
{
}

#pragma vector=PORT1_VECTOR
__interrupt void port1_isr(void)
{
  
  #if WK2124_EN==1
  WK_IRQ_Do();
  #endif
  
  KeyIsrDo();
  P1IFG=0;//�������
}
#pragma vector=PORT2_VECTOR
__interrupt void port2_isr(void)
{
  if(StateA2.WorkState==WORK_STATE_RECORDING)//���ڼ�¼
  {
     #if BSQ_ENABLE==0
     if(!((StateB.StartMode==START_MODE_HAND)&&(StateB.StopMode==STOP_MODE_HAND)&&(Flag.HandOff==1)))//���ֶ�ͣ��
     #endif
     {
         //����--------------------------------------------------------------------------------
         #if ((JLY_MODEL==_1_01YL)||(JLY_MODEL==_1_02YL)||(JLY_MODEL==_1_05YL)||(JLY_MODEL==_1_CS))
         if((YL1_IFG&YL1_BIT)==YL1_BIT)
         {
             CLR_YL1_INT;//�����жϹر�
             Delay(2000);
             if((YL1_PIN&YL1_BIT)==YL1_BIT)
             {
                 Delay(2000);                                                                                                                                                                                                                                                  
                 if((YL1_PIN&YL1_BIT)==YL1_BIT)
                 {
                     #if (JLY_MODEL==_1_01YL)
                     Pt.RainPt=Pt.RainPt+1;//0.1
                     #elif (JLY_MODEL==_1_02YL)
                     Pt.RainPt=Pt.RainPt+2;//0.2
                     #elif (JLY_MODEL==_1_05YL)
                     Pt.RainPt=Pt.RainPt+5;//0.5
                     #elif (JLY_MODEL==_1_CS)
                     Pt.RainPt=Pt.RainPt+10;//���� 1.0
                     #endif
                     StateC.Value[0]=Pt.RainPt;//��1ͨ��
                     Flag.RstFirstSample=1;//��һ����ʱ��һֱ��1
                     LcdValue();
                 }
             }
             SET_YL1_INT;
         }
         
         #elif (JLY_MODEL==_2_NTC_01YL)
         if((YL1_IFG&YL1_BIT)==YL1_BIT)//����
         {
            if((YL1_PIN&YL1_BIT)==YL1_BIT)
            {
               Delay(200);                                                                                                                                                                                                                                                  
               if((YL1_PIN&YL1_BIT)==YL1_BIT)
               {
                  Pt.RainPt=Pt.RainPt+1;//1λС����
                  StateC.Value[1]=Pt.RainPt;//��2ͨ��
               }
            }
         }
         
         
         #elif ((JLY_MODEL==_3_FS_FX_01YL)||(JLY_MODEL==_3_NTC_HIH5030_01YL))
         if((YL1_IFG&YL1_BIT)==YL1_BIT)//����
         {
            if((YL1_PIN&YL1_BIT)==YL1_BIT)
            {
               Delay(200);                                                                                                                                                                                                                                                  
               if((YL1_PIN&YL1_BIT)==YL1_BIT)
               {
                  Pt.RainPt=Pt.RainPt+1;//1λС����
                  StateC.Value[2]=Pt.RainPt;//��3ͨ��
               }
            }
         }
         


         #elif ((JLY_MODEL==_6_WD_GZ100K_SD_YL01_FS_FX)||(JLY_MODEL==_4_WD_GZ100K_SD_YL01)\
           ||(JLY_MODEL==_8_WD_GZ100K_SD_YL01_TRWD_TRSF_FS_FX)||(JLY_MODEL==_7_WD_GZ200K_SD_YL01_FS_FX_TRWD)\
               ||(JLY_MODEL==_8B_WD_GZ100K_SD_YL01_TRWD_TRSF_FS_FX)||(JLY_MODEL==_6_WD_SD_QY_YL01_FS_FX)\
               ||(JLY_MODEL==_8B_WD_GZV200K_SD_YL01_TRWD_TRSF_FS_FX)||(JLY_MODEL==_6B_WD_SD_QY_YL01_FS_FX))

         if((YL1_IFG&YL1_BIT)==YL1_BIT)//����
         {
            if((YL1_PIN&YL1_BIT)==YL1_BIT)
            {
               Delay(200);                                                                                                                                                                                                                                                  
               if((YL1_PIN&YL1_BIT)==YL1_BIT)
               {
                  Pt.RainPt=Pt.RainPt+1;//1λС����0.1
                  StateC.Value[3]=Pt.RainPt;//��4ͨ��
               }
            }
         }
         
         
         #elif (JLY_MODEL==_6_WD_GZ100K_SD_YL05_FS_FX)
         if((YL1_IFG&YL1_BIT)==YL1_BIT)//����
         {
            if((YL1_PIN&YL1_BIT)==YL1_BIT)
            {
               Delay(200);                                                                                                                                                                                                                                                  
               if((YL1_PIN&YL1_BIT)==YL1_BIT)
               {
                  Pt.RainPt=Pt.RainPt+5;//1λС����0.5
                  StateC.Value[3]=Pt.RainPt;//��4ͨ��
               }
            }
         }
         
         #elif ((JLY_MODEL==_7_WD_GZ100K_TRWD_TRSF_SD_YL01_QY))
         if((YL1_IFG&YL1_BIT)==YL1_BIT)//����
         {
            if((YL1_PIN&YL1_BIT)==YL1_BIT)
            {
               Delay(200);                                                                                                                                                                                                                                                  
               if((YL1_PIN&YL1_BIT)==YL1_BIT)
               {
                  Pt.RainPt=Pt.RainPt+1;//1λС����
                  StateC.Value[5]=Pt.RainPt;//��6ͨ��������
               }
            }
         }
         
         
         #endif
         
         
         //����---------------------------
         #if FS_FX_ENABLE==1
         if((FS_IFG&FS_BIT)==FS_BIT)//����
         {
            if((FS_PIN&FS_BIT)==0)
            {
               delay_us(100);
               if((FS_PIN&FS_BIT)==0)
               {
                 Pt.FsPt++;
               }
            }
         }
         #endif
         
         
     }
  }
  
  P2IFG=0;//�������
}
#pragma vector=USCIAB0TX_VECTOR
__interrupt void usciab0tx_isr(void)
{
}
#pragma vector=USCIAB1TX_VECTOR
__interrupt void usciab1tx_isr(void)
{
}
#pragma vector=RESERVED20_VECTOR
__interrupt void resered20_isr(void)
{
}
#pragma vector=RESERVED0_VECTOR
__interrupt void reserved0_isr(void)
{
}
#pragma vector=RESERVED1_VECTOR
__interrupt void reserved1_isr(void)
{
}
#pragma vector=RESERVED2_VECTOR
__interrupt void reserved2_isr(void)
{
}
#pragma vector=RESERVED3_VECTOR
__interrupt void reserved3_isr(void)
{
}
#pragma vector=RESERVED4_VECTOR
__interrupt void reserved4_isr(void)
{
}
#pragma vector=RESERVED5_VECTOR
__interrupt void reserved5_isr(void)
{
}
#pragma vector=RESERVED6_VECTOR
__interrupt void reserved6_isr(void)
{
}
#pragma vector=RESERVED7_VECTOR
__interrupt void reserved7_isr(void)
{
}
#pragma vector=RESERVED8_VECTOR
__interrupt void reserved8_isr(void)
{
}
#pragma vector=RESERVED9_VECTOR
__interrupt void reserved9_isr(void)
{
}
#pragma vector=RESERVED10_VECTOR
__interrupt void reserved10_isr(void)
{
}
#pragma vector=RESERVED11_VECTOR
__interrupt void reserved11_isr(void)
{
}
#pragma vector=RESERVED12_VECTOR
__interrupt void reserved12_isr(void)
{
}
#pragma vector=RESERVED13_VECTOR
__interrupt void reserved13_isr(void)
{
}
#pragma vector=RESERVED14_VECTOR
__interrupt void reserved14_isr(void)
{
}
#pragma vector=RESERVED15_VECTOR
__interrupt void reserved15_isr(void)
{
}


