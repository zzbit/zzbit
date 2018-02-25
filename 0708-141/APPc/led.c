#include "_config.h"

#if LED_WORK_ENABLE==1
void LedDeel(void)
{
  
  if(Pt.LedPt>0)
  {
    Pt.LedPt--;
    if(Pt.LedPt==0)
    {
      Pt.LedPt=LED_TIME;
      
      //停机或手动停机
      if(StateA2.WorkState==WORK_STATE_STOP)
      {
        if(Flag.HandJustStart==0)
        {
          LED1_ON;
          TB4_START;
        }
      }
      else
      {
         if((StateB.StartMode==START_MODE_HAND)
             &&(StateB.StopMode==STOP_MODE_HAND)
             &&(Flag.HandOff==1))
         {
            LED1_ON;
            TB4_START;
         }
      }
    }
    else if(Pt.LedPt==1)
    {
        LED1_ON;
        TB4_START;
    }
  }

 
}
#endif


