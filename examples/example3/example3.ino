/**
 * @file main
 *
 * @mainpage Serial Menu Command - exemple N°3
 *
 * @section description Description
 * Exemple of use of the Serial MenuCmd library. the example consists of configuring the
 * time of an RTC (real time clock) and reading the data on a DS1302 integrated circuit.
 * This example highlights the function to get a date/time group from a character string.
 *
 * @section circuit Circuit
 * Works based on boards arduino uno and prototype DS1302
 *
 * @section libraries Libraries
 * - SerialMenuCmd   -> Common line interface (user <-> Embedded system by serial communication)
 * - ThreeWire       -> Communication protocol with the integrated circuit DS1302 (must be declared before RtsDS1302)
 * - RtcDS1302       -> Real time clock manager for DS1302 IC's ("Rtc by Makuna" on Arduino Library List site's)
 *
 * @section notes Notes
 * none at this time
 *
 * @section todo TODO
 * none at this time
 *
 * @version version Version
 * V1.0.0, February 2022, initial release, Armand ROLLAND
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

/* Includes Files (libraries) ================================================================================= */
#include <Arduino.h>
#include <SerialMenuCmd.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

/* Macros ===================================================================================================== */
#define LedOnBoard 13

/* Class (instanciation) ====================================================================================== */
SerialMenuCmd SMC;
ThreeWire myWire(4, 5, 2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

/* Variables ================================================================================================== */

/* Function Prototype(s) ====================================================================================== */
/**
 * @brief System malfunction is indicated by the led, it flash to form the
 *        morse message "SOS"
 */
void LedInfoUserPanic(void);

/* Structure list of command ================================================================================== */

// Declaration texts of menu
tMenuCmdTxt txt1_DisplayMenu[] = "? - Menu";
tMenuCmdTxt txt2_Status[] = "s - Status";
tMenuCmdTxt txt3_DispData[] = "d - display date and time";
tMenuCmdTxt txt4_SetupTime[] = "t - setup date and time";

// Declaration text of prompt
tMenuCmdTxt txt_Prompt[] = "User";

// Prototype of function which are callback by the library
void cmd1_DisplayMenu(void);
void cmd2_Status(void);
void cmd3_DispData(void);
void cmd4_SetupTime(void);

// Initialization of structure
// type of data
// array sMenuTxt, code character , function   (Reminder the code character must be printable character)
stMenuCmd list[] = {
    {txt1_DisplayMenu, '?', cmd1_DisplayMenu},
    {txt2_Status, 's', cmd2_Status},
    {txt3_DispData, 'd', cmd3_DispData},
    {txt4_SetupTime, 't', cmd4_SetupTime}};

// NbCmds contains the number of command
#define NbCmds sizeof(list) / sizeof(stMenuCmd)

/*==============================================================================================================*/
/*=================== setup and loop functions implementation ==================================================*/
/*==============================================================================================================*/
void setup()
{
  // uint8_t i;

  /**
   * @brief Led on board configh
   */
  // Configure the pin to wich  the Led on board the card is connected
  pinMode(LedOnBoard, OUTPUT);
  // switch off the led
  digitalWrite(LedOnBoard, LOW);

  /**
   * @brief Serial port com Init
   *         variable fgConnToPC = true if PC connected
   */
  Serial.begin(115200);

  /**
   * @brief Enable RTC (real-time clock peripheral, DS1302 ic's)
   */
  Rtc.Begin();
  if (!Rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
  }
  else
  {
    if (Rtc.GetIsWriteProtected())
    {
      Serial.println("RTC was write protected, enabling writing now");
    }

    if (!Rtc.GetIsRunning())
    {
      Serial.println("RTC was not actively running");
    }
  }

  /**
   * @brief init. Common line interface for dialog between user en system by Serial port com.
   */
  if (SMC.begin(list, NbCmds, txt_Prompt) == false)
  {
    // If the initialization fails, the system informs the user via the
    // led: panic mode, led flashes in Morse code the letters "SOS"
    while (true)
    {
      // disp info PB menu cmd
      LedInfoUserPanic();
    }
  }
  // show menu and give to user the prompt
  SMC.ShowMenu();
  SMC.giveCmdPrompt();
}

void loop()
{

  uint8_t CmdCode;

  /**
   * @brief management of the interaction between the system and the user.
   * The "UserRequest" menbre function analyzes the characters transmitted by the user.
   * If an command code is identified, its number is returned (return 0 if no command). This
   * function is not blocking, it stores the intermediate data between 2 calls.
   */
  CmdCode = SMC.UserRequest();

  // possible pre-treatment here

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
    SMC.ExeCommand(CmdCode);
  }

  // Led State Change to indicate to the user that the system is OK
  // Minnimum delay : 100ms
  digitalWrite(LedOnBoard, !digitalRead(LedOnBoard));
  delay(100);
}

/*==============================================================================================================*/
/*==================== Application's functions implementation ==================================================*/
/*==============================================================================================================*/

/**
 * @fn Flash led on-board in morse code
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

  /// letter S -> 3 dots
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(600); // inter-character space

  /// Letter O -> 3 dash
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(600);
  digitalWrite(LedOnBoard, LOW);
  delay(600); // inter-character space

  /// letter S -> 3 dots
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(200); // intra-chacracter
  digitalWrite(LedOnBoard, HIGH);
  delay(200);
  digitalWrite(LedOnBoard, LOW);
  delay(1400); // word space
}

/**
 * @fn cmd1 CLI -> show menu and give to user the prompt
 */
void cmd1_DisplayMenu(void)
{
  SMC.ShowMenu();
  SMC.giveCmdPrompt();
};

/**
 * @fn cmd2 CLI -> show status (WavPlayer, RTC, SD memory is OK?)
 */
void cmd2_Status(void)
{
  Serial.println("");
  
  if (Rtc.IsDateTimeValid())
  {
    Serial.println("RTC is working");
  }
  else
  {
    Serial.println("RTC error !!");
  }

  SMC.giveCmdPrompt();
}

/**
 * @fn cmd3 CLI -> Show data (date/time, temprature, barometric pressure and relative humudity)
 */
void cmd3_DispData(void)
{
  // RTC_Date tNowandHere;
  RtcDateTime tNowandHere;
  char bufMess[20];
  uint8_t memSec = 0;

  Serial.println("");

  do
  {
    tNowandHere = Rtc.GetDateTime();
    if (tNowandHere.Second() != memSec)
    {
      memSec = tNowandHere.Second();
      sprintf(bufMess, "%04d-%02d-%02dT%02d:%02d:%02d", tNowandHere.Year(), tNowandHere.Month(), tNowandHere.Day(), tNowandHere.Hour(), tNowandHere.Minute(), tNowandHere.Second());
      Serial.write(bufMess);
      Serial.println("");
    }
    delay(100);
  } while (!Serial.available());

  while (Serial.available())
  {
    memSec = Serial.read();
  }

  SMC.giveCmdPrompt();
}

/**
 * @fn cmd4 CLI -> Set date and time
 */
void cmd4_SetupTime(void)
{
  //                   12345678901234567890123456789012345678901234567890123
  String stDateHour = "Enter date and time = yyyy-MM-ddThh:mm:ss";
  stDateTimeGroup stDTgConv;
  RtcDateTime tNowandHere;

  if (SMC.getStrOfChar(stDateHour) == true)
  {
    if (SMC.ConvStrToDTg(stDateHour, stDTgConv) == true)
    {
      Rtc.SetDateTime(RtcDateTime(stDTgConv.u16Year, stDTgConv.u8Month, stDTgConv.u8Day, stDTgConv.u8Hour, stDTgConv.u8Min, stDTgConv.u8Sec));

      tNowandHere = Rtc.GetDateTime();
      Serial.println("");
      Serial.println(F("Successful time and date setting"));
      Serial.print(tNowandHere.Year());
      Serial.print("-");
      Serial.print(tNowandHere.Month());
      Serial.print("-");
      Serial.print(tNowandHere.Day());
      Serial.print(" ");
      Serial.print(tNowandHere.Hour());
      Serial.print(":");
      Serial.print(tNowandHere.Minute());
      Serial.print(":");
      Serial.println(tNowandHere.Second());
    }
    else
    {
      Serial.println("");
      Serial.println(F("Failed to set time and date"));
    }
  }

  SMC.giveCmdPrompt();
}
