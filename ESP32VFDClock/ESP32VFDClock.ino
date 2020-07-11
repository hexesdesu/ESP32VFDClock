#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               // Include NTPClient library
#include <TimeLib.h>                 // Include Arduino time library

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "cn.ntp.org.cn", 8*3600, 60000);
// 'cn.ntp.org.cn' is used (default server) with +1 hour offset (3600 seconds) 60 seconds (60000 milliseconds) update interval
const char *ssid     = "<SSID>";
const char *password = "<Password>";

uint8_t din   = 33; // DA
uint8_t clk   = 23; // CK
uint8_t cs    = 19; // CS
uint8_t Reset = 22; // RS

char *str_time = "00:00:00";
String format_time = "00:00:00";

void write_6302(unsigned char w_data)
{
  unsigned char i;
  for (i = 0; i < 8; i++)
  {
    digitalWrite(clk, LOW);
    if ( (w_data & 0x01) == 0x01)
    {
      digitalWrite(din, HIGH);
    }
    else
    {
      digitalWrite(din, LOW);
    }
    w_data >>= 1;
    digitalWrite(clk, HIGH);
  }
  Serial.printf("write_6302");
}

void VFD_cmd(unsigned char command)
{
  digitalWrite(cs, LOW);
  write_6302(command);
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
  Serial.printf("S1201_show\n");
}

void S1201_show(void)
{
  digitalWrite(cs, LOW);//开始传输
  write_6302(0xe8);     //地址寄存器起始位置
  digitalWrite(cs, HIGH); //停止传输
  Serial.printf("VFD_cmd\n");
}

void VFD_init()
{
  //SET HOW MANY digtal numbers
  digitalWrite(cs, LOW);
  write_6302(0xe0);
  delayMicroseconds(5);
  write_6302(0x07);//8 digtal
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);

  //set bright
  digitalWrite(cs, LOW);
  write_6302(0xe4);
  delayMicroseconds(5);
  write_6302(0x33);//leve 255 max
  digitalWrite(cs, HIGH);
  delayMicroseconds(5);
  Serial.printf("VFD_init\n");
}

/******************************
  在指定位置打印一个字符(用户自定义,所有CG-ROM中的)
  x:0~11;chr:要显示的字符编码
*******************************/
void S1201_WriteOneChar(unsigned char x, unsigned char chr)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  write_6302(chr + 0x30);
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();
  Serial.printf("S1201_WriteOneChar\n");
}
/******************************
  在指定位置打印字符串
  (仅适用于英文,标点,数字)
  x:0~11;str:要显示的字符串
*******************************/
void S1201_WriteStr(unsigned char x, char *str)
{
  digitalWrite(cs, LOW);  //开始传输
  write_6302(0x20 + x); //地址寄存器起始位置
  while (*str)
  {
    write_6302(*str); //ascii与对应字符表转换
    str++;
  }
  digitalWrite(cs, HIGH); //停止传输
  S1201_show();
  Serial.printf("S1201_WriteStr\n");
}

void setup(){
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  Serial.print("Connecting.");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
 
  timeClient.begin();

  pinMode(clk, OUTPUT);
  pinMode(din, OUTPUT);
  pinMode(cs, OUTPUT);
  pinMode(Reset, OUTPUT);
  digitalWrite(Reset, LOW);
  delayMicroseconds(5);
  digitalWrite(Reset, HIGH);
  VFD_init();
  S1201_WriteStr(0, "________");
  delay(200);
  S1201_WriteStr(0, "_______@");
  delay(200);
  S1201_WriteStr(0, "______@H");
  delay(200);
  S1201_WriteStr(0, "_____@HE");
  delay(200);
  S1201_WriteStr(0, "____@HEX");
  delay(200);
  S1201_WriteStr(0, "___@HEXE");
  delay(200);
  S1201_WriteStr(0, "__@HEXES");
  delay(200);
  S1201_WriteStr(0, "_@HEXESD");
  delay(200);
  S1201_WriteStr(0, "@HEXESDE");
  delay(200);
  S1201_WriteStr(0, "HEXESDES");
  delay(200);
  S1201_WriteStr(0, "EXESDESU");
  delay(200);
  S1201_WriteStr(0, "XESDESU_");
  delay(200);
  S1201_WriteStr(0, "ESDESU__");
  delay(200);
  S1201_WriteStr(0, "SDESU___");
  delay(200);
  S1201_WriteStr(0, "DESU____");
  delay(200);
  S1201_WriteStr(0, "ESU_____");
  delay(200);
  S1201_WriteStr(0, "SU______");
  delay(200);
  S1201_WriteStr(0, "U_______");
  delay(200);
  S1201_WriteStr(0, "________");
  delay(1000);
  Serial.println("Initialized");
}

void loop()
{
  timeClient.update();
  format_time = timeClient.getFormattedTime();
  char *str_time = &format_time[0]; 
  S1201_WriteStr(0, str_time);
  Serial.println(timeClient.getFormattedTime());
  delay(1000);
}
