#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <WiFiUdp.h>
 
const char* SSID = "Debi lan";
const char* PSK = "HubbaBubbaGang799";
const char* MQTT_BROKER = "192.168.0.108";

#define PIN 5
#define NUM_LEDS 600
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

//--------------------------------------
//music control
#define BUFFER_LEN 1024
#define PRINT_FPS 0
unsigned int localPort = 7777;
char packetBuffer[BUFFER_LEN];

// Network information
// IP must match the IP in config.py in python folder
IPAddress ip(192, 168, 0, 250);
// Set gateway to your router's gateway
IPAddress gateway(192, 168, 0, 1);
//IPAddress subnet(255, 255, 255, 0);

WiFiUDP port;

uint8_t N = 0;

//--------------------------------------

int mode = 0;
int last_mode = 0;
float brightness = 1;
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
 
void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(MQTT_BROKER, 1883);
    client.setCallback(callback);

    port.begin(localPort);

    strip.begin();
    strip.show();
}
 
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(SSID);

    //WiFi.config(ip, gateway, subnet);
    WiFi.begin(SSID, PSK);
 
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
 
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Received message [");
    Serial.print(topic);
    Serial.print("] ");
    char msg[length+1];
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
        msg[i] = (char)payload[i];
    }
    Serial.println();
 
    msg[length] = '\0';
    Serial.println(msg);
 
    if(strcmp(msg,"on")==0){
        mode = 1;
    }
    else if(strcmp(msg,"off")==0){
        mode = 0;
        turn_off();
    }
    else if(strcmp(msg,"b_up")==0) {
      if(brightness < 1 ) {
        brightness += 0.1;
      }
      if(brightness > 1) {
        brightness = 1;
      }
    }
    else if(strcmp(msg,"b_down")==0) {
      if(brightness > 0 ) {
        brightness -= 0.1;
      }
      if(brightness < 0) {
        brightness  = 0;
      }
    }
    else if(strcmp(msg,"sparkle")==0){
        mode = 2;
    }
    else if(strcmp(msg,"running_w")==0){
        mode = 3;
    }
    else if(strcmp(msg,"rainbow")==0){
        mode = 4;
    }
    else if(strcmp(msg,"fire")==0){
        mode = 5;
    }
    else if(strcmp(msg,"wipe")==0){
        mode = 6;
    }
    else if(strcmp(msg,"music")==0){
        mode = 7;
    }
    else if(strcmp(msg,"music2")==0){
        mode = 8;
    }
    
}
 
void reconnect() {
    while (!client.connected()) {
        Serial.println("Reconnecting MQTT...");
        if (!client.connect("ESP8266Client")) {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
    client.subscribe("chris/led");
    Serial.println("MQTT Connected...");
}

//---------------------------------------------

void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void turn_off() {
  //dye 3 led ends orange
  for (int i = 0 ; i<3; i++){
    setPixel(i, 255, 123, 0);
    setPixel(NUM_LEDS-i, 255, 123, 0);
  }
  for (int i = 3 ; i<NUM_LEDS/2; i++){
    //left
    setPixel(i, 255, 123, 0);
    setPixel(i-3, 0, 0, 0);
    //right
    setPixel(NUM_LEDS-i, 255, 123, 0);
    setPixel(NUM_LEDS-i+3, 0, 0, 0);

    showStrip();
  }
  setAll(0,0,0);
  
  
}
  
//---------------------------------------------
 
void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    switch(mode) {
      case 1:
      //turn on white light
        for(int i = 0 ; i < NUM_LEDS ; i++) {
          setPixel(i,200,200,195);
          showStrip();
             
          client.loop();
          if(mode != 1) {break;}
        }
      break;
      case 2:
        //sparkle
        Sparkle(random(255), random(255), random(255), 0);
      break;
      case 3:
        //running white
        RunningLights(0xff,0xff,0xff, 50);  // white
      break;
      case 4:
        //Rainbow
        rainbowCycle(20);
      break;
      case 5:
        //Fire
        meteorRain(0xff,0xff,0xff,10, 64, true, 30);
      break;
      case 6:
        //Wipe
        colorWipe(36,176,179, 50);
        colorWipe(0x00,0x00,0x00, 50);
      break;
      case 7:
      //music control
      music();
      break;
      case 8:
      //music control
      music2();
      break;
    }
    }







    
    

//-----------------Music--------------------\\


void music() {

  Serial.println("Music mode");
  String last_val = "0";
  while(true){
     int packetSize = port.parsePacket();
     
      if (packetSize)
      {
        int len = port.read(packetBuffer, BUFFER_LEN);
        if (len > 0)
        {
          packetBuffer[len] = '\0';
        }
        Serial.println((String)packetBuffer);
        Serial.println("-----");
      
        if((String)packetBuffer == "1") {
          if (last_val == "1") {
            setPixel(NUM_LEDS/2, 0, 0, 0);
            last_val = "0";
          } else {
            setPixel(NUM_LEDS/2, 21, 130, 163);
            last_val = "1";
          }
          
        }else if( (String)packetBuffer == "2") {
          if (last_val == "2") {
            setPixel(NUM_LEDS/2, 0, 0, 0);
            last_val = "0";
          } else {
            setPixel(NUM_LEDS/2, 133, 25, 140);
            last_val = "2";
          }
        }
        else if ((String)packetBuffer == "3") {
          if (last_val == "3") {
            setPixel(NUM_LEDS/2, 0, 0, 0);
            last_val = "0";
          } else {
          setPixel(NUM_LEDS/2, 219, 18, 98);
          last_val = "3";
          }
        }
      }

      for(int i = 1 ; i < NUM_LEDS ; i++) {
        if(i < NUM_LEDS/2) {
          uint32_t color = strip.getPixelColor(i+ 1);
          strip.setPixelColor(i, color);
        }
        else if (i > NUM_LEDS/2){
          //von außen nach innen
          int n = NUM_LEDS - (i-NUM_LEDS/2);
          uint32_t color = strip.getPixelColor(n-1);
          strip.setPixelColor(n, color);
        }
      }

      showStrip();
      client.loop();
      if(mode != 7) {break;}
        
        
    }
  }  

  void music2() {

  Serial.println("Music mode2");
  String last_val = "0";
  while(true){
     int packetSize = port.parsePacket();
     
      if (packetSize)
      {
        int len = port.read(packetBuffer, BUFFER_LEN);
        if (len > 0)
        {
          packetBuffer[len] = '\0';
        }
        Serial.println((String)packetBuffer);
        Serial.println("-----");
      
        if((String)packetBuffer == "1") {
          if (last_val == "1") {
            setPixel(50, 0, 0, 0);
            setPixel(150, 0, 0, 0);
            setPixel(250, 0, 0, 0);
            last_val = "0";
          } else {
            setPixel(50, 31, 161, 222);
            setPixel(150, 31, 161, 222);
            setPixel(250, 31, 161, 222);
            last_val = "1";
          }
          
        }else if( (String)packetBuffer == "2") {
          if (last_val == "2") {
            setPixel(50, 0, 0, 0);
            setPixel(150, 0, 0, 0);
            setPixel(250, 0, 0, 0);
            last_val = "0";
          } else {
            setPixel(50, 237, 40, 83);
            setPixel(150, 237, 40, 83);  
            setPixel(250, 237, 40, 83);
            last_val = "2";
          }
        }
        else if ((String)packetBuffer == "3") {
          if (last_val == "3") {
            setPixel(50, 0, 0, 0);
            setPixel(150, 0, 0, 0);
            setPixel(250, 0, 0, 0);
            last_val = "0";
          } else {
          setPixel(50, 245, 153, 54);
          setPixel(150, 245, 153, 54);
          setPixel(250, 245, 153, 54);
          last_val = "3";
          }
        }
      }

      for(int i = 1 ; i < NUM_LEDS ; i++) {
        if(i < 50) {
          uint32_t color = strip.getPixelColor(i+ 1);
          strip.setPixelColor(i, color);
        }
        else if(i > 50 && i < 100){
          //von außen nach innen
          int n = 100 - (i-50);
          uint32_t color = strip.getPixelColor(n-1);
          strip.setPixelColor(n, color);
        }
        else if(i < 150 && i > 100){
          //von außen nach innen
          uint32_t color = strip.getPixelColor(i+1);
          strip.setPixelColor(i, color);
        }
        else if(i > 150 && i < 200){
          //von außen nach innen
          int n = 200 - (i-150);
          uint32_t color = strip.getPixelColor(n-1);
          strip.setPixelColor(n, color);
        }
        else if(i > 200 && i < 250){
          //von außen nach innen
          uint32_t color = strip.getPixelColor(i+1);
          strip.setPixelColor(i^1, color);
        }
        else if(i > 250 && i < 300){
          //von außen nach innen
          int n = 300 - (i-250);
          uint32_t color = strip.getPixelColor(n-1);
          strip.setPixelColor(n, color);
        }
      }

      showStrip();
      client.loop();
      if(mode != 8) {break;}
        
        
    }
  }  

//-----------------ANIMATIONS--------------------\\

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void RunningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;
 
  for(int j=0; j<NUM_LEDS*2; j++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<NUM_LEDS; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }
     
      showStrip();
      delay(WaveDelay);

      client.loop();
      if(mode != 3) {break;}
  }
}

void rainbowCycle(int SpeedDelay) {
  byte *c;
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< NUM_LEDS; i++) {
      c=Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c+1), *(c+2));
    }
    showStrip();
    delay(SpeedDelay);

    client.loop();
    if(mode != 4) {break;}
    Serial.println(brightness);
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];
 
  if(WheelPos < 85) {
   c[0]=WheelPos * 3;
   c[1]=255 - WheelPos * 3;
   c[2]=0;
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   c[0]=255 - WheelPos * 3;
   c[1]=0;
   c[2]=WheelPos * 3;
  } else {
   WheelPos -= 170;
   c[0]=0;
   c[1]=WheelPos * 3;
   c[2]=255 - WheelPos * 3;
  }

  return c;
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  setAll(0,0,0);
 
  for(int i = 0; i < NUM_LEDS+NUM_LEDS; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<NUM_LEDS; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <NUM_LEDS) && (i-j>=0) ) {
        setPixel(i-j, red, green, blue);
      }
    }
   
    showStrip();
    delay(SpeedDelay);

    client.loop();
    if(mode != 5) {break;}
    
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    strip.setPixelColor(ledNo, r,g,b);
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      setPixel(i, red, green, blue);
      showStrip();
      delay(SpeedDelay);

      client.loop();
      if(mode != 6) {break;}
  }
}
