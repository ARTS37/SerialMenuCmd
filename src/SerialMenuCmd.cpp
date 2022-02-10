/**
 * @file SerialMenuCmd.cpp
 *
 * @mainpage Serial Menu Command header file
 *
 * @section description Description
 * This file defines (implements) member functions of the class
 *  
 * 
 * @section circuit Circuit
 * This library is not attached to a device.
 * 
 * @section libraries Libraries
 * the library itself and avr/pgmspace.h to store data in memory program space
 * when used in the visual studio Code environment, the arduino.h library has been added
 * 
 * @section notes Notes
 * none at this time
 * 
 * @section todo TODO
 * Test and adapt if necessary on other target (i.e. esp32)
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


#include <Arduino.h>
#include <SerialMenuCmd.h>
#include <SerialMenuCmdLang.h>

SerialMenuCmd::SerialMenuCmd()
{
  /**
   * @brief default value is 0 or nullptr.
   * 
   */
  mMenuCmds = nullptr;
  mPromptText = nullptr;
  mNbCmd = 0;
}

bool SerialMenuCmd::begin(stMenuCmd list[], uint8_t NbCmd, const char *acPrompt)
{
 /**
  * @brief  Before assigning the data, they are verified. In case of error the 
  *         function displays a message and returns false
  * 
  */
 uint8_t i;

  if ((NbCmd == 0) || (list == nullptr) || (acPrompt == nullptr))
  {
    //Send to user : "structure, number of commands or text of prompt are invalid"
    Serial.println(F(TXT_fctMemberBegin_InvalidArguments));
    return false;
  }

  for(i=0; i<NbCmd; i++)
  {
    if(strlen_P(list[i].itemTxt) > NB_MAX_CHAR_IN_TXT)
    {
      //Send to user : "one or more menu text exceeds the limit of the number of characters allowed"
      Serial.println(F(TXT_fctMemberBegin_OversizeCmdTxt));
      return false;      
    }
  }

  if(strlen_P(acPrompt) > NB_MAX_CHAR_IN_TXT)
  {
    //Send to user : "text of prompt exceeds the limit of number of characters allowed"
    Serial.println(F(TXT_fctMemberBegin_OversizePrompt));
    return false;
  }

  mMenuCmds = list;
  mNbCmd = NbCmd;
  mPromptText = acPrompt;
  return true;
}


int8_t SerialMenuCmd::ExeCommand(uint8_t numItem)
{
/**
 * @brief Execute function pointed by the structure. Before, checks if index is valid.
 * 
 *        Note : this function isn't blocking, but the function callback can be.
 * 
 */
  if ((numItem > mNbCmd) || (numItem == 0))
  {
    return MENUCMD_ERROR_OVERRANGE_INDEX;
  }

  //Warning, For the user, the menu list ranges from 1 to n and for the system it is from 0 to n-1
  mMenuCmds[numItem-1].itemPtFunct();
  return true;
}


uint8_t SerialMenuCmd::searchCode(char car)
{
/**
 * @brief compares key with code of each command du menu. 
 *        Stops if matching ok or reaches the end of list. 
 *        Return command numéro if matching otherwise 0.
 *        Warning, number command if index of the command in array + 1
 *        for memory, index array -> 0 to n-1 command
 */
  
  uint8_t i;
  bool fDetect = false;

  for (i = 0; i < mNbCmd; i++)
  {
    if (car == mMenuCmds[i].itemCode)
    {
      fDetect = true;
      break;
    }
  }

  if (fDetect == true)
  {
    return i + 1;
  }
  else
  {
    return 0;
  }
}


uint8_t SerialMenuCmd::getNbCmds(void)
{
  return mNbCmd;
}


void SerialMenuCmd::ShowMenu(void)
{
/**
 * @brief for each menu command, the system displays the corresponding text
 *        however it is first checked that there are commands.
 *        regarding the length of character strings, they are checked at 
 *        initialization time (see the "begin" member function)
 */
 
  uint8_t i;
  char sMenuN[NB_MAX_CHAR_IN_TXT];

  Serial.println("");
  Serial.println("");
  if(mNbCmd == 0){
    //Send to user : "No Menu in memory"
    Serial.println(F(TXT_fctMemberShowMenu_NoMenu));

    //Send to user : "PB to interface with library SerialSerialMenuCmd")
    Serial.println(F(TXT_fctMemberShowMenu_PBInterf));
    return;
  }

  for (i = 0; i < mNbCmd; i++)
  {
    //notice : strlcpy_p copy a maximum of NbMaxChar - 1 (last character is NULL terminate)
    strlcpy_P(sMenuN, mMenuCmds[i].itemTxt, NB_MAX_CHAR_IN_TXT);
    Serial.println(sMenuN);  
  }
}


void SerialMenuCmd::giveCmdPrompt(void)
{
/**
 * @brief the command prompt consists of the text provided by the 
 *        user (see, function member "begin") and the sign '>'
 * 
 */
 char sMenuN[NB_MAX_CHAR_IN_TXT]; 

  Serial.println(F(""));
  strlcpy_P(sMenuN, mPromptText, NB_MAX_CHAR_IN_TXT);
  Serial.print(sMenuN);
  Serial.print(F(">"));
}


uint8_t SerialMenuCmd::UserRequest(void)
{
/**
 * @brief On each call, the function checks for the presence of a
 *        character in the serial stream. Each character is operated 
 *        individually. 
 *        Note : between 2 calls, the data is stored (variable with 
 *        static prefix to avoid declaring a global variable).
 *        
 *        Operating mode :
 *        a single keystroke (printable character) memorized, beyond that, t
 *        he user is warned that there are too many arguments (reset memory).
 *        For the other characters, only 'Carrier Return', 'Escape' and 
 *        'Backspace' are managed.
 * 
 *        Note : this function isn't blocking.
 */

  static char ValideCode;
  char kbEnter;
  uint8_t CodeCmd;

  while (Serial.available() > 0)
  {
    kbEnter = Serial.read();
    if(isGraph(kbEnter))  //is printer character ?
    {
      Serial.write(kbEnter);

      if (ValideCode == 0)
      {
        ValideCode = kbEnter;
      }
      else
      {
        //if multiple characters valideCode = 0x15 (character not printable)
        ValideCode = 0x15;
      }
    }
    else
    {
      switch (kbEnter)
      {
      case 0x0A: //Enter key pressed
        if(isGraph(ValideCode))
        {
          CodeCmd = this->searchCode(ValideCode);
          if (CodeCmd > 0)  //if this character corresponds to a command code
          {
            ValideCode = 0;
            return CodeCmd; //return number command
          }
          else
          {
            //else inform the user that there is no command code
            ValideCode = 0;
            //Send to user : " -> No command code matching !"
            Serial.println(F(TXT_fctMemberUserRequest_NoMatching));
            Serial.println(F(""));
            this->giveCmdPrompt();
          }
        }
        else
        {
          if(ValideCode == 0x15)
          {
            //Send to user : "-> too many arguments (one character only), !"
            Serial.println(F(TXT_fctMemberUserRequest_TooArgs));
          }
          this->giveCmdPrompt();
        }
        break;

      case 0x08: // Backspace pressed
        if (ValideCode != 0)
        {
          //delete if only one character memorized (validecode != 0x15) otherwise abort
          if(ValideCode != 0x15)
          {
            Serial.write(kbEnter);  //send back backspace character
          }
          else
          {
            //Send to user : "-> too many arguments (one character only), !"
            Serial.print(F(TXT_fctMemberUserRequest_TooArgs));
            //Send to user : " -> cancellation !"
            Serial.println(F(TXT_fctMemberUserRequest_Cancellation));
            this->giveCmdPrompt();
          }
          ValideCode = 0;
        }
        break;

      case 0x1B: // Escape pressed
        //Send to user : " -> cancellation !"
        Serial.println(F(TXT_fctMemberUserRequest_Cancellation));
        this->giveCmdPrompt();
        ValideCode = 0;
        break;

      default:

        break;
      }
    }
  }
  return 0;
}


bool SerialMenuCmd::getStrValue(String& sMessValue)
{
/**
 * @brief this function retrieves the serial flux. if character
 *        is "digital" (0123456789) or sign + or - (as first 
 *        received character) or decimal separator '.' (a single 
 *        occurence), it stored.
 *        the memorized character string is returned when user 
 *        valid by Enter key. the function return 'true' value. 
 *        Backspace key delete the last character.
 *        Escape key interrupts the sequence : the function ends 
 *        by returning 'false' value.
 * 
 *        Note : this function is blocking.
 */
  
  bool fEndOfCapture = false;
  char kbEnter;
  uint8_t count = 0;

  Serial.println("");
  Serial.println(sMessValue);
  this->giveCmdPrompt();
  sMessValue = "";

  while (fEndOfCapture == false)
  {
    if (Serial.available() > 0)
    {
      kbEnter = Serial.read();

      if (isdigit(kbEnter))  //is it digital character ? (0 1 2 3 4 5 6 7 8 9)
      {
        sMessValue.concat(kbEnter);
        Serial.write(kbEnter);
        count++;
      }
      else
      {
        switch (kbEnter)
        {
        case '+': //sign '+'
          if (count == 0)
          {
            sMessValue.concat(kbEnter);
            Serial.write(kbEnter);
            count++;
          }
          break;

        case '-':  //sign '-'
          if (count == 0)
          {
            sMessValue.concat(kbEnter);
            Serial.write(kbEnter);
            count++;
          }
          break;

        case '.': //decimal separator (warning it is not the same depending on the country or the language)
          if((count > 0) && (sMessValue.indexOf('.')<0))
          {
            sMessValue.concat(kbEnter);
            Serial.write(kbEnter);
            count++;
          }
          break;

        case 0x0A: //Enter key pressed
          fEndOfCapture = true;
          break;

        case 0x08: //Backspace key pressed
          if (count > 0)
          {
            Serial.write(kbEnter);
            count--;
            sMessValue.remove(sMessValue.length()-1);
          }
          break;

        case 0x1B: //Escape key pressed
          sMessValue = "";
          return false;
          break;
        }
      }
    }
  }
  return true;
}



bool SerialMenuCmd::getStrOfChar(String& sMessText)
{
/**
 * @brief this function retrieves the serial flux. if character
 *        is "printable", it stored.
 *        the memorized character string is returned when user 
 *        valid by Enter key, the function return 'true' value.
 *        Backspace key delete the last character.
 *        Escape key interrupts the sequence : the function ends 
 *        by returning 'false' value
 * 
 *        Note : this function is blocking.
 */
  
  bool fEndOfCapture = false;
  char kbEnter;
  uint8_t count = 0;

  Serial.println("");
  Serial.println(sMessText);
  this->giveCmdPrompt();
  sMessText = "";

  while (fEndOfCapture == false) 
  {
    if (Serial.available() > 0)
    {
      kbEnter = Serial.read();

      if(isprint(kbEnter))
      {
        if(count < NB_MAX_CHAR_IN_TXT)
        {
          sMessText.concat(kbEnter);
          Serial.write(kbEnter);
          count++;
        }
      }
      else
      {
        switch (kbEnter)
        {
        case 0x0A: //Enter key pressed
          fEndOfCapture = true;
          break;

        case 0x08: //Backspace key pressed
          if (count > 0)
          {
            Serial.write(kbEnter);
            count--;
            sMessText.remove(sMessText.length()-1);
          }
          break;

        case 0x1B: //Escape key pressed
          sMessText = "";
          return false;
          break;
        }
      }
    }
  }
  return true;
}