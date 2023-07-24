// RadioLab Superheterodyne Radio Sketch

#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include "si5351.h"
#include "Wire.h"


void cw_transmit( void * pvParameters );

String convertCharacterToMorseCode(char c) {
  String code;
  
  switch(c) {
    case 'a':
    case 'A':
      code = ".-";
      break;
    case 'b':
    case 'B':
      code = "-...";
      break;
    case 'c':
    case 'C':
      code = "-.-.";
      break;
    case 'd':
    case 'D':
      code = "-..";
      break;
    case 'e':
    case 'E':
      code = ".";
      break;
    case 'f':
    case 'F':
      code = "..-.";
      break;
    case 'g':
    case 'G':
      code = "--.";
      break;
    case 'h':
    case 'H':
      code = "....";
      break;
    case 'i':
    case 'I':
      code = "..";
      break;
    case 'j':
    case 'J':
      code = ".---";
      break;
    case 'k':
    case 'K':
      code = "-.-";
      break;
    case 'l':
    case 'L':
      code = ".-..";
      break;
    case 'm':
    case 'M':
      code = "--";
      break;
    case 'n':
    case 'N':
      code = "-.";
      break;
    case 'o':
    case 'O':
      code = "---";
      break;
    case 'p':
    case 'P':
      code = ".--.";
      break;
    case 'q':
    case 'Q':
      code = "--.-";
      break;
    case 'r':
    case 'R':
      code = ".-.";
      break;
    case 's':
    case 'S':
      code = "...";
      break;
    case 't':
    case 'T':
      code = "-";
      break;
    case 'u':
    case 'U':
      code = "..-";
      break;
    case 'v':
    case 'V':
      code = "...-";
      break;
    case 'w':
    case 'W':
      code = ".--";
      break;
    case 'x':
    case 'X':
      code = "-..-";
      break;
    case 'y':
    case 'Y':
      code = "-.--";
      break;
    case 'z':
    case 'Z':
      code = "--..";
      break;
    case '1':
      code = ".----";
      break;
    case '2':
      code = "..---";
      break;
    case '3':
      code = "...--";
      break;
    case '4':
      code = "....-";
      break;
    case '5':
      code = ".....";
      break;
    case '6':
      code = "-....";
      break;
    case '7':
      code = "--...";
      break;
    case '8':
      code = "---..";
      break;
    case '9':
      code = "----.";
      break;
    case '0':
      code = "-----";
      break;
    case ' ':
      code = "    ";
    default:
      code = "";
      break;
  }
  return code;
}

//------------------------------- TFT Display Init ------------------------------//
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
void setupDisplay(void)
{
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
}

//------------------------------- Encoder Init ------------------------------//
#define ROTARY_ENCODER_A_PIN 34
#define ROTARY_ENCODER_B_PIN 35
#define ROTARY_ENCODER_BUTTON_PIN 32
#define ROTARY_ENCODER_STEPS 4
#define ROTARY_ENCODER_VCC_PIN -1
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);
long lastEncoderReading = 0;

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}

void setupEncoder(void)
{
  //we must initialize rotary encoder
	rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
	rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
}

//------------------------------- Si5351 Init ------------------------------//
Si5351 si5351(0x60);
#define IF_FILTER_PEAK (4914727-800)
//#define IF_FILTER_PEAK (4915151)

//#define IF_FILTER_PEAK (9216283)


int slelectedClock = 0;
uint64_t clk_1_frequency = IF_FILTER_PEAK + 7023000; 
uint64_t clk_2_frequency = IF_FILTER_PEAK;
uint64_t clk_3_frequency = 0;

void setupSi5351()
{
  bool i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, 0, 0);
  if(!i2c_found)
  {
    Serial.println("Device not found on I2C bus!");
  }
  else
    Serial.println("Device found on I2C bus!");  

  si5351.set_correction(146999, SI5351_PLL_INPUT_XO);

  si5351.drive_strength(SI5351_CLK0,SI5351_DRIVE_2MA);
  si5351.drive_strength(SI5351_CLK1,SI5351_DRIVE_2MA);

  si5351.set_freq(clk_1_frequency*100, SI5351_CLK0);
  si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);

  si5351.update_status();
}


void updateTFT()
{

  String freq = String(clk_1_frequency-IF_FILTER_PEAK);
  tft.drawString(freq+"+IF",60,30,2);

  freq = String(clk_2_frequency);
  tft.drawString(freq,60,50,2);

  freq = String(clk_3_frequency);
  tft.drawString(freq,60,70,2);  

  tft.drawString("  ",145,30,2);
  tft.drawString("  ",145,50,2);
  tft.drawString("  ",145,70,2);

  switch(slelectedClock)
  {
  case 0: tft.drawString("<-",145,30,2);  break;
  case 1: tft.drawString("<-",145,50,2);  break;
  case 2: tft.drawString("<-",145,70,2);  break;
  }
}




#define SW_4_PIN 19
#define SW_3_PIN 16
#define SW_2_PIN 4
//#define SW_ENCODER_PIN 32


#define TX_RX_Pin 1
#define TX_EN_Pin 3

TaskHandle_t Task1;

void setup()
{
   Serial.begin(115200);
   while(!Serial);

  setupDisplay();
  setupEncoder();
  setupSi5351();
 
  tft.fillRect(0,0, 160, 128, TFT_BLACK);    

  tft.drawString(" www.RADIOBUILDER.org",5,5,2);

  tft.drawString(" OSC-1 ",5,30,2);
  tft.drawString(" OSC-2 ",5,50,2);
  tft.drawString(" OSC-3 ",5,70,2);

  tft.fillRoundRect(5, 108,30,15,3, TFT_BLUE); 
  tft.fillRoundRect(45, 108,30,15,3, TFT_RED); 
  tft.fillRoundRect(85, 108,30,15,3, TFT_GREEN); 
  tft.fillRoundRect(125, 108,30,15,3,TFT_YELLOW);

  updateTFT();

  pinMode(TX_RX_Pin, OUTPUT);
  pinMode(TX_EN_Pin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(27, OUTPUT);
  digitalWrite(27,0);
  digitalWrite(12,1);

  digitalWrite(TX_RX_Pin,1);
  digitalWrite(TX_EN_Pin,0);


  int ledPin = 33;
  int freq = 600;
  int ledChannel = 0;
  int resolution = 8;

  ledcSetup(ledChannel, freq, resolution);  
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, 127);

  pinMode(SW_2_PIN,INPUT);
  pinMode(SW_3_PIN,INPUT);

  //SendSequence("CQ CQ CQ De EI8KI EI8KI EI8KI K");

  xTaskCreatePinnedToCore(
      cw_transmit, /* Function to implement the task */
      "cw_transmit", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */
}

#define DOT_TIME 100


void  SendDot(void)
{
  digitalWrite(27,1);
  digitalWrite(TX_RX_Pin,0); 
  digitalWrite(TX_EN_Pin,1); 
  delay(DOT_TIME);

}

void SendDash(void)
{
  SendDot();
  SendDot();
  SendDot();
}

void SendSpace(int numberSpaces)
{
  // digitalWrite(12,0);  // Mute Audio
  // delay(5);
  digitalWrite(27,0);
  digitalWrite(TX_RX_Pin,1);
  digitalWrite(TX_EN_Pin,0);
  delay(DOT_TIME*numberSpaces);
  // digitalWrite(12,1);  // Mute Audio
  // delay(5);

}

void SendSequence(String sequence)
{
  si5351.set_freq(0, SI5351_CLK1);
  si5351.set_freq((clk_1_frequency-800-IF_FILTER_PEAK)*100, SI5351_CLK2);  

  for(unsigned int i=0;i<sequence.length();i++)
  {

    if(digitalRead(SW_3_PIN) == true){
      SendSpace(1);

      digitalWrite(12,0);  // Mute Audio
      delay(5);
      si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);
      si5351.set_freq(0, SI5351_CLK2);  
      delay(5);
      digitalWrite(12,1);
      return;
      }

    String str = convertCharacterToMorseCode(sequence[i]);

    for(unsigned int j=0;j<str.length();j++)
      {
      char c = str.charAt(j);

      if(c == '.'){
        SendDot();
        SendSpace(1);
        }
      if(c == '-' ){
        SendDash();
        SendSpace(1);
        }
      if(c == ' ' )
        SendSpace(1);
    }
    SendSpace(3);
  }



  

  

  digitalWrite(12,0);  // Mute Audio
  delay(5);
  si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);
  si5351.set_freq(0, SI5351_CLK2);  
  delay(5);
  digitalWrite(12,1);

}


float value = 0.0f;
int sign = 1;

#define IDLE      0
#define RAMP_UP   1
#define STEADY    2
#define RAMP_DOWN 3
#define QSK       4

int state = IDLE;
float amplitude = 0.0f;
float volume = 0.5;
int debounce = 0;
int qsk_counter = 0;

#define RAMP_VALUE    0.005
#define MAX_VALUE     2*3.14
#define SMAPLE_SIZE   0.20;
#define DEBOUNCE_TIME 000
#define QSK_TIME      500

void loop() {
static int idle = true;

  bool KeyDown = digitalRead(SW_4_PIN);
  if(KeyDown == 1){
    digitalWrite(27,1);
    si5351.set_freq(0, SI5351_CLK1);
    si5351.set_freq((clk_1_frequency-800-IF_FILTER_PEAK)*100, SI5351_CLK2);  
    digitalWrite(TX_RX_Pin,0); 
    digitalWrite(TX_EN_Pin,1); 
    idle = false;
    qsk_counter = 0;
    }
  else if(idle==false)
    {
    digitalWrite(27,0);
    digitalWrite(TX_RX_Pin,1);
    digitalWrite(TX_EN_Pin,0);
    qsk_counter++;
      if(qsk_counter >= QSK_TIME){
        digitalWrite(12,0);  // Mute Audio
        delay(5);
        si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);
        //si5351.set_freq(clk_1_frequency*100, SI5351_CLK0);
        qsk_counter=0;
        si5351.set_freq(0, SI5351_CLK2);  
        idle = true;  
        delay(5);
        digitalWrite(12,1);

      }

    }


    delay(1);

    static bool SendingSequence = false;

    bool SendSeq = digitalRead(SW_2_PIN);
    if(SendSeq == true)
      SendingSequence = true;

    if(SendingSequence == true){
      SendSequence("CQ CQ CQ De EI8KI EI8KI EI8KI PSE K");
      for(int i=0;i<5000;i++)
        {
        if(digitalRead(SW_3_PIN) == true)
          SendingSequence = false;
        delay(1);
        }
      }
}

  





void cw_transmit( void * pvParameters ){
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  pinMode(SW_4_PIN, INPUT);

  for(;;){
    if (rotaryEncoder.encoderChanged())
    {
    Serial.print("Encoder Changed ");

      long currentReading = rotaryEncoder.readEncoder();
      long change =  currentReading -lastEncoderReading;

      lastEncoderReading = currentReading;

      switch(slelectedClock)
      {
      case 0: clk_1_frequency += change*100;  break;
      case 1: clk_2_frequency += change*100;  break;
      case 2: clk_3_frequency += change*10000;  break;
      }
      updateTFT();
      si5351.set_freq(clk_1_frequency*100, SI5351_CLK0);
      si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);

      
    }


    if (rotaryEncoder.isEncoderButtonClicked(1))
    {
      Serial.print("Button Clicked ");
      slelectedClock++;
      if(slelectedClock>2)
      slelectedClock = 0;

      updateTFT();
    } 
  }
}
