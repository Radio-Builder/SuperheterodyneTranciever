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

#define DOT_TIME 100


void  SendDot(void)
{
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
  digitalWrite(TX_RX_Pin,1);
  digitalWrite(TX_EN_Pin,0);
  delay(DOT_TIME*numberSpaces);

}

void SendSequence(String sequence)
{
  si5351.set_freq((clk_1_frequency-200-IF_FILTER_PEAK)*100, SI5351_CLK1);
  si5351.set_freq((clk_1_frequency-800-IF_FILTER_PEAK)*100, SI5351_CLK2);  

  for(unsigned int i=0;i<sequence.length();i++)
  {

    if(digitalRead(SW_3_PIN) == true){
      SendSpace(1);

      //digitalWrite(12,0);  // Mute Audio
      delay(5);
      si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);
      si5351.set_freq(0, SI5351_CLK2);  
      delay(5);
      //digitalWrite(12,1);
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



  

  

  //digitalWrite(12,0);  // Mute Audio
  delay(5);
  si5351.set_freq(clk_2_frequency*100, SI5351_CLK1);
  si5351.set_freq(0, SI5351_CLK2);  
  delay(5);
  //digitalWrite(12,1);

}