
#include <MCUFRIEND_kbv.h>

#define buzzer 13
#include <Wire.h>
#include "RTClib.h"
#include "StringSplitter.h"


float stime;
uint8_t shour;
uint8_t sminute;
float           floatnow=0;

/////// Seting RTC via sermon
char c;
String phrase;
int Y,M,D,Hh,Mm,Ss;

RTC_DS3231 rtc;
float la   = -7.271733;  //-7.271733, 112.736177
float lo   = 112.736177;
float zona = 7;
float iht  = 0.03;

uint8_t   hD = 0;
uint8_t   hM = 0;
uint16_t  hY = 0;
    
float sholatT[7];
uint8_t dotSt = 0;
char locBuff[50];


#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_TFTLCD.h> // Hardware-specific library
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET 0 // Can alternately just connect to Arduino's reset pin

char buff[50];
char daysOfTheWeek[7][10] = {"MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU"};
char bulan [12][10] = {"Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "Nopember", "Desember"};

//analog clock
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg = 0, mdeg = 0, hdeg = 0;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
int ss,mm,hh,d,m,y;
//Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
MCUFRIEND_kbv tft;

void setup(void) {
  Serial.begin(9600);
  phrase.reserve(100);
  
    uint32_t when = millis();
    //    while (!Serial) ;   //hangs a Leonardo until you connect a Serial
    if (!Serial) delay(5000);           //allow some time for Leonardo
    Serial.println("Serial took " + String((millis() - when)) + "ms to start");
       // tft.reset();                 //hardware reset
    uint16_t ID = tft.readID(); //
    Serial.print("ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
//    ID = 0x9329;                             // force ID
    tft.begin(ID);
    rtc.begin();

    tft.fillScreen(BLACK);
    tft.setRotation(1);
    pinMode(buzzer,OUTPUT);
    tft.fillScreen(BLACK);
    fiil();
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2020, 4, 22, 19, 42, 0));
    
  // rtc only
    if(! rtc.begin()) {
      Serial.println("RTC tidak terhubung, Cek kembai wiring! ");
      Serial.flush();
      while (1);
      }
    if (! rtc.lostPower()) {
      Serial.println("RTC tidak bekerja, Setel ulang Waktu!");
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      //rtc.adjust(DateTime(2020, 4, 22, 19, 42, 0));
      }
     Serial.println("Test modul RTC DS 3231!");
     delay (500);
    //print_intro();
}

void loop(void) {
  updateTime();
  DateTime now = rtc.now();
  ss = now.second();
  mm = now.minute();
  hh = now.hour();
  d  = now.day();
  m  = now.month();
  y  = now.year();
  sholatCal();

  while (Serial.available() > 0) {
    c = Serial.read();
    phrase += c;
    if (c == '\n') {
      Serial.println();
      decode_c();
    }
  }
  
  tft.fillRect(20, 130, 200, 40, GREEN); 
  //analogClock ();
  tft.fillRect(0, 0, 600, 50, BLACK); //x,y,w,h,c
  sprintf(buff,"<<%02d:%02d:%02d>>",hh,mm,ss);
  tft.setTextColor(WHITE);
  tft.setTextSize(6);
  tft.setCursor(30, 5);
  tft.println(buff);
  tft.setTextColor(BLACK);
  sprintf(buff,"%02d/%02d/%02d",d,m,y);
  tft.setTextSize(3);
  tft.setCursor(25, 140);
  tft.println(buff);

  /// judul jws///
  tft.setTextColor (RED);
  tft.setTextSize(3);
  tft.setCursor(4,180);
  tft.println(F("Jadwal Sholat "));
  
  ///////// Menampilkan hari///////////
  tft.fillRect(0,70,260,25,GREEN); // Background hari
  tft.setTextColor(BLACK,GREEN);
  tft.setTextSize(2);
  tft.setCursor(ss*2.1,76);
  tft.print(F("HARI:"));
  tft.print(daysOfTheWeek[now.dayOfTheWeek()]);

  stime = sholatT[0]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(155, 235);
  tft.print("Subuh =");
  tft.print(locBuff);

  stime = sholatT[2]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor(155, 265);
  tft.print("Dhuha =");
  tft.print(locBuff);

  stime = sholatT[3]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(BLUE);
  tft.setTextSize(2);
  tft.setCursor(155, 295);
  tft.print("Dzuhur=");
  tft.print(locBuff);

  stime = sholatT[4]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setCursor(320, 235);
  tft.print("Ashar =");
  tft.print(locBuff);

  stime = sholatT[5]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(320, 265);
  tft.print("Magrib=");
  tft.print(locBuff);

  stime = sholatT[6]+0.5/60;
  shour = floor(stime);
  sminute = floor((stime-(float)shour)*60);
  sprintf(locBuff,"%02d:%02d\0",shour,sminute);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.setCursor(320, 295);
  tft.print("Isya  =");
  tft.print(locBuff);
  
  if (hh == 0 && mm == 0 && ss < 5){fiil();}
  delay(500);
  check_time();
}




void fiil(){

      //tft.fillRoundRect(0,0,400,500,30,GREEN); // warna back ground jam analok
      //tft.fillRect(20, 130, 200, 40, GREEN); // Background Tanggal
      tft.fillRect(150, 230, 160, 25, BLUE);
      tft.fillRect(150, 260, 160, 25, CYAN);
      tft.fillRect(150, 290, 160, 25, GREEN); 
      tft.fillRect(315, 230, 160, 25, YELLOW);
      tft.fillRect(315, 260, 160, 25, RED);
      tft.fillRect(315, 290, 160, 25, MAGENTA); 
}

/*
//Jam analog
void analogClock () {
  boolean initial = 1;
  uint16_t xpos = 65; // x posisi jam

    sdeg = ss * 6;
    mdeg = mm * 6 + sdeg * 0.01666667;
    hdeg = hh * 30 + mdeg * 0.0833333; 
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      tft.drawLine(ohx, ohy, xpos,  171, BLACK);
      ohx = hx * 62 + xpos + 1;
      ohy = hy * 62 +  171;
      tft.drawLine(omx, omy, xpos,  171, BLACK);
      omx = mx * 84 + xpos;
      omy = my * 84 +  171;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft.drawLine(osx, osy, xpos,  171, BLACK);
    osx = sx * 90 + xpos + 1;
    osy = sy * 90 +  171;
    tft.drawLine(osx, osy, xpos,  171, RED);
    tft.drawLine(ohx, ohy, xpos,  171, CYAN);
    tft.drawLine(omx, omy, xpos,  171, WHITE);
    tft.drawLine(osx, osy, xpos,  171, RED);
    tft.fillCircle(xpos,  171, 3, RED);
    tft.drawRoundRect(xpos-50,  121, 100, 100, 10, WHITE);
  }
  */
//Jam analog
void analogClock () {
  boolean initial = 1;
  uint16_t xpos = 320; // x posisi jam

    sdeg = ss * 6;
    mdeg = mm * 6 + sdeg * 0.01666667;
    hdeg = hh * 30 + mdeg * 0.0833333; 
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);
    
    if (ss == 0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute   
      tft.drawLine(ohx, ohy, xpos,  155, BLACK);   
      ohx = hx * 35 + xpos + 1 ;
      ohy = hy * 35 +  155; 
      tft.drawLine(omx, omy, xpos,  155, BLACK);    
      omx = mx * 45 + xpos + 1;
      omy = my * 45 +  155;     
      
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    tft.drawLine(osx, osy, xpos,  155, BLACK);
    osx = sx * 50 + xpos + 1;
    osy = sy * 50 +  155;      
    tft.drawLine(osx, osy, xpos,  155, RED);
    tft.drawLine(ohx, ohy, xpos,  155, CYAN);
    tft.drawLine(omx, omy, xpos,  155, WHITE);
    tft.drawLine(osx, osy, xpos,  155, RED);
    tft.fillCircle(320,  155, 3, RED);
    tft.drawRoundRect(255,  90, 130, 130,30, WHITE);
    
    
    /*
    tft.fillRect(x,y,w,h,t); //fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t t)
    tft.drawRect(x,y,w,h,t); //drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t t)
    tft.fillRoundRect(x,y,w,h,r,t); //fillRoundRect (int16_t x, int16_t y, int16_t w, int16_t h, uint8_t R , uint16_t t)
    tft.drawRoundRect(x,y,w,h,r,t); //drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t R , uint16_t t)
    
    fillRect function draws a filled rectangle in x and y location. w is width, h is height and t is color of the rectangle

    drawRect function draws a rectangle in x and y location with w width and h height and t color.
    
    fillRoundRect function draws a filled Rectangle with r radius round corners in x and y location and w width and h height and t color.
    
    drawRoundRect function draws a Rectangle with r radius round corners in x and y location and w width and h height and t color.
    */
    
    //Angka Jam Analog
    
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor (310,93);
  tft.print ("12");
  
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor (372,150);
  tft.print ("3");
  
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor (315,202);
  tft.print ("6");
  
  tft.setTextColor(RED);
  tft.setTextSize(2);
  tft.setCursor (258,150);
  tft.print ("9");
  
  }



void check_time()
    {
      for(uint8_t i=0; i <7; i++)
        {
          if (not((i == 1) or (i == 2))){
            if( (floatnow >= (sholatT[i])) and (floatnow < (sholatT[i]+0.015)))
              { 
               digitalWrite(buzzer, HIGH);
               delay(5000);
               digitalWrite(buzzer, LOW);
               for (int x = 0; x < 10; x++){
                  digitalWrite(buzzer, HIGH);
                  delay(1000);
                  digitalWrite(buzzer, LOW);
                  delay(1000);
               }
               delay(1000);
              }
          }
       }
    }


void updateTime()
    {
      DateTime now = rtc.now();
      floatnow = float(now.hour()) + float(now.minute())/60 + float(now.second())/3600;
      //Serial.println(floatnow);
    }

void decode_c(){
  if (phrase.charAt(phrase.length() - 1) == '\n') {
        Serial.print(phrase);
        StringSplitter *splitter = new StringSplitter(phrase, ',', 6);  
        int itemCount = splitter->getItemCount();
        Serial.println("Data count: " + String(itemCount));
       
        Y = splitter->getItemAtIndex(0).toInt();
        M = splitter->getItemAtIndex(1).toInt();
        D = splitter->getItemAtIndex(2).toInt();
        hh = splitter->getItemAtIndex(3).toInt();
        mm = splitter->getItemAtIndex(4).toInt();
        ss = splitter->getItemAtIndex(5).toInt();
        
        //validasi
        if((Y<2020)||(M>12)||(D>31)||(hh>24)||(mm>60)||(ss>60)){
          Serial.println("Pengaturan tidak diterima!");
          Serial.println("Sesuaikan dengan tanggal dan waktu yang berlaku");
          Serial.println();
        }
        else{
          Serial.println("Pengaturan diterima!");
          rtc.adjust(DateTime(Y, M, D, Hh, Mm, Ss));
          Serial.print("Mohon tunggu sebentar .");
          int count=0;
          while(count != 10){
            Serial.print(".");
            count++;
            delay(500);     
          } 
        }
        Serial.println();
        phrase="";
        delay(2000);
  } 
}
    
/*
void print_intro(){
  Serial.println("### Konfigurasi DS3231 Via Serial Command ###");
  Serial.println("    format -> Y,M,D,h,m,s");
  Serial.println("    contoh -> 2020,5,10,7,0,0");
  Serial.println("              10 mei 2020 jam 7:00");
  Serial.println();

    DateTime now = rtc.now();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

      

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    Serial.println();
  delay(500);
}
*/
