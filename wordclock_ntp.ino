#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <FastLED.h> //https://github.com/FastLED/FastLED

FASTLED_USING_NAMESPACE;
#define DATA_PIN    3
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    164      // Gesamtanzahl der WS2812 LEDs
CRGB leds[NUM_LEDS];

uint8_t maxBrightness     = 150;  // Wenn Fotoresistor benutzt wird, hier max. Helligkeit eintragen (0=Off->255=max)
uint8_t minBrightness     = 35;   // Wenn Fotoresistor benutzt wird, hier min. Helligkeit eintragen (0=Off->255=max)
uint8_t AmbientLightPin   = 0;    // Fotoresistor Pin 
uint8_t BRIGHTNESS        = 180;  // Wenn kein Fotoresistor benutzt wird hier dauerhafte Helligkeit eintragen

uint8_t Stunde     = 0;
uint8_t Minute     = 15; 
uint8_t WochenTag  = 0;   
uint8_t Tag        = 30;
uint8_t Monat      = 10;
boolean DST        = false;

// wifi and ntp
const char* ssid = "";
const char* password = "";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");


//***HIER LED POSITIONEN EINTRAGEN***//
int EsIst[]     = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

int Eins[]      = {96, 97, 98, 99, 100, 101};
int Ein[]       = {97, 98, 99, 100, 101};
int Zwei[]      = {86, 87, 88, 89, 90, 91};
int Drei[]      = {102, 103, 104, 105, 106, 107, 108};
int Vier[]      = {109, 110, 111, 112, 113, 114};
int Fuenf[]     = {80, 81, 82, 83, 84, 85};
int Sechs[]     = {122, 123, 124, 125, 126, 127, 128, 129};
int Sieben[]    = {130, 131, 132, 133, 134, 135, 136, 137, 138, 139};
int Acht[]      = {115, 116, 117, 118, 119, 120, 121};
int Neun[]      = {73, 74, 75, 76, 77, 78, 79};
int Zehn[]      = {153, 154, 155, 156, 157, 158, 159};
int Elf[]       = {92, 93, 94, 95};
int Zwoelf[]    = {140, 141, 142, 143, 144, 145, 146, 147};
int MZwanzig[]  = {19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30};
int MFuenf[]    = {12, 13, 14, 15, 16, 17, 18};
int MZehn[]     = {36, 37, 38, 39, 40, 41, 42};

int Viertel[]   = {43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54};
int Vor[]       = {68, 69, 70, 71, 72};
int Nach[]      = {55, 56 ,57, 58, 59, 60, 61};
int Halb[]      = {62, 63, 64, 65, 66, 67};
int Uhr[]       = {148, 149, 150, 151, 152};
int Und[]       = {31, 32, 33, 34, 35};

int EinsM[]     = {160}; //Minuten Punkt 1
int ZweiM[]     = {161}; //Minuten Punkt 2
int DreiM[]     = {162}; //Minuten Punkt 3
int VierM[]     = {163}; //Minuten Punkt 4
//**********************************//

int i, Ambient, LastAmbient;

void setup() {
  Serial.begin(9600);
  delay(3000); // Sicherheits Delay

  // connect to wifi
  WiFi.begin(ssid, password);

  Serial.print("connecting to ");
  Serial.println(ssid);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connected!");
  
  // configure ntp
  Serial.println("starting ntp client");
  timeClient.begin();
  timeClient.setTimeOffset(3600); // offset in seconds to GMT
  timeClient.update();
  Serial.print("ntp time: ");
  Serial.println(timeClient.getFormattedTime());

  //LEDs werden eingefügt + Farbkorrektur und Farbtemperatur auf Wolfram (warmweiß)
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalPixelString);
  FastLED.setTemperature( Tungsten40W );
  Serial.println("Starte Wordclock ...");
}

void SwitchLED(int MyArray[], int n) {
  //Umgebungshelligkeit überprüfen (sofern gewünscht)
  if(maxBrightness != 0){
      Ambient = analogRead(AmbientLightPin);
      //Nur bei größeren Helligkeitsänderungen soll die Helligkeit der LEDs angepasst werden:
      if((Ambient > LastAmbient*1.10) || (Ambient < LastAmbient*0.90)){
        BRIGHTNESS = map(Ambient, 0, 1023, maxBrightness, minBrightness);
        LastAmbient = Ambient;
      }
  }
  //Funktion zum Anschalten der LEDs in warmweiß (oder gewünschter Farbe)
  for (i = MyArray[0]; i < MyArray[0]+n; i++) {
      leds[i] = 0xFFC58F;                         // HEX Warmweiß | Hier gewünschte LED Farbe (HEX) eintragen
      FastLED.setBrightness(BRIGHTNESS);
      //leds[i] = CHSV(140, 27, BRIGHTNESS);      // ODER hier gewünschte Farbe in HSV (  CHSV(FARBE, SÄTTIGUNG, BRIGHTNESS)  )
      //leds[i].setRGB(255, 68, 221);             // ODER hier gewünschte Farbe in RGB (  .setRGB(Rot,Grün,Blau)   )
  }
}

void displaytime(void){
  //zuerst setzten wir alle LEDs zurück
  fill_solid( leds, NUM_LEDS, CHSV(0, 0, 0));

  // Nun suchen wir die richtigen LEDs und übergeben sie an die Funktion zum Anschalten
  //Serial.print("Es ist ");
  SwitchLED(EsIst, (sizeof(EsIst)/2));
  
  if (((Minute>4) && (Minute<10)) || (Minute>54)) { 
    SwitchLED(MFuenf, (sizeof(MFuenf)/2));
    //Serial.print("5 Minuten ");
  }
  if (((Minute>9) && (Minute<15)) || ((Minute>49) && (Minute<55))) { 
    SwitchLED(MZehn, (sizeof(MZehn)/2));
    //Serial.print("10 Minuten ");
  }
  if (((Minute>14) && (Minute<20)) || ((Minute>44) && (Minute<50))) {
    SwitchLED(Viertel, (sizeof(Viertel)/2)); 
    //Serial.print("Viertel ");
  }
  if (((Minute>19) && (Minute<25)) || ((Minute>39) && (Minute<45))) { 
    SwitchLED(MZwanzig, (sizeof(MZwanzig)/2)); 
    //Serial.print("20 Minuten ");
  }
  if (((Minute>24) && (Minute<30)) || ((Minute>34) && (Minute<40))) { 
    SwitchLED(MFuenf, (sizeof(MFuenf)/2)); 
    SwitchLED(Und, (sizeof(Und)/2));
    SwitchLED(MZwanzig, (sizeof(MZwanzig)/2)); 
    //Serial.print("25 Minuten ");
  }  
  if ((Minute>29) && (Minute<35)) {
    SwitchLED(Halb, (sizeof(Halb)/2));
    //Serial.print("Halb ");
  }

  if (Minute <5){
    switch (Stunde) {
      case 1: 
        SwitchLED(Ein, (sizeof(Ein)/2));
        //Serial.print("1 ");
        break;
      case 2: 
        SwitchLED(Zwei, (sizeof(Zwei)/2));
        //Serial.print("2 ");
        break;
      case 3: 
        SwitchLED(Drei, (sizeof(Drei)/2));
        //Serial.print("3 ");
        break;
      case 4: 
        SwitchLED(Vier, (sizeof(Vier)/2));
        //Serial.print("4 ");
        break;
      case 5: 
        SwitchLED(Fuenf, (sizeof(Fuenf)/2));
        //Serial.print("5 ");
        break;
      case 6: 
        SwitchLED(Sechs, (sizeof(Sechs)/2));
        //Serial.print("6 ");
        break;
      case 7: 
        SwitchLED(Sieben, (sizeof(Sieben)/2));
        //Serial.print("7 ");
        break;
      case 8: 
        SwitchLED(Acht, (sizeof(Acht)/2));
        //Serial.print("8 ");
        break;
      case 9: 
        SwitchLED(Neun, (sizeof(Neun)/2));
        //Serial.print("9 ");
        break;
      case 10: 
        SwitchLED(Zehn, (sizeof(Zehn)/2));
        //Serial.print("10 ");
        break;
      case 11: 
        SwitchLED(Elf, (sizeof(Elf)/2));
        //Serial.print("11 ");
        break;
      case 12: 
        SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
        //Serial.print("12 ");
        break;
    }
    SwitchLED(Uhr, (sizeof(Uhr)/2));
    //Serial.println("Uhr");
  }
  else if ((Minute < 30) && (Minute >4)) {
    SwitchLED(Nach, (sizeof(Nach)/2));
    //Serial.print("nach ");
    switch (Stunde) {
      case 1: 
        SwitchLED(Eins, (sizeof(Eins)/2));
        //Serial.println("1 ");
        break;
      case 2: 
        SwitchLED(Zwei, (sizeof(Zwei)/2));
        //Serial.println("2 ");
        break;
      case 3: 
        SwitchLED(Drei, (sizeof(Drei)/2));
        //Serial.println("3 ");
        break;
      case 4: 
        SwitchLED(Vier, (sizeof(Vier)/2));
        //Serial.println("4 ");
        break;
      case 5: 
        SwitchLED(Fuenf, (sizeof(Fuenf)/2));
        //Serial.println("8 ");
        break;
      case 6: 
        SwitchLED(Sechs, (sizeof(Sechs)/2));
        //Serial.println("6 ");
        break;
      case 7: 
        SwitchLED(Sieben, (sizeof(Sieben)/2));
        //Serial.println("7 ");
        break;
      case 8: 
        SwitchLED(Acht, (sizeof(Acht)/2));
        //Serial.println("8 ");
        break;
      case 9: 
        SwitchLED(Neun, (sizeof(Neun)/2));
        //Serial.println("9 ");
        break;
      case 10: 
        SwitchLED(Zehn, (sizeof(Zehn)/2));
        //Serial.println("10 ");
        break;
      case 11: 
        SwitchLED(Elf, (sizeof(Elf)/2));
        //Serial.println("11 ");
        break;
      case 12: 
        SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
        //Serial.println("12 ");
        break;
    }
  }
  else{
    if ((Minute>29) && (Minute<35)){
      switch (Stunde) {
        case 1: 
          SwitchLED(Zwei, (sizeof(Zwei)/2));
          //Serial.println("2 ");
          break;
        case 2: 
          SwitchLED(Drei, (sizeof(Drei)/2));  
          //Serial.println("3");
          break;
        case 3: 
          SwitchLED(Vier, (sizeof(Vier)/2));  
          //Serial.println("4 ");
          break;
        case 4: 
          SwitchLED(Fuenf, (sizeof(Fuenf)/2));
          //Serial.println("5 ");
          break;
        case 5: 
          SwitchLED(Sechs, (sizeof(Sechs)/2)); 
          //Serial.println("6 ");
          break;
        case 6: 
          SwitchLED(Sieben, (sizeof(Sieben)/2));
          //Serial.println("7 ");
          break;
        case 7: 
          SwitchLED(Acht, (sizeof(Acht)/2));
          //Serial.println("8 ");
          break;
        case 8: 
          SwitchLED(Neun, (sizeof(Neun)/2));  
          //Serial.println("9 ");
          break;
        case 9: 
          SwitchLED(Zehn, (sizeof(Zehn)/2)); 
          //Serial.println("10 ");
          break;
        case 10: 
          SwitchLED(Elf, (sizeof(Elf)/2)); 
          //Serial.println("11 ");
          break;
        case 11: 
          SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
          //Serial.println("12 ");
          break;
        case 12: 
          SwitchLED(Eins, (sizeof(Eins)/2));
         // Serial.println("1 ");
          break;
        }
      }
      else{
        SwitchLED(Vor, (sizeof(Vor)/2));
        //Serial.print("vor ");
        switch (Stunde) {
          case 1: 
            SwitchLED(Zwei, (sizeof(Zwei)/2));
            //Serial.println("2 ");
            break;
          case 2: 
            SwitchLED(Drei, (sizeof(Drei)/2));  
            //Serial.println("3");
            break;
          case 3: 
            SwitchLED(Vier, (sizeof(Vier)/2));  
            //Serial.println("4 ");
            break;
          case 4: 
            SwitchLED(Fuenf, (sizeof(Fuenf)/2));
            //Serial.println("5 ");
            break;
          case 5: 
            SwitchLED(Sechs, (sizeof(Sechs)/2)); 
            //Serial.println("6 ");
            break;
          case 6: 
            SwitchLED(Sieben, (sizeof(Sieben)/2));
            //Serial.println("7 ");
            break;
          case 7: 
            SwitchLED(Acht, (sizeof(Acht)/2));
            //Serial.println("8 ");
            break;
         case 8: 
            SwitchLED(Neun, (sizeof(Neun)/2));  
            //Serial.println("9 ");
            break;
          case 9: 
            SwitchLED(Zehn, (sizeof(Zehn)/2)); 
            //Serial.println("10 ");
            break;
         case 10: 
            SwitchLED(Elf, (sizeof(Elf)/2)); 
            //Serial.println("11 ");
            break;
          case 11: 
            SwitchLED(Zwoelf, (sizeof(Zwoelf)/2));
            //Serial.println("12 ");
            break;
          case 12: 
            SwitchLED(Eins, (sizeof(Eins)/2));
            //Serial.println("1 ");
            break;
        }
      }
    }
  // Minuten Zähler
  uint8_t MinCount = Minute-(floor(Minute/10)*10);
  if(MinCount > 5)
    MinCount = MinCount - 5;
  switch(MinCount){
    case 4:
      SwitchLED(VierM, (sizeof(VierM)/2));
    case 3:
      SwitchLED(DreiM, (sizeof(DreiM)/2));
    case 2:
      SwitchLED(ZweiM, (sizeof(ZweiM)/2));
    case 1:
      SwitchLED(EinsM, (sizeof(EinsM)/2));
  }
  FastLED.show();
}

void CheckDST(void){
  //Funktion zum Überprüfen ob Sommer oder Winterzeit

  // get epoch time to calculate date
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime ((time_t *)&epochTime);

  Tag = ptm->tm_mday;
  Monat = ptm->tm_mon+1;
 
  WochenTag = (timeClient.getDay()-1);
  int Hilf = Tag-WochenTag;
  if(Monat <= 2 || Monat >= 11)
    DST = false;                                 // Winterzeit

  if(Monat >= 4 && Monat <= 9)
    DST = true;                                  // Sommerzeit
      
  if((Monat == 3) && (Hilf >= 25))
     DST = true;                                 // Beginn der Sommerzeit
       
  if((Monat == 10) && (Hilf >= 25))
    DST = false;                                 // Beginn der Winterzeit
 
}

void loop() {
  /* Schleifen Funktion, die die Uhrzeit ausliest und dann in die 
   * entsprechende Funktionen übergibt.
   */
  timeClient.update();
  
  CheckDST();
  Stunde = timeClient.getHours();
  Minute = timeClient.getMinutes();

  if(DST == true){
    Stunde += 1;
  }
   
  if(Stunde > 12)
    Stunde = Stunde-12;
  else if (Stunde == 0)
    Stunde = 12;

  displaytime();
  FastLED.delay(250);
  Serial.print(Stunde);
  Serial.print(":");
  Serial.print(Minute);
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
}
