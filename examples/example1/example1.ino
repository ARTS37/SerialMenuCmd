/**
 * @file TstLibSMC1
 *
 * @mainpage Serial Menu Command - example N°1
 *
 * @section description Description
 * Example of use of the SerialMenuCmd library. The example consists in enterint 2 
 * operands then in carrying out various arithmetic operations
 *
 * @section circuit Circuit
 * The example only requires the PC and an Arduino UNO board
 * \image html ArduinoUno.svg width=200px
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
 * Copyright (c) 2021 Armand ROLLAND
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 */

/* Includes Files (libraries) =================================================== */
//#include <Arduino.h>       //Library only needed width Visual Studio Code
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
float x, y;

/* Function Prototype(s) ================================================================= */
/**
 * @brief System malfunction is indicated by the led, it flash to form the
 *        morse message "SOS"
 */
void LedInfoUserPanic(void);

/**
 * @brief Initialization of the structure MenuCmdItem.
 *        This structure gather the different elements allowing to
 *        operate a basic CLI (Common Line Interface).
 * 
 *        Every item of the structure is composed as follows :
 *        text of menu
 *        code keybord (ASCII printable, see -> http://facweb.cs.depaul.edu/sjost/it212/documents/ascii-pr.htm)
 *        Fonction to execute (callback)
 */

/* CLI( Command Line Interfaface) making of================================================ */
//Declaration texts of menu
tMenuCmdTxt txt1_enterX[] = "x - give an float for x";
tMenuCmdTxt txt2_enterY[] = "y - give an float for y";
tMenuCmdTxt txt3_displayVal[] = "v - Display values x and y";
tMenuCmdTxt txt4_switchXY[] = "i - invert variables x <=> y";
tMenuCmdTxt txt5_addXY[] = "+ - Perform the operation -> x + y";
tMenuCmdTxt txt6_minusXY[] = "- - Perform the operation -> x - y";
tMenuCmdTxt txt7_mulXY[] = "* - Perform the operation -> x * y";
tMenuCmdTxt txt8_divXY[] = "/ - Perform the operation -> x / y";
tMenuCmdTxt txt9_DisplayMenu[] = "? - Displaying menu";

//Declaration text of prompt
tMenuCmdTxt txt_Prompt[] = "User";

//Prototype of function which are callback by the library
void cmd1_enterX(void);
void cmd2_enterY(void);
void cmd3_displayVal(void);
void cmd4_switchXY(void);
void cmd5_addXY(void);
void cmd6_minusXY(void);
void cmd7_mulXY(void);
void cmd8_divXY(void);
void cmd9_DisplayMenu(void);

//Structure initialisation
//Data type
//array sMenuTxt, code (character) , function (Reminder the code character must be printable character)
stMenuCmd list[] = {
    {txt1_enterX, 'x', cmd1_enterX},
    {txt2_enterY, 'y', cmd2_enterY},
    {txt3_displayVal, 'v', cmd3_displayVal},
    {txt4_switchXY, 'i', cmd4_switchXY},
    {txt5_addXY, '+', cmd5_addXY},
    {txt6_minusXY, '-', cmd6_minusXY},
    {txt7_mulXY, '*', cmd7_mulXY},
    {txt8_divXY, '/', cmd8_divXY},
    //{txt9_DisplayMenu, '?', cmd9_DisplayMenu}
    {txt9_DisplayMenu, '?', []() { myMmuCmd.ShowMenu(); myMmuCmd.giveCmdPrompt();}}};

/**
 * @note
 * it's possible to implement lambda function as shown in the previous example
 * see menu N°9
 */

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

  //Led State Change to indicate to the user that the system is OK
  //Minnimum delay : 100ms
  digitalWrite(LedOnBoard, !digitalRead(LedOnBoard));
  delay(100);
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
  
  ///letter S -> 3 dots 
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
 * @brief get x value enter by the user (keyboard)
 *        Sign => +, - nothing
 *        integer part
 *        decimal separator => '.'
 *        decimal part
 *        example = 0, 1.0, +12.45, -45.2258, 1585
 * 
 * @return * void 
 */
void cmd1_enterX(void)
{
  String aValue = "! Enter a floating value";
  /**
   * @brief the string type argument associated with the getStrValue 
   *        function has a dual use. Input, it contains message to 
   *        guide user, output, it contains the value defined by user.
   *        Warning, value output is string character. It is needed to
   *        convert it to a numeric value (atof function).
   */
  if (myMmuCmd.getStrValue(aValue) == false)
  {
    Serial.println(F(""));
    Serial.println(F("Entry failure"));
  }
  else
  {
    Serial.println(F(""));
    Serial.print(F("x = "));
    //Store in global variable x
    x = atof(aValue.c_str());
    Serial.println(x);
  }
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief get x value enter by the user (keyboard)
 *        Sign => +, - nothing
 *        integer part
 *        decimal separator => '.'
 *        decimal part
 *        example = 0, 1.0, +12.45, -45.2258, 1585
 * 
 * @return * void 
 */
void cmd2_enterY(void)
{
  /**
   * @brief the string type argument associated with the getStrValue 
   *        function has a dual use. Input, it contains message to 
   *        guide user, output, it contains the value defined by user.
   *        Warning, value output is string character. It is needed to
   *        convert it to a numeric value (atof function).
   */
  String aValue = "! Enter a floating value";

  if (myMmuCmd.getStrValue(aValue) == false)
  {
    Serial.println(F(""));
    Serial.println(F("Entry failure"));
  }
  else
  {
    Serial.println(F(""));
    Serial.print(F("y = "));
    //Store in global variable y
    y = atof(aValue.c_str());
    Serial.println(y);
  }
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief Display value x and y
 * 
 * @return * void 
 */
void cmd3_displayVal(void)
{
  Serial.println(F(""));
  Serial.print(F("Values x = "));
  Serial.print(x);
  Serial.print(F("; y = "));
  Serial.print(y);
  Serial.println(F(""));
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief invert value x and y and display them
 *        third = x
 *        x = y
 *        y = third
 * 
 * @return * void 
 */
void cmd4_switchXY(void)
{
  float fOther;

  Serial.println(F(""));
  fOther = x;
  x = y;
  y = fOther;
  Serial.print(F("Values x = "));
  Serial.print(x);
  Serial.print(F("; y = "));
  Serial.print(y);
  Serial.println(F(""));
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief Display Addition x + y
 * 
 * @return * void 
 */
void cmd5_addXY(void)
{
  Serial.println(F(""));
  Serial.print(F("x + y = "));
  Serial.print(x);
  Serial.print(F(" + "));
  Serial.print(y);
  Serial.print(F(" = "));
  Serial.print(x + y);
  Serial.println(F(""));
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief Display subtraction x - y
 * 
 * @return * void 
 */
void cmd6_minusXY(void)
{
  Serial.println(F(""));
  Serial.print(F("x - y = "));
  Serial.print(x);
  Serial.print(F(" - "));
  Serial.print(y);
  Serial.print(F(" = "));
  Serial.print(x - y);
  Serial.println(F(""));
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief Display multiplication x * y
 * 
 * @return * void 
 */
void cmd7_mulXY(void)
{
  Serial.println(F(""));
  Serial.print(F("x * y = "));
  Serial.print(x);
  Serial.print(F(" * "));
  Serial.print(y);
  Serial.print(F(" = "));
  Serial.print(x * y);
  Serial.println(F(""));
  Serial.println(F(""));
  myMmuCmd.giveCmdPrompt();
};

/**
 * @brief Display division x / y
 *        Check if y != 0 else abort
 * 
 * @return * void 
 */
void cmd8_divXY(void)
{
  if (y != 0)
  {
    Serial.println(F(""));
    Serial.print(F("x / y = "));
    Serial.print(x);
    Serial.print(F(" / "));
    Serial.print(y);
    Serial.print(F(" = "));
    Serial.print(x / y);
    Serial.println(F(""));
    Serial.println(F(""));
    myMmuCmd.giveCmdPrompt();
  }
  else
  {
    Serial.println(F(""));
    Serial.print(F("x / y not possible because y = 0"));
    Serial.println(F(""));
    Serial.println(F(""));
    myMmuCmd.giveCmdPrompt();
  }
};

/**
 * @brief DIsplay menu
 * 
 * @return * void 
 */
void cmd9_DisplayMenu(void)
{
  myMmuCmd.ShowMenu();
  myMmuCmd.giveCmdPrompt();
};
