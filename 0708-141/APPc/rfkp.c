#include "_config.h"

#if RF_KP_EN==1


//�������ߴ����� Ŀ��SN
void SetKpSnToEE(void)
{
   I2C_EE_BufferWrite(Pt.KpSn,RF_SN_ADDR,2,0);//�����־
}
//��ȡ���ߴ����� Ŀ��SN
void GetKpSnFromEE(void)
{
  I2C_EE_BufferRead(Pt.KpSn,RF_SN_ADDR,2,0);
}

//�̶�8�ֽ�Э��: A0(1)  ADDR(1)  SN(2)  TH(3)   CRC(1) 
        
// TH(3): ��ʪ�ȹ���3���ֽ�, ��24λ���¶�ռ14λ��23-10����ʪ��ռ10λ��9-0��
// �¶�14λ���λ1Ϊ����0Ϊ��������13λ��ֵ.  ��Χ��-8191��8191��
// ʪ��10λ����Χ(0-3FF)�� ��(0,1023)

//������ָʾ----------------------------------------
//B0(1)  ADDR(1)  SN_BAT(2)  TH(3)  ADD(1)
//������A0ָ��һ��������
//1.  A0�����B0
//2.  SN(2) �ֳ���2���� SN_BAT(2)
//    AAAB   AAAΪԭ��SN(2)��ǰ12λ�� BΪ����(0-9)
/*
��3�� ֧��2���¶�  170818  ��Ҫ��������
B0(1)  ADR(1)  SN_BAT(2)  TH(3)  ADD(1)
TH(3) ����Ϊ2��ͨ����ǰ12λΪͨ��1����12λΪͨ��2
       ���λ1Ϊ������Ϊ��������11λ��ֵ����Χ 7FF( -2047, 2047)
ADR(1): ���λ1Ϊ 2�¶ȿ�ƬЭ�飬���λ0Ϊ���¶Ȼ���ʪ�ȿ�ƬЭ�顣
*/
void SendRf_Kp8(u8 addr)
{
        u8 TT;//2·�¶ȿ�Ƭ����
        u8 bb;
        u8 add=0;
        u16 i,j,tt,hh;
        u32 th;
        u8 buf[8];
        
        
        TT=addr;
        #if JLY_MODEL==_2_NTC
        TT|=0x80|addr;
        #endif
        
        bb=GetBatt();//��ص���
        
        
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
               tt=0xfff;//12λ
            }
            else
            {
                tt=0x7ff&StateC.Value[j];//��ֵ
                if(0x01&(StateC.FuhaoBit>>j))//����
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


        j=0;//�¶�
        if((StateC.Value[j]==SENIOR_NULL)||(StateC.Value[j]==HAND_STOP))
        {
           tt=0x3fff;
        }
        else
        {
            tt=0x1fff&StateC.Value[j];//��ֵ
            if(0x01&(StateC.FuhaoBit>>j))//����
            {
               tt|=0x2000;
            }
        }
                
        //ʪ��
        if(CH_NUM==2)
        {
            j=1;//�¶�
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
                
         //�¶Ⱥ�ʪ�Ⱥϳ�3���ֽ�
         th=tt;
         th=(th<<10)|hh;
      #endif
                
         
         
         add+=buf[i++]=0xff&(th>>16);
         add+=buf[i++]=0xff&(th>>8);
         add+=buf[i++]=0xff&(th>>0);
                
         //У���
         buf[i++]=add;
         
         //����
         U0Init(115200,1);
         
         VDC33_ON;
         RF_IN_REC;
         delay_ms(55);//����Դ�󣬱�����ʱ  ����50
         
         delay_ms(6);
         
         RfU_SendBuf(buf,i);
         
         //RF_IN_SLP;
         delay_us(500);//������ʱ
         U0Init(9600,0);
         
         //�رյ�Դ
         delay_ms(10);//������ʱ����6
         VDC33_OFF;
         RF_IN_REC;

}

void RfKp1Sec(void)
{
    if(Pt.snpt>0)
    {
      Pt.snpt--;
      if(Pt.snpt==0)//��ʱʱ�䵽�Զ��˳�HHHH״̬
      {
          RF_IN_SLP;
          if(StateA2.WorkState!=WORK_STATE_RECORDING)
          {
             StateA2.WorkState=WORK_STATE_RECORDING;//����
          }
      }
      
      
    }
}



//6ms����
u8 RfKpHave(void)
{
    if(++Pt.RfInvPt>=RF_SEN_TIME)
    {
        //�����
        Pt.Random=TAR;
        Pt.Random+=TBR;
        Pt.Random+=StateC.Adc[3];
        Pt.Random+=StateC.Adc[4];
        Pt.Random+=StateC.Adc[5];
        Pt.Random+=StateC.Adc[6];
        Pt.Random=Pt.Random%RF_RADOM_N;
   
        Pt.RfInvPt=(u16)Pt.Random;//zz  �ӣ�0-1999����0-99�����¿�ʼ
      
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

