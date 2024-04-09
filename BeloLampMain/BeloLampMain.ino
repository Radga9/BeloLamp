#include <ESP8266WiFi.h>
#include <SimplePortal.h>
#include <GyverPortal.h>
#include <FastLED.h>

#define AP_SSID "NETWORK NAME"
#define AP_PASS "NETWORK PASSWORD"

#define NUM_STRIPS 8

#define LEDS_PER_STRIP 4

#define DATA_PIN_D4 2
#define DATA_PIN_D3 0 
#define DATA_PIN_D2 4 
#define DATA_PIN_D1 5 
#define DATA_PIN_D8 15 
#define DATA_PIN_D6 12 
#define DATA_PIN_D5 14 
#define DATA_PIN_D0 16

#define Ksesa_PINK "#ffb6c1"
#define ADMIN_NICKNAME "ADMIN"
#define ADMIN_PASS  "PASS"
#define mDNS_NAME "DNS-LOCAL-LINK"

int offset_hue = 0;  // Initial offset_hue for the running hue
int runningHueSpeed = 20;  // Adjust this value to change the speed
int BrightnessWS2812B = 100;

bool power_mode;


GyverPortal LampaUI;

CRGB leds[NUM_STRIPS][LEDS_PER_STRIP];


void build() {
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);
  GP.TITLE("SOME TITLE");
  GP_MAKE_BOX(
    GP.LABEL("POWER");
    GP.SWITCH("POWER_SW",0,Ksesa_PINK);
    );
  GP.BREAK();
  GP_MAKE_BOX(
    GP.LABEL("SPEED");
    GP.SLIDER("SPEED_SLIDER", 5, 1, 30, 1, 0,Ksesa_PINK);
    );
  GP_MAKE_BOX(
    GP.LABEL("BRIGHTNESSS");
    GP.SLIDER("BRIGHTNESS_SLIDER", BrightnessWS2812B, 1, 255, 1, 0,Ksesa_PINK);
  );
  GP.BUILD_END();
}

void action() {
  if (LampaUI.click()) {
    Serial.println("SOMETHING IS CLICKED");
    if(LampaUI.clickInt("SPEED_SLIDER",runningHueSpeed))
    {
      Serial.print("New hue speed : ");
      Serial.println(runningHueSpeed);
    }
    if(LampaUI.clickInt("BRIGHTNESS_SLIDER",BrightnessWS2812B))
    {
      Serial.print("New brightness : ");
      Serial.println(BrightnessWS2812B);
    }
    if (LampaUI.clickBool("POWER_SW", power_mode)) {
      if(power_mode)
        Serial.println("turn on");
      else
        Serial.println("turn off");
    }
  }
}

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN_D4, GRB>(leds[0], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D3, GRB>(leds[1], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D2, GRB>(leds[2], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D1, GRB>(leds[3], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D8, GRB>(leds[4], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D6, GRB>(leds[5], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D5, GRB>(leds[6], LEDS_PER_STRIP);
  FastLED.addLeds<WS2812B, DATA_PIN_D0, GRB>(leds[7], LEDS_PER_STRIP);
  FastLED.setBrightness(BrightnessWS2812B);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    SetColorStrip(0,CHSV(0,int(255*0.31),255));
    delay(250);
    Serial.print(".");
    SetColorStrip(0,CHSV(0,0,0));
    delay(250);
  }
  Serial.println(WiFi.localIP());

  LampaUI.attachBuild(build);
  LampaUI.attach(action);
  LampaUI.start(mDNS_NAME);
  LampaUI.enableAuth(ADMIN_NICKNAME, ADMIN_PASS);
}

void loop() {
  LampaUI.tick();
  if(power_mode)
  {
    if(FastLED.getBrightness()!= BrightnessWS2812B)
    {
      FastLED.setBrightness(BrightnessWS2812B);
    }
    RunningHuePerStrip();
  }
  else
  {
    for(int i=0;i<NUM_STRIPS;i++)
    {
      SetColorStrip(i,CHSV(0,0,0));
    }
  }
}

void SetColorStrip(int StripNum,CHSV color){
  for(int i=0;i<LEDS_PER_STRIP;i++){
      leds[StripNum][i] = color;
    }
    FastLED.show();
}


void AllHue(){
  for(int k = 0; k < 256; k++){
    for (int i = 0; i < NUM_STRIPS; i++) {
      SetColorStrip(i,CHSV(k,255,255)); 
    }

  // Add a delay for visibility
  delay(250);
  }
}

void HuePerStrip() {
  // Set different hues for each strip
  for (int i = 0; i < NUM_STRIPS; i++) {
    CHSV color = CHSV(i * (256 / NUM_STRIPS), 255, 255);
    SetColorStrip(i, color);
    delay(50);  // Add a delay for visibility
  }
}

void RunningStrip(){
  for(int i=0;i<NUM_STRIPS;i++){
    FastLED.clear();
    SetColorStrip(i,CHSV(0,255,255));
    delay(100);
    }
  }

void FullingStrips(){ // сначала 1 потом 1 и 2 потом 123 так до последней
for(int i=0;i<NUM_STRIPS;i++){
  SetColorStrip(i,CHSV(0,255,255));
  delay(100);
  }
  FastLED.clear();
}

void RunningHuePerStrip() {
    // Set hues based on the running offset_hue
    for (int i = 0; i < NUM_STRIPS; i++) {
      CHSV color = CHSV((offset_hue + i * (256 / NUM_STRIPS)) % 256, 255, 255);
      SetColorStrip(i, color);
    }
    offset_hue = (offset_hue + runningHueSpeed) % 256;  // Update the running offset_hue
    delay(50);  // Add a delay for visibility
}
