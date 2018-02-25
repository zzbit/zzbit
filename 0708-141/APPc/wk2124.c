#include "_config.h"

#if WK2124_EN==1


#define   WK_CLK_BIT     BIT3
#define   WK_SO_BIT      BIT2
#define   WK_SI_BIT      BIT1
#define   WK_CS_BIT      BIT0

#define   WK_PSEL        P5SEL
#define   WK_PDIR        P5DIR
#define   WK_POUT        P5OUT
#define   WK_PREN        P5REN

#define   WK_CLK_L       WK_PDIR|=WK_CLK_BIT;WK_PREN&=~WK_CLK_BIT;WK_POUT&=~WK_CLK_BIT;
#define   WK_CLK_H       WK_PDIR|=WK_CLK_BIT;WK_PREN|=WK_CLK_BIT;WK_POUT|=WK_CLK_BIT;


#define   WK_SO_L        WK_PDIR|=WK_SO_BIT;WK_PREN&=~WK_SO_BIT;WK_POUT&=~WK_SO_BIT;
#define   WK_SO_H        WK_PDIR|=WK_SO_BIT;WK_PREN|=WK_SO_BIT;WK_POUT|=WK_SO_BIT;

#define   WK_SI_L        WK_PDIR|=WK_SI_BIT;WK_PREN&=~WK_SI_BIT;WK_POUT&=~WK_SI_BIT;
#define   WK_SI_H        WK_PDIR|=WK_SI_BIT;WK_PREN|=WK_SI_BIT;WK_POUT|=WK_SI_BIT;

#define   WK_CS_L        WK_PDIR|=WK_CS_BIT;WK_PREN&=~WK_CS_BIT;WK_POUT&=~WK_CS_BIT;
#define   WK_CS_H        WK_PDIR|=WK_CS_BIT;WK_PREN|=WK_CS_BIT;WK_POUT|=WK_CS_BIT;


//SO  5.2
#define   WK_SO_PIN      P5IN
#define   WK_SO_DIR      P5DIR

#define   WK_SO_IN       WK_SO_DIR&=~WK_SO_BIT;



//IRQ  1.0
#define WK_IRQ_BIT        BIT0
#define WK_IRQ_SET_INT    P1SEL&=~WK_IRQ_BIT;P1DIR&=~WK_IRQ_BIT;P1IES|=WK_IRQ_BIT;P1IFG&=~WK_IRQ_BIT;P1IE|=WK_IRQ_BIT;
#define WK_IRQ_CLR_INT    P1IE&=~WK_IRQ_BIT;




/*****************SPIʱ����******************************************/
u8 WK_SpiByte(u8 cc)
{
       u8 i;
   	for(i=0;i<8;i++)          // ѭ��8��
   	{
	    if(cc&0x80)
	    {
		//WK_SI=1;
                WK_SI_H;
	    }
	    else
	    {
		//WK_SI=0;   // byte���λ�����MOSI
                WK_SI_L;
	    }
	    cc<<=1;             // ��һλ��λ�����λ
	    //WK_CLK=1; 
            WK_CLK_H;
            
            WK_SO_IN;
            
	    
	    //if(WK_SO)               // ����SCK����MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
	    if(WK_SO_PIN&WK_SO_BIT)
            {
		 cc|=0x01;       // ��MISO��byte���λ
	    }
	    //WK_CLK=0;            	// SCK�õ�
            WK_CLK_L;
   	}
       return(cc);           	// ���ض�����һ�ֽ�
}



/**************************** Wk2124WriteReg*********************************************/
//�������ܣ��ú���Ϊд�Ĵ�������
//������port��Ϊ�Ӵ��ڵĶ˿ں�(C0\C1)������������ȫ�ּĴ�����ʱ�򣬸ò���������Ϊȫ�ּĴ�����ַ��BIT4\BIT5
//      reg����Ϊ�Ĵ�����ַ�ĵ�4λ��A3\A2\A1\A0��
//      dat��д��Ĵ���������
//
/*************************************************************************/
void Wk2124WriteReg(u8 port,u8 reg,u8 dat)
{	 
	 u8 cmd;
	 cmd=(port-1)<<4|reg;//�����ֽڹ���
	 //WK_CS=0;//����cs�źţ���ʾһ�ζ�д�Ĵ����Ŀ�ʼ
         WK_CS_L;
	 WK_SpiByte(cmd);//���������ֽ�
	 WK_SpiByte(dat);//���������ֽ�
	 //WK_CS=1;//����cs�źţ���ʾһ�ζ�д�Ĵ����Ľ���
         WK_CS_H;
}

/******************************Wk2124ReadReg*******************************************/
//�������ܣ��ú���Ϊ���Ĵ�������
//������port��Ϊ�Ӵ��ڵĶ˿ں�(C0\C1)������������ȫ�ּĴ�����ʱ�򣬸ò���������Ϊȫ�ּĴ�����ַ��BIT4\BIT5
//      reg����Ϊ�Ĵ�����ַ�ĵ�4λ��A3\A2\A1\A0��
//      dat��д��Ĵ���������
//
/*************************************************************************/

u8 Wk2124ReadReg(u8 port,u8 reg)
{	 
	 u8 cmd,ret;
	 cmd=0x40|(port-1)<<4|reg;//�����ֽڹ���
	 //WK_CS=0;//����cs�źţ���ʾһ�ζ�д�Ĵ����Ŀ�ʼ
         WK_CS_L;
	 WK_SpiByte(cmd);//���������ֽ�
	 ret=WK_SpiByte(0);//����0.����WK2124���صļĴ���ֵ
	 //WK_CS=1;//����cs�źţ���ʾһ�ζ�д�Ĵ����Ľ���
         WK_CS_H;
     return ret;
}

/**************************** Wk2124WriteFifo*********************************************/
//�������ܣ��ú���ΪдFIFO������ͨ���ú���д������ݻ�ֱ�ӽ����Ӵ��ڵķ���FIFO��Ȼ��ͨ��TX���ŷ���
//������port��Ϊ�Ӵ��ڵĶ˿ں�(C0\C1)
//      *wbuf:д�����ݲ���
//      len��  д�����ݳ���
//
/*************************************************************************/
void Wk2124WriteFifo(u8 port,u8 *wbuf,u16 len)
{  
	u8 cmd;
	u16 count;
	cmd=0x80|(port-1)<<4;//�����ֽڹ���
	//WK_CS=0;//����cs�źţ���ʾһ�ζ�д�����Ŀ�ʼ
        WK_CS_L;
	WK_SpiByte(cmd);//���������ֽ�
	for(count=0;count<len;count++)
	   WK_SpiByte(*(wbuf+count));//���������ֽ�  
	//WK_CS=1;//����cs�źţ���ʾһ�ζ�д�Ĵ����Ľ���
        WK_CS_H;
    	
}

/**************************** Wk2124ReadFifo*********************************************/
//�������ܣ��ú���Ϊ��FIFO������ͨ���ú�������һ�ζ����������FIFO�е����ݣ����256���ֽ�
//������port��Ϊ�Ӵ��ڵĶ˿ں�(C0\C1)
//      *rbuf:д�����ݲ���
//      len��  д�����ݳ���
//
/*************************************************************************/
void Wk2124ReadFifo(u8 port,u8 *rbuf,u16 len)
{  
	u8 cmd;
	u16 count;
	cmd=0xc0|(port-1)<<4;//�����ֽڹ���
	//WK_CS=0;//����cs�źţ���ʾһ�ζ�д�����Ŀ�ʼ
        WK_CS_L;
	WK_SpiByte(cmd);//���������ֽ�
	for(count=0;count<len;count++)
	   *(rbuf+count)=WK_SpiByte(0);//��ȡ����fifo�е�����  
	//WK_CS=1;//����cs�źţ���ʾһ�ζ���������
        WK_CS_H;
    	
}


/******************************Wk2124Init*******************************************/
//�������ܣ���������Ҫ���ʼ��һЩоƬ�����Ĵ�����
/*********************************************************************************/
void Wk2124Init(u8 port)
{
    u8 gena,grst,gier,sier,scr;
	//ʹ���Ӵ���ʱ��
    gena=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
                case 1://ʹ���Ӵ���1��ʱ��
              gena|=WK2XXX_UT1EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://ʹ���Ӵ���2��ʱ��
              gena|=WK2XXX_UT2EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://ʹ���Ӵ���3��ʱ��
              gena|=WK2XXX_UT3EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://ʹ���Ӵ���4��ʱ��
              gena|=WK2XXX_UT4EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
	//�����λ�Ӵ���
	grst=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              grst|=WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://�����λ�Ӵ���2
              grst|=WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://�����λ�Ӵ���3
              grst|=WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://�����λ�Ӵ���4
             grst|=WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
  //ʹ���Ӵ����жϣ������Ӵ������жϺ��Ӵ����ڲ��Ľ����жϣ��������жϴ���
	gier=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GIER); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              gier|=WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		  case 2://�����λ�Ӵ���2
              gier|=WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 3://�����λ�Ӵ���3
              gier|=WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 4://�����λ�Ӵ���4
              gier|=WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
	 }	 
	 //ʹ���Ӵ��ڽ��մ����жϺͳ�ʱ�ж�
	 sier=Wk2124ReadReg(port,WK2XXX_SIER); 
	 sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
	 Wk2124WriteReg(port,WK2XXX_SIER,sier);
	 // ��ʼ��FIFO�����ù̶��жϴ���
	 Wk2124WriteReg(port,WK2XXX_FCR,0XFF);
	 //���������жϴ��㣬��������������Ч����ô����FCR�Ĵ����жϵĹ̶��жϴ��㽫ʧЧ
	 Wk2124WriteReg(port,WK2XXX_SPAGE,1);//�л���page1
	 Wk2124WriteReg(port,WK2XXX_RFTL,0X40);//���ý��մ���Ϊ64���ֽ�
	 Wk2124WriteReg(port,WK2XXX_TFTL,0X10);//���÷��ʹ���Ϊ16���ֽ�
	 Wk2124WriteReg(port,WK2XXX_SPAGE,0);//�л���page0 
	 //ʹ���Ӵ��ڵķ��ͺͽ���ʹ��
	 scr=Wk2124ReadReg(port,WK2XXX_SCR); 
	 scr|=WK2XXX_TXEN|WK2XXX_RXEN;
	 Wk2124WriteReg(port,WK2XXX_SCR,scr);
}

/******************************Wk2124Close*******************************************/
//�������ܣ���������رյ�ǰ�Ӵ��ڣ��͸�λ��ʼֵ��
/*********************************************************************************/

void Wk2124Close(u8 port)
{
    u8 gena,grst;
	//��λ�Ӵ���
	grst=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://�����λ�Ӵ���1
              grst&=~WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://�����λ�Ӵ���2
              grst&=~WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://�����λ�Ӵ���3
              grst&=~WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://�����λ�Ӵ���4
              grst&=~WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
	//�ر��Ӵ���ʱ��
    gena=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://ʹ���Ӵ���1��ʱ��
              gena&=~WK2XXX_UT1EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://ʹ���Ӵ���2��ʱ��
              gena&=~WK2XXX_UT2EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://ʹ���Ӵ���3��ʱ��
              gena&=~WK2XXX_UT3EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://ʹ���Ӵ���4��ʱ��
              gena&=~WK2XXX_UT4EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
}






/**************************Wk2124SetBaud*******************************************************/
//�������ܣ������Ӵ��ڲ����ʺ������˺����в����ʵ�ƥ��ֵ�Ǹ���11.0592Mhz�µ��ⲿ��������
// port:�Ӵ��ں�
// baud:�����ʴ�С.�����ʱ�ʾ��ʽ��
//
/**************************Wk2124SetBaud*******************************************************/
void Wk2124SetBaud(u8 port,u32 baud)
{  
	u8 baud1,baud0,pres,scr;
	//���²�������Ӧ�ļĴ���ֵ�������ⲿʱ��Ϊ12M������¼������ã����ʹ������������Ҫ���¼���
	switch (baud) 
	{
            case 9600:
                            baud1=0x00;
                            baud0=0x4d;
                            pres=0x01;
                             
                            break;
    
            case 115200:
                            baud1=0x00;
                            baud0=0x05;
                            pres=0x05;
                            break;
    
            default:
                            baud1=0x00;
                            baud0=0x00;
                            pres=0;
        }
	//�ص��Ӵ����շ�ʹ��
	scr=Wk2124ReadReg(port,WK2XXX_SCR); 
	Wk2124WriteReg(port,WK2XXX_SCR,0);
	//���ò�������ؼĴ���
	Wk2124WriteReg(port,WK2XXX_SPAGE,1);//�л���page1
	Wk2124WriteReg(port,WK2XXX_BAUD1,baud1);
	Wk2124WriteReg(port,WK2XXX_BAUD0,baud0);
	Wk2124WriteReg(port,WK2XXX_PRES,pres);
	Wk2124WriteReg(port,WK2XXX_SPAGE,0);//�л���page0 
	//ʹ���Ӵ����շ�ʹ��
	Wk2124WriteReg(port,WK2XXX_SCR,scr);
	
	
}
/*****************************Wk2124SendBuf****************************************/
//������Ϊ�Ӵ��ڷ������ݵĺ������������ݵ��Ӵ��ڵ�FIFO.Ȼ��ͨ���ٷ���
//����˵����port���Ӵ��ڶ˿ں�
//          *sendbuf:��Ҫ���͵�����buf
//          len����Ҫ�������ݵĳ���
// ��������ֵ��ʵ�ʳɹ����͵�����
//˵�������ô˺���ֻ�ǰ�����д���Ӵ��ڵķ���FIFO��Ȼ���ٷ��͡�1������ȷ���Ӵ��ڵķ���FIFO�ж������ݣ����ݾ��������
//ȷ��д��FIFO���ݵĸ�����
/*********************************************************************/
u16 Wk2124SendBuf(u8 port,u8 *sendbuf,u16 len)
{
	 u16 ret,tfcnt,sendlen;
	 u8  fsr;
	  
	 fsr=Wk2124ReadReg(port,WK2XXX_FSR);
	 if(~fsr&WK2XXX_TFULL )//�Ӵ��ڷ���FIFOδ��
	 {

	     tfcnt=Wk2124ReadReg(port,WK2XXX_TFCNT);//���Ӵ��ڷ���fifo�����ݸ���
		 sendlen=256-tfcnt;//FIFO��д�������ֽ���
		 
		 if(sendlen<len)
		 {
			ret=sendlen; 
			Wk2124WriteFifo(port,sendbuf,sendlen);
		 }
		 else
		 {
			 Wk2124WriteFifo(port,sendbuf,len);
			 ret=len;
		 }
	  }
	 
	 return ret;
}

/*****************************Wk2124GetBuf****************************************/
//������Ϊ�Ӵ��ڽ������ݺ���
//����˵����port���Ӵ��ڶ˿ں�
//          *getbuf:���յ�������buf
// ��������ֵ��ʵ�ʽ��յ������ݸ���
/*********************************************************************/
u16 Wk2124GetBuf(u8 port,u8 *getbuf)
{
	u16 ret=0,rfcnt;
	u8 fsr;
	
	fsr=Wk2124ReadReg(port,WK2XXX_FSR);
	if(fsr&WK2XXX_RDAT )//�Ӵ��ڽ���FIFOδ��
	  {
	        rfcnt=Wk2124ReadReg(port,WK2XXX_RFCNT);//���Ӵ��ڽ���FIFO�����ݸ���
		 if(rfcnt==0)//��RFCNT�Ĵ���Ϊ0��ʱ�������������������256������0�����ʱ��ͨ��FSR���жϣ����FSR��ʾ����FIFO��Ϊ�գ���Ϊ256���ֽ�
		 {rfcnt=256;}
		 Wk2124ReadFifo(port,getbuf,rfcnt);
		 ret=rfcnt;
	   }
	 return ret;	
}

void WK_IRQ_Do(void)
{
      u8 fr;
      u8 buf[256];
      u16 len;
      
      fr=Wk2124ReadReg(4,WK2XXX_GIFR);
      
      if(fr&BIT0)//U1
      {
        
      }
      
      if(fr&BIT1)
      {
        
      }
      
      if(fr&BIT2)
      {
        
      }
      
      //if(fr&BIT3)
      {
        

  
          if(StateC.GpsRecWait==0)
          {
            
              StateC.GpsRecWait=GPS_REC_JG_TIME;//MCU ����GPS������С���
              
              len=Wk2124GetBuf(1,buf);
    
              if(len)
              {
                
                Wk2124SendBuf(2,buf,len);

        
              }
          }
          
      }
      
      NOP;
  
}

//1:GPS  2:PRT  3:BT  4:NOP
void WK_Ini(void)
{
	Wk2124Init(1);
	Wk2124SetBaud(1,9600);

	Wk2124Init(2);
	Wk2124SetBaud(2,9600);
        
	Wk2124Init(3);
	Wk2124SetBaud(3,9600);
  
	Wk2124Init(4);
	Wk2124SetBaud(4,9600);
        
        
        WK_IRQ_SET_INT;//���ж�
	

        SHDN_H;
	Wk2124SendBuf(2,"WK Uart \r\n",10);
}


void WK_Test(void)
{
	
	//U1SendStrPC("WK4\r\n");
        //SHDN_H;
	//Wk2124SendBuf(4,"WK\r\n",4);
        
        
        WK_IRQ_Do();
        
        /*
                u8 buf[10];
                        SHDN_H;
                buf[0]='2';buf[0]='2';
                Wk2124SendBuf(2,buf,2);
                
                Wk2124SendBuf(4,buf,2);
        */
        
	/*
	static u8 buf[10];
	static u8 len;
	
	len=Wk2124GetBuf(1,buf);
	if(len)
	{
         //Wk2124SendBuf(1,"len12345  ",10);		
	  Wk2124SendBuf(1,buf,len);
	  
		
	}
        */
}








#endif













