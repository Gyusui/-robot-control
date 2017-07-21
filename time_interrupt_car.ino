#include <arduino.h>
#include <MsTimer2.h>  
//*****************************************************
//モータのdefine
//*****************************************************
#define ML_PWM_PIN      10     // 左のモータ接続ピン
#define ML_IN1_PIN      4
#define ML_IN2_PIN      5
#define MR_PWM_PIN      6     // 右のモータ接続ピン
#define MR_IN1_PIN      8
#define MR_IN2_PIN      7
#define LEFT     0            //エンコーダの接続ピン
#define RIGHT    1
//#define Pi      3.14
unsigned long old_time = 0;
unsigned long time = 0;      //時間タグ
unsigned long dt=10;
int leftCount = 0;           //カウンター割り込みのステップ数初期化
int rightCount = 0;   
int time_Lcount = 0;         //タイマー割り込みのステップ数初期化
int time_Rcount = 0;         
double R = 7.5;              //ロボットの半径
double r = 4.25;             //車輪の半径
double v1=0,v2=0,v=0;        //ロボット両車輪の速度、ロボット自体の速度
double degreeCur = 0;        //当前角度
//float w = 0;                //ロボットの角速度
double wl=0,wr = 0;               //左車輪,右車輪の目標角速度
double w1=0,w2 = 0;               //左車輪,右車輪の現在目標角速度
int rpm1=0,rpm2=0;          //左右車輪の回転数
int val_left = 225,val_right = 225;  //PWM50％   
int old_time_Lcount=0;
int old_time_Rcount=0;
void setup() {
  Serial.begin(9600);
  pinMode(ML_IN1_PIN,OUTPUT);// モータのセット
  pinMode(ML_IN2_PIN,OUTPUT);
  pinMode(MR_IN1_PIN,OUTPUT);
  pinMode(MR_IN2_PIN,OUTPUT);
  MsTimer2::set(dt,ReadCount);     // 10ms毎にReadCount( )割込み関数を呼び出す様に設定
  MsTimer2::start();      
}
void loop() {
    attachInterrupt(0,LeftWheel,FALLING);
    attachInterrupt(1,RightWheel,FALLING);//割り込みを始め直す
    //rpm1 = (float)leftCount*60/20;  //ロタリエンコーダで記述されたパルスを回転数に変更する、エンコーダは20穴
    //rpm2 = (float)rightCount*60/20;//60をかけるかどうかわからない
    rpm1 = (float)old_time_Lcount/20*10;
    rpm2 = (float)old_time_Rcount/20*10;
    w1 = PI*2*rpm1;        //w1,w2の現在値、
    w2 = PI*2*rpm2;
    turnDegree(90);
    //delay(500);
}
void forward()
{
   digitalWrite(ML_IN1_PIN,HIGH);
   digitalWrite(ML_IN2_PIN,LOW);     //左の車輪は前に回る
   analogWrite(ML_PWM_PIN,val_left);  //analogWrite()は0~255
   digitalWrite(MR_IN1_PIN,HIGH);    //右の車輪は前に回る
   digitalWrite(MR_IN2_PIN,LOW);
   analogWrite(MR_PWM_PIN,val_right);
}
void stopCar()
{
  digitalWrite(ML_IN1_PIN,LOW);
  digitalWrite(ML_IN2_PIN,LOW);
  digitalWrite(MR_IN1_PIN,LOW);
  digitalWrite(MR_IN2_PIN,LOW);
  delay(4000);
}
void turnRight()
{
   digitalWrite(ML_IN1_PIN,HIGH);
   digitalWrite(ML_IN2_PIN,LOW);
   analogWrite(ML_PWM_PIN,val_left);
   digitalWrite(MR_IN1_PIN,LOW);    
   digitalWrite(MR_IN2_PIN,HIGH);
   analogWrite(MR_PWM_PIN,val_right);
}
void turnDegree(int degree)
{
  double Kp = 0.5;
  double k = 0.01;
  v = 0;
  double thetaD = degree*PI/180;
  double w = Kp*(thetaD-degreeCur);
  wl = (2*v+w*(2*R))/2;              //目標角速度
  wr = -(2*v-w*(2*R))/2;
  if(thetaD > degreeCur)
  {
     turnRight();
     val_left =k*(wl+Kp*(wl-w1));
     val_right = k*(wr+Kp*(wr-w2));
     delay(500);
     //leftCount = 0;
     //rightCount = 0;
  }
   else
  {
   stopCar();
   Serial.print("stop");
   //leftCount = 0;
   //rightCount = 0;
  }
   degreeCur=degreeCur+w*0.1;
   Serial.print("degreeCur = ");
   Serial.println(degreeCur);
   Serial.print("degree = ");
   Serial.println(degree);
   Serial.print("w = ");
   Serial.println(w);
   Serial.print("dt = ");
   Serial.println(dt);
}
void Lspeed_up()
{
    val_left=val_left+5;
    if(val_left>255)
    {
      val_left=127;
    }
    delay(50);
}
void Rspeed_up()
{
    val_right=val_right+5;
     if(val_left>255)
    {
      val_left=127;
    }
    delay(50);
}
void Lslow_down()
{
    val_left=val_left-5;
     if(val_left<64)
    {
      val_left=127;
    }
    delay(50);
}
void Rslow_down()
{
    val_right=val_right-5;
    if(val_left<64)
    {
      val_left=127;
    }
    delay(50);
}
void LeftWheel()
{
  leftCount++;
}
void RightWheel()
{
  rightCount++;
}
void ReadCount()                           //現在のパルス数を読む
{
  int new_time_Lcount = leftCount;
  int new_time_Rcount = rightCount;
  old_time_Lcount = new_time_Lcount - old_time_Lcount;
  old_time_Rcount = new_time_Rcount - old_time_Rcount;
  
  /*Serial.print("old_time_Rcount = ");
  Serial.print(old_time_Rcount);
  Serial.print("old_time_Rcount = ");
  Serial.println(old_time_Rcount);*/
}
