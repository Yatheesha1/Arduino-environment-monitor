
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>


String num="+918123855628";
String st="";

SoftwareSerial mySerial(9,10);//(txPin, rxPin)
#define pHsensorPin 2         
unsigned long int avgValue;  
float phValue=0.0;

 int buf[10],temp;
 int inp1 = 11;
 int inp2 = 12;

 int inf1 = 3;
 int inf2 = 4;
 int ine1 = 5;
 int ine2 = 6;
 int inp3 = 7;
 int inp4 = 8;
 int led =  2;
#define DHTPIN     13
 int sensorPin = A0;
 int sensorValue = 0;
 int ldrPin = A1; 
 int ldrValue = 0;
 
 int humflag=0,temflag=0,moiflag=0,lightflag=0,phflag=0;
 
#define DHTTYPE           DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

sensors_event_t event;
int sensorValue1;
int Light;

void setup() 
{
  mySerial.begin(9600);
  Serial.begin(9600);
  mySerial.println("AT+CMGF=1");
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(100);
  if (mySerial.available()>0)
  {
    Serial.println(mySerial.readString());
  }
  dht.begin();
  pinMode(inf1,OUTPUT);
  pinMode(inf2,OUTPUT);
  pinMode(ine1,OUTPUT);
  pinMode(ine2,OUTPUT);
  pinMode(inp3,OUTPUT);
  pinMode(inp4,OUTPUT);
  pinMode(led,OUTPUT);
  pinMode(sensorPin,INPUT);
  pinMode(pHsensorPin,INPUT);
  pinMode(inp1,OUTPUT); 
  pinMode(inp2,OUTPUT);
}
void loop()
{

  // TEMPERATURE
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println("Error reading temperature!");
  }
  else 
  {
    Serial.print("TEMPERATURE: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
   }
   if (event.temperature > 40) 
   {
    if(temflag==0)
    {
    SendMessage("Temp: "+String(event.temperature)+"*C",num);
    temflag=1;
    }
   }
  else
   {
   temflag=0;
   }
  
   //  HUMIDITY

  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) 
  {
    Serial.println("Error reading humidity!");
    delay(2000);
  }
  else 
  {
    Serial.print("RELATIVE HUMIDITY: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
  if (event.relative_humidity > 80) 
   {
    if(humflag==0)
    {
    SendMessage("Relative Hum: " +String(event.relative_humidity)+"%",num);
    humflag=1;
    }
   }
   else
   {
     humflag=0;
   }
   //SOIL MOISTURE SENSOR

  sensorValue = analogRead(sensorPin);
  
  sensorValue1 = map(sensorValue,200,1023,100,0);
   
   Serial.print("SOIL MOISTURE:");
   Serial.print(sensorValue1);
   Serial.println("%");
   if (sensorValue1 < 1) 
   {
    if(moiflag==0)
    {
    SendMessage("Soil Moisture: "+String(sensorValue1)+"%",num);
    moiflag=1;
    }
    
    }
  else
   {
    moiflag=0;
    }
   
   //LDR SENSOR
  
  ldrValue = analogRead(ldrPin);
  double Vout = ldrValue*0.0048875;
  Light=(500*(5-Vout))/(10*Vout);
  Serial.print("LIGHT INTENSITY:");
  Serial.print(Light);
  Serial.println(" lux");
  if (Light < 30) 
   {
    if(lightflag==0)
    {
    SendMessage("Light: "+String(Light)+"Lux",num);
    lightflag=1;
    } 
  }
  else
  {
    lightflag=0;
  }

  phsense();
  Serial.println();

  SendMessage();
  delay(1000);
}

void SendMessage()
{
  if (mySerial.available())
  { 
    String message="";
    String numb="";
    String msg=mySerial.readString();
    Serial.println(msg);
    numb=msg.substring(9,22);
    Serial.print("Number:");   
    Serial.println(numb);   
    if(numb==num) 
    {    

       message=msg.substring(51,msg.length()-2); 
       Serial.println(message);
       if(message=="FANON")
       {
        Serial.println("fan");
          digitalWrite(inf1,HIGH);
          digitalWrite(inf2,LOW);
       }
       if(message=="FANOFF")
       {
          digitalWrite(inf1,LOW);
          digitalWrite(inf2,LOW);
       }
       if(message=="EFANON")
       {
          digitalWrite(ine1,HIGH);
          digitalWrite(ine2,LOW);
       }
       if(message=="EFANOFF")
       {
          digitalWrite(ine1,LOW);
          digitalWrite(ine2,LOW);
       }
       if(message=="MOTORON")
       {
          digitalWrite(inp3,HIGH);
          digitalWrite(inp4,LOW);
       }
       if(message=="MOTOROFF")
       {
          digitalWrite(inp3,LOW);
          digitalWrite(inp4,LOW);
       } 
       if(message=="PUMPON")
       {
          digitalWrite(inp1,LOW);
          digitalWrite(inp2,LOW);
       } 
       if(message=="PUMPOFF")
       {
          digitalWrite(inp1,LOW);
          digitalWrite(inp2,LOW);
       }
       if(message=="LIGHTON")
       {
          digitalWrite(led,HIGH);
       }
       if(message=="LIGHTOFF")
       {
          digitalWrite(led,LOW);
       }
       if(message=="STATUS")
       {
         String val="Temperature:"+String(event.temperature,1)+"*C  Relative Humidity:"+String(event.relative_humidity)+"% Soil Moisture:"+String(sensorValue1)+"% Light:"+String(Light)+"Lux  Soil pH:"+String(phValue,2);
         SendMessage(val,num);
       }
     }   
  }
}

void SendMessage(String st,String nu)
{
  mySerial.println("AT+CMGF=1\r\n"); //Sets the GSM Module in Text Mode
  delay(1000); // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\""+nu+"\"\r\n"); // Replace x with mobile number
  delay(1000);
  mySerial.println(st);
  delay(100);
  mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}


 void phsense()
     {
         for(int i=0;i<10;i++)       
         { 
             buf[i]=analogRead(pHsensorPin);
             delay(10);
         }
         for(int i=0;i<9;i++)        
          {
           for(int j=i+1;j<10;j++)
                  {
                      if(buf[i]>buf[j])
                       {
                            temp=buf[i];
                            buf[i]=buf[j];
                            buf[j]=temp;
                        }
                   }
              }
          avgValue=0;
          for(int i=2;i<8;i++)                      
          avgValue+=buf[i];
          float phVoltage=((float)avgValue*4.52/1024)/6; 
          
          phValue=3*phVoltage;                     
          Serial.print("SOIL pH:");  
          Serial.print(phValue,2);
          Serial.println(" ");
          delay(1000);
          if ( phValue<5) 
              {
               if(phflag==0)
               {
                SendMessage("Soil pH: "+String(phValue,2),num);
                phflag=1;
               }  
    
              }
          else
              {
                  phflag=0;
              }
  

}
