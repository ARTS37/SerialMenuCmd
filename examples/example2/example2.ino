/**
 * @file TstLibSMC2
 *
 * @mainpage Serial Menu Command - example N°2
 *
 * @section description Description
 * Example of use of the SerialMenuCmd library. The example consists of
 * switch on, off or blinking the led on board the card. In the case of 
 * flashing, the user can set the duration of the LED on and off.
 * 
 *
 * @section circuit Circuit
 * The example only requires the PC and an Arduino UNO board
 *
 * @section libraries Libraries
 * - SerialMenuCmd
 * 
 * @section notes Notes
 * none at this time
 * 
 * @section todo TODO
 * none at this time
 *
 * @version version Version
 * V1.0.0, May 05th 2021, initial release, Armand ROLLAND
 * 
 * @section author Author
 * Armand ROLLAND -> armand.rolland71@gmail.com
 * 
 * @copyright copyright MIT Licence
 * Copyright (c) 2021 Armand ROLLAND - Europe, France, Bretagne, Rennes
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Includes Files (libraries) =================================================== */
#include <SerialMenuCmd.h> //Library referred to in this example

/* Macros =============================================================================== */
#define LedOnBoard 13

/* Class (instanciation) =============================================================== */
/**
 * @brief Instanciation of the SerialMenuCmd library (Class)
 *        This library allows thez user(développeur) to implement a basic CLI (Common Line Interface).
 *        Operation is based on the serial monitor, for the user it consists of consulting a menu
 *        and then activating a command by sending the corresponding code.
 *        
 *        example menu (the text must include the corresponding code or othewise the key):
 *        a - todo command 1 
 *        b - todo command 2
 *        1 - todo command 3
 *        * - todo command ....
 * 
 *        The menu items are put together in a structure MenCmdItem, see further in the file
 */
SerialMenuCmd myMmuCmd;

/* Variables ============================================================================= */
uint8_t LedStatus;
uint16_t LedOnTime;
uint16_t LedOffTime;
uint32_t tEvent;
String Message;

/* Function Prototype(s) ================================================================= */
void LedInfoUserPanic(void);

/* CLI( Command Line Interfaface) making of================================================ */
//Declaration texts of menu
tMenuCmdTxt txt1_LedOn[] = "1 - Switch On the Led";
tMenuCmdTxt txt2_LedOff[] = "0 - Switch Off the Led";
tMenuCmdTxt txt3_LedBlink[] = "2 - Led Blink";
tMenuCmdTxt txt4_ConfOnD[] = "n - Config. time Led on";
tMenuCmdTxt txt5_ConfOffD[] = "f - Config. time Led off";
tMenuCmdTxt txt6_ConfMess[] = "t - Set up Text Message";
tMenuCmdTxt txt7_Status[] = "s - Status";
tMenuCmdTxt txt8_Menu[] = "m - menu";

tMenuCmdTxt txt3_DisplayMenu[] = "? - Displaying menu";

//Declaration text of prompt
tMenuCmdTxt txt_Prompt[] = "Master";

//Prototype of function which are callback by the library
void cmd1_LedOn(void);
void cmd2_LedOff(void);
void cmd3_Blink(void);
void cmd4_ConfOnD(void);
void cmd5_ConfOffD(void);
void cmd6_ConfMess(void);
void cmd7_Status(void);
void cmd8_Menu(void);

//Structure initialisation
//Data type
//array sMenuTxt, code (character) , function (Reminder the code character must be printable character)
stMenuCmd list[] = {
    {txt1_LedOn, '1', cmd1_LedOn},
    {txt2_LedOff, '0', cmd2_LedOff},
    {txt3_LedBlink, '2', cmd3_Blink},
    {txt4_ConfOnD, 'n', cmd4_ConfOnD},
    {txt5_ConfOffD, 'f', cmd5_ConfOffD},
    {txt6_ConfMess, 't', cmd6_ConfMess},  
    {txt7_Status, 's', cmd7_Status},  
    {txt8_Menu, 'm', cmd8_Menu}
};

//KmenuCount contains the number of command
#define NbCmds sizeof(list) / sizeof(stMenuCmd)


/* Functions Implementation =================================================== */

/**
 * @brief Standard function Arduino for initialisation
 * 
 */
void setup()
{
  //Activate and initialize the serial bus with the baudrate passed in parameter
  Serial.begin(115200);

  //Configure the pin to wich  the Led on board the card is connected
  pinMode(LedOnBoard, OUTPUT);
  //switch off the led
  digitalWrite(LedOnBoard, LOW);

  //Reset variables
  LedStatus = 0;
  LedOnTime = 1000;
  LedOffTime = 1000;
  tEvent = 0;
  Message = "";

  //Initialize the library with structure defined by the user
  if (myMmuCmd.begin(list, NbCmds, txt_Prompt) == false)
  {
    //If the initialization fails, the system informs the user via the
    //led: panic mode, led flashes in Morse code the letters "SOS"
    while (true)
    {
      LedInfoUserPanic();
    }
  }

  //Display the menu
  myMmuCmd.ShowMenu();
  myMmuCmd.giveCmdPrompt();
 
}

/**
 * @brief Function standard Arduino to repeat the tasks
 * 
 */
void loop()
{
  uint8_t CmdCode;

  /**
   * @brief management of the interaction between the system and the user.
   * The "UserRequest" menbre function analyzes the characters transmitted by the user. 
   * If an command code is identified, its number is returned (return 0 if no command). This 
   * function is not blocking, it stores the intermediate data between 2 calls. 
   */
  CmdCode = myMmuCmd.UserRequest();

  //possible pre-treatment here

  /**
   * @brief Execute Command
   * if a command code is returned, the system executes the corresponding command. 
   * To do this, it uses the "OpsCallback" member function. this function receives 
   * the command code parameter
   * 
   * @note In this way, it is possible to carry out a preprocessing and postprocessing
   */
  if (CmdCode != 0)
  {
    myMmuCmd.ExeCommand(CmdCode);
  }
  

  /**
   * @brief Switch on, off or blink the led
   * 
   */
  switch(LedStatus)
  {
    case 0: //Switch off 
      //before switching off the led check that it is on
      if(digitalRead(LedOnBoard) == true){
        digitalWrite(LedOnBoard, false);
      }
      break;

    case 1: //switch on led
      //before switching on the led check that it is off
      if(digitalRead(LedOnBoard) == false){
        digitalWrite(LedOnBoard, true);
      }
      break;

    case 2:
      //if reached event duration
      //tEvent is a timestamp in millisecond
      if(millis() >= tEvent)
      {
        //if led is off
        if(digitalRead(LedOnBoard) == true)
        {
          //Programm next event with led duration off
          tEvent = millis() + LedOffTime;
          digitalWrite(LedOnBoard, false);
        }
        else
        {
          //Programm next event with led duration on
          tEvent = millis() + LedOnTime;
          digitalWrite(LedOnBoard, true);
        }
      }
      break;

    default:
      break;
  }
}

/**
 * @brief Flash led on-board in morse code
 * 
 */
void LedInfoUserPanic(void)
{
  /**
   * @brief Timing Morse
   * Letter S -> 3 dot (short mark)
   * Letter O -> 3 dash (long mark)
   * 1 dot => 1 unit time
   * 1 dash => 3 units time
   * 1 intra-character space => 1 units time
   * 1 inter-character space => 3 units time
   * 1 word space => 7 untis time  
   * 
   */
  
  //letter S -> 3 dots 
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(600); //inter-character space

  ///Letter O -> 3 dash 
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(200);  //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(200);  //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(600); //inter-character space

  ///letter S -> 3 dots 
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200);  //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200);  //intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(1400); //word space
}


/**
 * @brief 
 * 
 */
void cmd1_LedOn(void)
{
  LedStatus = 1;
  Serial.println("");
  Serial.println(F("Led switch ON"));
  myMmuCmd.giveCmdPrompt();
}


/**
 * @brief 
 * 
 */
void cmd2_LedOff(void)
{
  LedStatus = 0;
  Serial.println("");
  Serial.println(F("Led switch OFF"));
  myMmuCmd.giveCmdPrompt();
}

/**
 * @brief 
 * 
 */
void cmd3_Blink(void)
{
  LedStatus = 2;
  tEvent = 0;
  Serial.println("");
  Serial.println(F("Led BLINK"));
  myMmuCmd.giveCmdPrompt();
}

/**
 * @brief 
 * 
 */
void cmd4_ConfOnD(void)
{
 //                        1         2         3         4         5
 //               12345678901234567890123456789012345678901234567890123
 String aValue = "! Enter config of LED switch on time ( value in ms) ";

  if(myMmuCmd.getStrValue(aValue) == true)
  {
    Serial.println(F(""));
    Serial.print(F("Duration of LED switch on = "));
    LedOnTime = atoi(aValue.c_str());
    Serial.println(LedOnTime);
  }
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
}

void cmd5_ConfOffD(void)
{
 String aValue = "! Enter config of LED switch off time ( value in ms) ";

  if(myMmuCmd.getStrValue(aValue) == true)
  {
    Serial.println(F(""));
    Serial.print(F("Duration of LED switch off = "));
    LedOffTime = atoi(aValue.c_str());
    Serial.println(LedOffTime);
  }
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
}

void cmd6_ConfMess(void)
{
 String aValue = "! Enter message text (64 characters max.) ";

  if(myMmuCmd.getStrOfChar(aValue) == true)
  {
    Serial.println(F(""));
    Serial.print(F("Message = "));
    Message = aValue;
    Serial.println(Message);
  }
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();

}

/**
 * @brief 
 * 
 */
void cmd7_Status(void)
{
  Serial.println(F(""));
  Serial.println(F("General status :"));
  Serial.print(F("- LED status = "));
  switch(LedStatus)
  {
    case 0:
      Serial.println(F("off"));
      break;

    case 1:
      Serial.println(F("on"));
      break;

    case 2:
      Serial.println(F("blink"));
      break;

    default:
      Serial.println(F("undetermined"));
      break;
  }

  Serial.print(F("- Duration LED on (ms) = "));
  Serial.println(LedOnTime);

  Serial.print(F("- Duration LED off (ms) = "));
  Serial.println(LedOffTime);


  Serial.print(F("- Message = "));
  Serial.println(Message);
  myMmuCmd.giveCmdPrompt();
}

/**
 * @brief 
 * 
 */
void cmd8_Menu(void)
{
  myMmuCmd.ShowMenu();
  myMmuCmd.giveCmdPrompt();
}
