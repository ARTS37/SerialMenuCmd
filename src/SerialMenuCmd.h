/**
 * @file SerialMenuCmd.h
 *
 * @mainpage Serial Menu Command header file
 *
 * @section description Description
 * This file declares all elements and processes of "Serial Menu Command" library :
 * - macro,
 * - error codes,
 * - definition of types,
 * - function specification,
 * - class.
 *
 * In use, the operation is similar to the basic CLI mode (Command Line Interface).
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
 * V1.1.0, February 2022, added a function to get a date/time group from a string, Armand ROLLAND
 * V1.1.1, February 2022, adapt library to work with external terminals, Armand ROLLAND
 * V1.1.2, February 2022, Full compliance with terminals emulators and code restyling, Armand ROLLAND
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

// Include guard to avoid multiple compilation of the library
#ifndef SerialMenuCmd_h
#define SerialMenuCmd_h

/* Includes Files (libraries) =================================================== */
#include <Arduino.h> /// required for Visual Studio Code
#include <avr/pgmspace.h> /// for memory program access
#include <stdio.h> ///required for formatted output conversion to a string of characters
#include <time.h> ///required for date operating

/* macros =============================================================================== */
#define NB_MAX_CHAR_IN_TXT 64 /// maximum number of characters used in strings

enum MenuCmdReturnCodeError {
    MENUCMD_ERROR_OVERRANGE_INDEX = -1 /// error return code, see implementation of OpsCallback member function
};

/**
 * @typedef Type to declare the menu character strings
 *
 */
typedef const char PROGMEM tMenuCmdTxt;

/**
 * @typedef Intermediate type to specify function pointer
 *
 */
typedef void (*PtFunc)(void);

/**
 * @struct Structure which gathers the menus / commands in list
 *         A menu/command item incorporate :
 *         - a descriptive text for the user
 *         - a keycode touch keyboard (a printable character in the ascii sense of the term)
 *         - a pointer to a function to be executed when the user presses the correponding keycode
 */
typedef struct
{
    const char* itemTxt;
    uint8_t itemCode;
    PtFunc itemPtFunct;
} stMenuCmd;

/**
 * @struct structure that contains the individual elements of a date/time group :
 *         - u16Year, 2 bytes, to specify the year (0 to 65535)
 *         - u8Month, 1 byte, to specify the month (1 to 12)
 *         - u8Day, 1 byte, to specify the day (1 to 31)
 *         - u8Hour, 1 byte, to specify the hour (00 to 23)
 *         - u8Min, 1 byte, to specify the minute (00 to 59)
 *         - u8Sec, 1 Byte, to specify the second (00 to 59)
 *
 *         Outside lower and upper limit the date/time group is considered wrong
 */
typedef struct
{
    uint16_t u16Year;
    uint8_t u8Month;
    uint8_t u8Day;
    uint8_t u8Hour;
    uint8_t u8Min;
    uint8_t u8Sec;
    //} stDTGi;
} stDateTimeGroup;

/**
 * @class Serial Menu Command Library
 *
 */
class SerialMenuCmd {
private:
    stMenuCmd* mMenuCmds; // Pointer to the structure defined by the user
    uint8_t mNbCmd; // number of menu in the list
    const char* mPromptText; // Text to write before '>'

    /**
     * @fn Find a matching between command codes list and the parameter
     *
     * @param key pressed by user
     * @return uint8_t 0 -> no command, >0 order command in the list
     */
    uint8_t searchCode(char key);

public:
    /**
     * @fn Construct a new Serial Menu Cmd object
     *
     */
    SerialMenuCmd();

    /**
     * @fn Initialization
     *
     * @param list      structure stMenuCmd
     * @param NbCmd     the number of commands in list
     * @param acPrompt  text of prompt
     * @return true     success initialization
     * @return false    invalid initialization
     */
    bool begin(stMenuCmd list[], uint8_t NbCmd, const char* acPrompt);

    /**
     * @fn Execution of a command in the list
     *
     * @param numItem command number in the list
     * @return int8_t true = Command executed, <0 error code (see enum MenuCmdReturnCodeError)
     */
    int8_t ExeCommand(uint8_t numItem);

    /**
     * @fn Get the number of commands in the list
     *
     * @return uint8_t number of commands (0 = no list)
     */
    uint8_t getNbCmds(void);

    /**
     * @fn Display menu on serial monitor or terminal
     *
     */
    void ShowMenu(void);

    /**
     * @fn Scan serial Flow to identify commands codes
     *
     * @return uint8_t 0 = no code, >0 command number in the list
     */
    uint8_t UserRequest(void);

    /**
     * @fn getting a string object containing a numeric value
     *
     * @param sMessValue
     *        - Enter function = Message to user to guide him
     *        - output function = string containing numéric value
     * @return true   parameter containts a value
     * @return false  aborted operation
     */
    bool getStrValue(String& sMessValue);

    /**
     * @fn Getting a string object containing a string of character
     *        - Enter function = Message to user to guide him
     *        - output function = string containing string of character
     * @param sMessText
     *
     * @return true  parameter containts a string of character
     * @return false aborted operation
     */
    bool getStrOfChar(String& sMessText);

    /**
     * @fn Convert a string object to a date-time group structure.
     *        a date-time group (DTg) includes : Year, Month, day, Hour, minute, second.
     *        DTg is formated in ISO8601 in the object string.
     *        YYYY-MM-DDThh:mm:ss
     *        YYYY (4 char.) -> Year
     *        MM (2 char.) -> Month
     *        DD (2 char.) -> Day
     *        T -> letter T
     *        hh (2 char.) -> Hour
     *        mm (2 char.) -> minute
     *        ss (2 char.) -> second
     *
     * @param sDTgStrChar String object including a string of max 20 characters in ISO 8601 format
     * @param stRet struct stDateTimeGroup
     * @return true Successful conversion
     * @return false failed conversion (do not use struct stDateTimeGroup)
     */
    // bool ConvStrToDTg(char *DTgString, stDTGi &stRet);
    bool ConvStrToDTg(String& sDTgStrChar, stDateTimeGroup& stRet);

    /**
     * @fn give command prompt to user
     *
     */
    void giveCmdPrompt(void);
};

#endif