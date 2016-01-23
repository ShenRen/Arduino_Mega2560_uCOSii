#include <ucos.h>

//OS_CPU_EXT void  SYSTEM_TickInit (void);

OS_STK TestTask1Stk[OS_TASK_STK_SIZE];
OS_STK TestTask2Stk[OS_TASK_STK_SIZE];
OS_STK TestTask3Stk[OS_TASK_STK_SIZE];
OS_STK BlinkStk[OS_TASK_STK_SIZE];
OS_STK BlinkRedStk[OS_TASK_STK_SIZE];
OS_STK BlinkGreenStk[OS_TASK_STK_SIZE];
OS_STK BlinkYellowStk[OS_TASK_STK_SIZE];

int led = 13,red = 12,green = 11,yellow = 10;

void TestTask1(void * p_arg);
void TestTask2(void * p_arg);
void TestTask3(void * p_arg);
void Blink(void * p_arg);
void BlinkRed(void * p_arg);
void BlinkGreen(void * p_arg);
void BlinkYellow(void * p_arg);

void *TestMsgQAry[10] = {NULL};
OS_EVENT *MsgQSem = NULL;

void setup(){  
  OSInit();  
  OSTaskCreate(TestTask1, (void *)0, &TestTask1Stk[OS_TASK_STK_SIZE - 1], 5); 
  OSTaskCreate(TestTask2, (void *)0, &TestTask2Stk[OS_TASK_STK_SIZE - 1], 3); 
  OSTaskCreate(TestTask3, (void *)0, &TestTask3Stk[OS_TASK_STK_SIZE - 1], 4);   
  OSTaskCreate(Blink, (void *)0, &BlinkStk[OS_TASK_STK_SIZE - 1], 9);  
  OSTaskCreate(BlinkRed, (void *)0, &BlinkRedStk[OS_TASK_STK_SIZE - 1], 6);  
  OSTaskCreate(BlinkGreen, (void *)0, &BlinkGreenStk[OS_TASK_STK_SIZE - 1], 7);  
  OSTaskCreate(BlinkYellow, (void *)0, &BlinkYellowStk[OS_TASK_STK_SIZE - 1], 8);  
    init();
//    SYSTEM_TickInit();
    Serial.begin(9600); 
  OSStart();  
}

void loop(){}

void TestTask1(void * p_arg)
{
  p_arg = p_arg;
  OSSchedLock();
  Serial.println("Task_1 Rdy to run!!!");
  OSSchedUnlock();
  MsgQSem = OSQCreate(&TestMsgQAry[0], 10);
  
  for(;;){
    OSSchedLock();
    Serial.println("Hello World!");
    OSSchedUnlock();
    OSQPost(MsgQSem, (void *)3);
    OSTimeDlyHMSM(0,0,1,0);
//      OSTaskSuspend(2);
//      OSTaskResume(3);
//      OSTaskResume(4);
//      OSTaskResume(5);
  }
}

void TestTask2(void * p_arg)
{
  p_arg = p_arg; 
  OSSchedLock();
  Serial.println("Task_2 Rdy To Run!!!");
  OSSchedUnlock();
  for(;;){
    OSSchedLock();
    Serial.println("Lady GaGa!");
    OSSchedUnlock();
    OSTimeDlyHMSM(0,0,1,0);
//    OSTaskSuspend(3);
  }
}

void TestTask3(void * p_arg)
{
  p_arg = p_arg;  
  INT8U* msg = 0;
  INT8U err = 0;
  OSSchedLock();
  Serial.println("Task_3 Rdy To Run!!!"); 
  OSSchedUnlock();
  for(;;){
    msg = (INT8U *)OSQPend(MsgQSem, 0, &err);
    if( msg == (INT8U *)3){
      OSSchedLock();
      Serial.println("Ono Lisa!");
      OSSchedUnlock();
    }
    else{
      OSSchedLock();
      Serial.println("Msg ERR!!!");  
      OSSchedUnlock();
    }
    OSTimeDlyHMSM(0,0,1,0);
//    OSTaskSuspend(4);
  }
}

void Blink(void * p_arg){
    p_arg = p_arg;  
  // initialize the digital pin as an output.
  int coo = 2 ;
  pinMode(led, OUTPUT);
// the loop routine runs over and over again forever:
for(;;) {
//  DDRB=0xFF;
//    sbi(PORTB,5);
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  OSTimeDlyHMSM(0,0,0,100);
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
  OSTimeDlyHMSM(0,0,0,100);
}
}

void BlinkRed(void * p_arg){
    p_arg = p_arg;  
  // initialize the digital pin as an output.
  pinMode(red, OUTPUT);
// the loop routine runs over and over again forever:
for(;;) {
  digitalWrite(red, HIGH);   // turn the LED on (HIGH is the voltage level)
  OSTimeDlyHMSM(0,0,0,200);
  digitalWrite(red, LOW);    // turn the LED off by making the voltage LOW
  OSTimeDlyHMSM(0,0,0,200);
//    OSTaskSuspend(6);
//  OSTaskResume(2);

}
}

void BlinkGreen(void * p_arg){
    p_arg = p_arg;  
  // initialize the digital pin as an output.
  pinMode(green, OUTPUT);
// the loop routine runs over and over again forever:
for(;;) {
  digitalWrite(green, LOW);    // turn the LED off by making the voltage LOW
  OSTimeDlyHMSM(0,0,0,100);
  digitalWrite(green, HIGH);   // turn the LED on (HIGH is the voltage level)
  OSTimeDlyHMSM(0,0,0,100);

//  OSTaskResume(2);

}
}

void BlinkYellow(void * p_arg){
    p_arg = p_arg;  
  // initialize the digital pin as an output.
  pinMode(yellow, OUTPUT);
// the loop routine runs over and over again forever:
for(;;) {
  digitalWrite(yellow, HIGH);   // turn the LED on (HIGH is the voltage level)
  OSTimeDlyHMSM(0,0,0,500);
  digitalWrite(yellow, LOW);    // turn the LED off by making the voltage LOW
  OSTimeDlyHMSM(0,0,0,500);

//  OSTaskResume(2);

}
}
