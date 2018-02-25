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




/*****************SPI时序函数******************************************/
u8 WK_SpiByte(u8 cc)
{
       u8 i;
   	for(i=0;i<8;i++)          // 循环8次
   	{
	    if(cc&0x80)
	    {
		//WK_SI=1;
                WK_SI_H;
	    }
	    else
	    {
		//WK_SI=0;   // byte最高位输出到MOSI
                WK_SI_L;
	    }
	    cc<<=1;             // 低一位移位到最高位
	    //WK_CLK=1; 
            WK_CLK_H;
            
            WK_SO_IN;
            
	    
	    //if(WK_SO)               // 拉高SCK，从MOSI读入1位数据，同时从MISO输出1位数据
	    if(WK_SO_PIN&WK_SO_BIT)
            {
		 cc|=0x01;       // 读MISO到byte最低位
	    }
	    //WK_CLK=0;            	// SCK置低
            WK_CLK_L;
   	}
       return(cc);           	// 返回读出的一字节
}



/**************************** Wk2124WriteReg*********************************************/
//函数功能：该函数为写寄存器函数
//参数：port：为子串口的端口号(C0\C1)，当操作的是全局寄存器的时候，该参数仅仅作为全局寄存器地址的BIT4\BIT5
//      reg：作为寄存器地址的低4位（A3\A2\A1\A0）
//      dat：写入寄存器的数据
//
/*************************************************************************/
void Wk2124WriteReg(u8 port,u8 reg,u8 dat)
{	 
	 u8 cmd;
	 cmd=(port-1)<<4|reg;//命令字节构成
	 //WK_CS=0;//拉低cs信号，表示一次读写寄存器的开始
         WK_CS_L;
	 WK_SpiByte(cmd);//发送命令字节
	 WK_SpiByte(dat);//发送数据字节
	 //WK_CS=1;//拉高cs信号，表示一次读写寄存器的结束
         WK_CS_H;
}

/******************************Wk2124ReadReg*******************************************/
//函数功能：该函数为读寄存器函数
//参数：port：为子串口的端口号(C0\C1)，当操作的是全局寄存器的时候，该参数仅仅作为全局寄存器地址的BIT4\BIT5
//      reg：作为寄存器地址的低4位（A3\A2\A1\A0）
//      dat：写入寄存器的数据
//
/*************************************************************************/

u8 Wk2124ReadReg(u8 port,u8 reg)
{	 
	 u8 cmd,ret;
	 cmd=0x40|(port-1)<<4|reg;//命令字节构成
	 //WK_CS=0;//拉低cs信号，表示一次读写寄存器的开始
         WK_CS_L;
	 WK_SpiByte(cmd);//发送命令字节
	 ret=WK_SpiByte(0);//发送0.接收WK2124返回的寄存器值
	 //WK_CS=1;//拉高cs信号，表示一次读写寄存器的结束
         WK_CS_H;
     return ret;
}

/**************************** Wk2124WriteFifo*********************************************/
//函数功能：该函数为写FIFO函数，通过该函数写入的数据会直接进入子串口的发送FIFO，然后通过TX引脚发送
//参数：port：为子串口的端口号(C0\C1)
//      *wbuf:写入数据部分
//      len：  写入数据长度
//
/*************************************************************************/
void Wk2124WriteFifo(u8 port,u8 *wbuf,u16 len)
{  
	u8 cmd;
	u16 count;
	cmd=0x80|(port-1)<<4;//命令字节构成
	//WK_CS=0;//拉低cs信号，表示一次读写操作的开始
        WK_CS_L;
	WK_SpiByte(cmd);//发送命令字节
	for(count=0;count<len;count++)
	   WK_SpiByte(*(wbuf+count));//发送数据字节  
	//WK_CS=1;//拉高cs信号，表示一次读写寄存器的结束
        WK_CS_H;
    	
}

/**************************** Wk2124ReadFifo*********************************************/
//函数功能：该函数为读FIFO函数，通过该函数可以一次读出多个接收FIFO中的数据，最多256个字节
//参数：port：为子串口的端口号(C0\C1)
//      *rbuf:写入数据部分
//      len：  写入数据长度
//
/*************************************************************************/
void Wk2124ReadFifo(u8 port,u8 *rbuf,u16 len)
{  
	u8 cmd;
	u16 count;
	cmd=0xc0|(port-1)<<4;//命令字节构成
	//WK_CS=0;//拉低cs信号，表示一次读写操作的开始
        WK_CS_L;
	WK_SpiByte(cmd);//发送命令字节
	for(count=0;count<len;count++)
	   *(rbuf+count)=WK_SpiByte(0);//读取接收fifo中的数据  
	//WK_CS=1;//拉高cs信号，表示一次读操作结束
        WK_CS_H;
    	
}


/******************************Wk2124Init*******************************************/
//函数功能：本函数主要会初始化一些芯片基本寄存器；
/*********************************************************************************/
void Wk2124Init(u8 port)
{
    u8 gena,grst,gier,sier,scr;
	//使能子串口时钟
    gena=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
                case 1://使能子串口1的时钟
              gena|=WK2XXX_UT1EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://使能子串口2的时钟
              gena|=WK2XXX_UT2EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://使能子串口3的时钟
              gena|=WK2XXX_UT3EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://使能子串口4的时钟
              gena|=WK2XXX_UT4EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
	//软件复位子串口
	grst=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://软件复位子串口1
              grst|=WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://软件复位子串口2
              grst|=WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://软件复位子串口3
              grst|=WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://软件复位子串口4
             grst|=WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
  //使能子串口中断，包括子串口总中断和子串口内部的接收中断，和设置中断触点
	gier=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GIER); 
	switch (port)
    {
          case 1://软件复位子串口1
              gier|=WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		  case 2://软件复位子串口2
              gier|=WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 3://软件复位子串口3
              gier|=WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
		   case 4://软件复位子串口4
              gier|=WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GIER,gier);
              break;
	 }	 
	 //使能子串口接收触点中断和超时中断
	 sier=Wk2124ReadReg(port,WK2XXX_SIER); 
	 sier |= WK2XXX_RFTRIG_IEN|WK2XXX_RXOUT_IEN;
	 Wk2124WriteReg(port,WK2XXX_SIER,sier);
	 // 初始化FIFO和设置固定中断触点
	 Wk2124WriteReg(port,WK2XXX_FCR,0XFF);
	 //设置任意中断触点，如果下面的设置有效，那么上面FCR寄存器中断的固定中断触点将失效
	 Wk2124WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
	 Wk2124WriteReg(port,WK2XXX_RFTL,0X40);//设置接收触点为64个字节
	 Wk2124WriteReg(port,WK2XXX_TFTL,0X10);//设置发送触点为16个字节
	 Wk2124WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
	 //使能子串口的发送和接收使能
	 scr=Wk2124ReadReg(port,WK2XXX_SCR); 
	 scr|=WK2XXX_TXEN|WK2XXX_RXEN;
	 Wk2124WriteReg(port,WK2XXX_SCR,scr);
}

/******************************Wk2124Close*******************************************/
//函数功能：本函数会关闭当前子串口，和复位初始值；
/*********************************************************************************/

void Wk2124Close(u8 port)
{
    u8 gena,grst;
	//复位子串口
	grst=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GRST); 
	switch (port)
    {
          case 1://软件复位子串口1
              grst&=~WK2XXX_UT1RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		  case 2://软件复位子串口2
              grst&=~WK2XXX_UT2RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 3://软件复位子串口3
              grst&=~WK2XXX_UT3RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
		   case 4://软件复位子串口4
              grst&=~WK2XXX_UT4RST;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GRST,grst);
              break;
	 }	
	//关闭子串口时钟
    gena=Wk2124ReadReg(WK2XXX_GPORT,WK2XXX_GENA); 
	switch (port)
    {
          case 1://使能子串口1的时钟
              gena&=~WK2XXX_UT1EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		  case 2://使能子串口2的时钟
              gena&=~WK2XXX_UT2EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 3://使能子串口3的时钟
              gena&=~WK2XXX_UT3EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
		   case 4://使能子串口4的时钟
              gena&=~WK2XXX_UT4EN;
		      Wk2124WriteReg(WK2XXX_GPORT,WK2XXX_GENA,gena);
              break;
	 }	
}






/**************************Wk2124SetBaud*******************************************************/
//函数功能：设置子串口波特率函数、此函数中波特率的匹配值是根据11.0592Mhz下的外部晶振计算的
// port:子串口号
// baud:波特率大小.波特率表示方式，
//
/**************************Wk2124SetBaud*******************************************************/
void Wk2124SetBaud(u8 port,u32 baud)
{  
	u8 baud1,baud0,pres,scr;
	//如下波特率相应的寄存器值，是在外部时钟为12M的情况下计算所得，如果使用其他晶振，需要重新计算
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
	//关掉子串口收发使能
	scr=Wk2124ReadReg(port,WK2XXX_SCR); 
	Wk2124WriteReg(port,WK2XXX_SCR,0);
	//设置波特率相关寄存器
	Wk2124WriteReg(port,WK2XXX_SPAGE,1);//切换到page1
	Wk2124WriteReg(port,WK2XXX_BAUD1,baud1);
	Wk2124WriteReg(port,WK2XXX_BAUD0,baud0);
	Wk2124WriteReg(port,WK2XXX_PRES,pres);
	Wk2124WriteReg(port,WK2XXX_SPAGE,0);//切换到page0 
	//使能子串口收发使能
	Wk2124WriteReg(port,WK2XXX_SCR,scr);
	
	
}
/*****************************Wk2124SendBuf****************************************/
//本函数为子串口发送数据的函数，发送数据到子串口的FIFO.然后通过再发送
//参数说明：port：子串口端口号
//          *sendbuf:需要发送的数据buf
//          len：需要发送数据的长度
// 函数返回值：实际成功发送的数据
//说明：调用此函数只是把数据写入子串口的发送FIFO，然后再发送。1、首先确认子串口的发送FIFO有多少数据，根据具体情况、
//确定写入FIFO数据的个数，
/*********************************************************************/
u16 Wk2124SendBuf(u8 port,u8 *sendbuf,u16 len)
{
	 u16 ret,tfcnt,sendlen;
	 u8  fsr;
	  
	 fsr=Wk2124ReadReg(port,WK2XXX_FSR);
	 if(~fsr&WK2XXX_TFULL )//子串口发送FIFO未满
	 {

	     tfcnt=Wk2124ReadReg(port,WK2XXX_TFCNT);//读子串口发送fifo中数据个数
		 sendlen=256-tfcnt;//FIFO能写入的最多字节数
		 
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
//本函数为子串口接收数据函数
//参数说明：port：子串口端口号
//          *getbuf:接收到的数据buf
// 函数返回值：实际接收到的数据个数
/*********************************************************************/
u16 Wk2124GetBuf(u8 port,u8 *getbuf)
{
	u16 ret=0,rfcnt;
	u8 fsr;
	
	fsr=Wk2124ReadReg(port,WK2XXX_FSR);
	if(fsr&WK2XXX_RDAT )//子串口接收FIFO未空
	  {
	        rfcnt=Wk2124ReadReg(port,WK2XXX_RFCNT);//读子串口接收FIFO中数据个数
		 if(rfcnt==0)//当RFCNT寄存器为0的时候，有两种情况，可能是256或者是0，这个时候通过FSR来判断，如果FSR显示接收FIFO不为空，就为256个字节
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
            
              StateC.GpsRecWait=GPS_REC_JG_TIME;//MCU 接收GPS数据最小间隔
              
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
        
        
        WK_IRQ_SET_INT;//开中断
	

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













