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
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Includes Files (libraries) ================================================================== */
#include <Arduino.h>
#include <SerialMenuCmd.h>
#include <SerialMenuCmdLang.h>

/* Macros ====================================================================================== */
#define ascii_CR 0x0D // Carrier Return
#define ascii_LF 0x0A // Line Feed
#define ascii_DLE 0x10 // Data link Escape
#define ascii_ESC 0x1B // Escape
#define ascii_BS 0x08 // Backspace


/* Local function prototype ==================================================================== */
// implementation at end of file
bool DTgChekField(char* strStop, char* strStart, uint8_t nbChar);


/* Implementation member functions ============================================================= */
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


bool SerialMenuCmd::begin(stMenuCmd list[], uint8_t NbCmd, const char* acPrompt)
{
    /**
     * @brief  Before assigning the data, they are verified. In case of error the
     *         function displays a message and returns false
     *
     */
    uint8_t i;

    if ((NbCmd == 0) || (list == nullptr) || (acPrompt == nullptr)) {
        // Send to user : "structure, number of commands or text of prompt are
        // invalid"
        Serial.println(F(TXT_fctMemberBegin_InvalidArguments));
        return false;
    }

    for (i = 0; i < NbCmd; i++) {
        if (strlen_P(list[i].itemTxt) > NB_MAX_CHAR_IN_TXT) {
            // Send to user : "one or more menu text exceeds the limit of the number
            // of characters allowed"
            Serial.println(F(TXT_fctMemberBegin_OversizeCmdTxt));
            return false;
        }
    }

    if (strlen_P(acPrompt) > NB_MAX_CHAR_IN_TXT) {
        // Send to user : "text of prompt exceeds the limit of number of characters
        // allowed"
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
     * @brief Execute function pointed by the structure. Before, checks if index
     * is valid.
     *
     *        Note : this function isn't blocking, but the function callback can
     * be.
     *
     */
    if ((numItem > mNbCmd) || (numItem == 0)) {
        return MENUCMD_ERROR_OVERRANGE_INDEX;
    }

    // Warning, For the user, the menu list ranges from 1 to n and for the system
    // it is from 0 to n-1
    mMenuCmds[numItem - 1].itemPtFunct();
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

    for (i = 0; i < mNbCmd; i++) {
        if (car == mMenuCmds[i].itemCode) {
            fDetect = true;
            break;
        }
    }

    if (fDetect == true) {
        return i + 1;
    } else {
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
    if (mNbCmd == 0) {
        // Send to user : "No Menu in memory"
        Serial.println(F(TXT_fctMemberShowMenu_NoMenu));

        // Send to user : "PB to interface with library SerialSerialMenuCmd")
        Serial.println(F(TXT_fctMemberShowMenu_PBInterf));
        return;
    }

    for (i = 0; i < mNbCmd; i++) {
        /**
         * @brief strlcpy_P copy a maximum of NbMaxChar - 1 (last character is NULL
         * terminate) According to the IDE, the strlcpy_P function may need special
         * attention because the overriding function is only defined at compile
         * time.
         */
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
    /**
     * @brief strlcpy_P copy a maximum of NbMaxChar - 1 (last character is NULL
     * terminate) According to the IDE, the strlcpy_P function may need special
     * attention because the overriding function is only defined at compile time.
     */
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

    CodeCmd = 0;
    if (Serial.available() > 0) {
        kbEnter = Serial.read();
        // is printer character ?
        if (isGraph(kbEnter)) {
            Serial.write(kbEnter);
            // if there is no memorized code yet
            if (ValideCode == 0) {
                ValideCode = kbEnter;
            } else {
                /*To limit RAM memory consumption,  only one character is stored.
                if a second character is sent in this case it is considered
                that there is an overflow*/
                ValideCode = ascii_DLE;
            }
        } else {
            switch (kbEnter) {
            case ascii_LF: // Enter key pressed -> ASCII code [LF]
            case ascii_CR: // Enter key pressed -> ASCII code [CR]
                switch (ValideCode) {
                case 0:
                    this->giveCmdPrompt();
                    break;

                case ascii_DLE:
                    // Send to user : "-> too many arguments (one character only), !"
                    Serial.println(F(TXT_fctMemberUserRequest_TooArgs));
                    ValideCode = 0;
                    this->giveCmdPrompt();
                    break;

                default:
                    CodeCmd = this->searchCode(ValideCode);
                    if (CodeCmd == 0) {
                        // Send to user : " -> No command code matching !"
                        Serial.println(F(TXT_fctMemberUserRequest_NoMatching));
                        Serial.println(F(""));
                        this->giveCmdPrompt();
                    }
                    ValideCode = 0;
                    break;
                }
                ////flush serial buffer, in case of transmission [CR] + [LF]
                // wait transmit caracter delay
                delay(2);
                if (Serial.available() > 0) {
                    kbEnter = Serial.peek();
                    if ((kbEnter == ascii_LF) || (kbEnter == ascii_CR)) {
                        kbEnter = Serial.read();
                    }
                }
                break;

            case ascii_BS: // Backspace pressed
                if (ValideCode != 0) {
                    // delete if only one character memorized (validecode != ascii_DLE)
                    // otherwise abort
                    if (ValideCode != ascii_DLE) {
                        Serial.write(kbEnter); // send back backspace character
                    } else {
                        // Send to user : "-> too many arguments (one character only), !"
                        Serial.print(F(TXT_fctMemberUserRequest_TooArgs));
                        // Send to user : " -> cancellation !"
                        Serial.println(F(TXT_fctMemberUserRequest_Cancellation));
                        this->giveCmdPrompt();
                    }
                    ValideCode = 0;
                }
                break;

            case ascii_ESC: // Escape pressed
                // Send to user : " -> cancellation !"
                Serial.println(F(TXT_fctMemberUserRequest_Cancellation));
                this->giveCmdPrompt();
                ValideCode = 0;
                break;

            default:
                // no other non-printing characters supported at this stage of
                // development
                ValideCode = 0;
                break;
            }
        }
    }
    return CodeCmd;
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

    while (fEndOfCapture == false) {
        if (Serial.available() > 0) {
            kbEnter = Serial.read();

            if (isdigit(kbEnter)) // is it digital character ? (0 1 2 3 4 5 6 7 8 9)
            {
                sMessValue.concat(kbEnter);
                Serial.write(kbEnter);
                count++;
            } else {
                switch (kbEnter) {
                case '+': // sign '+'
                    if (count == 0) {
                        sMessValue.concat(kbEnter);
                        Serial.write(kbEnter);
                        count++;
                    }
                    break;

                case '-': // sign '-'
                    if (count == 0) {
                        sMessValue.concat(kbEnter);
                        Serial.write(kbEnter);
                        count++;
                    }
                    break;

                case '.': // decimal separator (warning it is not the same depending on
                          // the country or the language)
                    if ((count > 0) && (sMessValue.indexOf('.') < 0)) {
                        sMessValue.concat(kbEnter);
                        Serial.write(kbEnter);
                        count++;
                    }
                    break;

                case ascii_CR:
                case ascii_LF: // Enter key pressed
                    fEndOfCapture = true;
                    break;

                case ascii_BS: // Backspace key pressed
                    if (count > 0) {
                        Serial.write(kbEnter);
                        count--;
                        sMessValue.remove(sMessValue.length() - 1);
                    }
                    break;

                case ascii_ESC: // Escape key pressed
                    sMessValue = "";
                    return false;
                    break;
                }
            }
        }
    }
    ////flush serial buffer, in case of transmission [CR] + [LF]
    // wait transmit caracter delay
    delay(2);
    if (Serial.available() > 0) {
        kbEnter = Serial.peek();
        if ((kbEnter == ascii_LF) || (kbEnter == ascii_CR)) {
            kbEnter = Serial.read();
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

    while (fEndOfCapture == false) {
        if (Serial.available() > 0) {
            kbEnter = Serial.read();

            if (isprint(kbEnter)) {
                if (count < NB_MAX_CHAR_IN_TXT) {
                    sMessText.concat(kbEnter);
                    Serial.write(kbEnter);
                    count++;
                }
            } else {
                switch (kbEnter) {
                case ascii_CR:
                case ascii_LF: // Enter key pressed
                    fEndOfCapture = true;
                    break;

                case ascii_BS: // Backspace key pressed
                    if (count > 0) {
                        Serial.write(kbEnter);
                        count--;
                        sMessText.remove(sMessText.length() - 1);
                    }
                    break;

                case ascii_ESC: // Escape key pressed
                    sMessText = "";
                    return false;
                    break;
                }
            }
        }
    }
    ////flush serial buffer, in case of transmission [CR] + [LF]
    // wait transmit caracter delay
    delay(2);
    if (Serial.available() > 0) {
        kbEnter = Serial.peek();
        if ((kbEnter == ascii_LF) || (kbEnter == ascii_CR)) {
            kbEnter = Serial.read();
        }
    }

    return true;
}


bool SerialMenuCmd::ConvStrToDTg(String& sDTgStrChar, stDateTimeGroup& stRet)
{
    /**
     * @brief This function Convert a string object to a date-time group structure
     *        It proceeds as follows :
     *        1- Check string length
     *        2- locate delimiters (-, T, :)
     *        3- check their position
     *        4- convert strings to integer
     *        5- check date and hour (bounds, bi-weekly year)
     *        6- transfer data to the return address
     *
     *        In the event of an error, the function returns false. In this case,
     *        the return structure should not be used.
     */
    char buf[20];
    char *delimHyphen1, *delimHyphen2, *delimT, *delim2pt1, *delim2pt2, *delimEOS;
    stDateTimeGroup GdtLocal;
    bool bLeap;

    // Check string length -> AAAA-MM-DDThh:mm:ss (19 characters max.)
    if (sDTgStrChar.length() > 19) {
        // Send to user : "-> too many characters in string date/time !"
        Serial.print(F(TXT_fctMemberConvStrToDTg_TooChar));
        return false;
    }

    // Convert string in char array
    sDTgStrChar.toCharArray(buf, sDTgStrChar.length() + 1);

    // Search and locate all delimiter
    delimHyphen1 = strchr(buf, '-');
    delimHyphen2 = strchr(delimHyphen1 + 1, '-');
    delimT = strchr(delimHyphen2 + 1, 'T');
    delim2pt1 = strchr(delimT + 1, ':');
    delim2pt2 = strchr(delim2pt1 + 1, ':');
    delimEOS = buf + strlen(buf) + 1;

    if ((delimHyphen1 == NULL) || (delimHyphen2 == NULL)) {
        // Send to user : " -> Error of delimiter '-' in date"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrHyphen));
        return false;
    }

    if (delimT == NULL) {
        // Send to user : " -> Error of delimiter 'T' Between date and time"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrT));
        return false;
    }

    if ((delim2pt1 == NULL) || (delim2pt2 == 0)) {
        // Send to user : " -> Error of delimiter ':' in time"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrColon));
        return false;
    }

    *delimHyphen1 = 0;
    if (DTgChekField(delimHyphen1, buf, 4) == false) {
        // Send to user : " -> Year field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrYearFd));
        return false;
    }
    delimHyphen1++;

    *delimHyphen2 = 0;
    if (DTgChekField(delimHyphen2, delimHyphen1, 2) == false) {
        // Send to user : " -> Month field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrMonthFd));
        return false;
    }
    delimHyphen2++;

    *delimT = 0;
    if (DTgChekField(delimT, delimHyphen2, 2) == false) {
        // Send to user : " -> Day field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrDayFd));
        return false;
    }
    delimT++;

    *delim2pt1 = 0;
    if (DTgChekField(delim2pt1, delimT, 2) == false) {
        // Send to user : " -> Hour field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrHourFd));
        return false;
    }
    delim2pt1++;

    *delim2pt2 = 0;
    if (DTgChekField(delim2pt2, delim2pt1, 2) == false) {
        // Send to user : " -> Minute field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrMinuteFd));
        return false;
    }
    delim2pt2++;

    // delimEOS already to 0 (end of string char)
    if (DTgChekField(delimEOS, delim2pt1, 2) == false) {
        // Send to user : " -> Second field format error"
        Serial.print(F(TXT_fctMemberConvStrToDTg_ErrSecondFd));
        return false;
    }

    // Convert string to number
    GdtLocal.u16Year = atoi(buf);
    GdtLocal.u8Month = atoi(delimHyphen1);
    GdtLocal.u8Day = atoi(delimHyphen2);
    GdtLocal.u8Hour = atoi(delimT);
    GdtLocal.u8Min = atoi(delim2pt1);
    GdtLocal.u8Sec = atoi(delim2pt2);

    // Determine if leap year (bLeap = 1 if year is leap)
    bLeap = ((GdtLocal.u16Year % 4 == 0) && (GdtLocal.u16Year % 100 != 0)) || (GdtLocal.u16Year % 400 == 0);

    if ((GdtLocal.u16Year < 1970) || (GdtLocal.u16Year > 9999)) {
        // Send to user : " -> Year out of range, it must be between 1970 and 9999"
        Serial.print(F(TXT_fctMemberConvStrToDTg_YearOutRange));
        return false;
    }

    if ((GdtLocal.u8Month < 1) || (GdtLocal.u8Month > 12)) {
        // Send to user : " -> Month out of range, it must be between 1 and 12"
        Serial.print(F(TXT_fctMemberConvStrToDTg_MonthOutRange));
        return false;
    }

    if ((GdtLocal.u8Day < 1) || (GdtLocal.u8Day > 31)) {
        // Send to user : " -> Day out of range, it must be between 1 and 31"
        Serial.print(F(TXT_fctMemberConvStrToDTg_DayOutRange));
        return false;
    }

    if (GdtLocal.u8Month == 4 || GdtLocal.u8Month == 6 || GdtLocal.u8Month == 9 || GdtLocal.u8Month == 11) {
        if (GdtLocal.u8Day > 30) {
            // Send to user : " -> this month only has 30 days"
            Serial.print(F(TXT_fctMemberConvStrToDTg_Month30day));
            return false;
        }
    }

    if (GdtLocal.u8Month == 2) {
        if (bLeap == true) {
            if (GdtLocal.u8Day > 29) {
                // Send to user : " -> February only has 29 days in leap year"
                Serial.print(F(TXT_fctMemberConvStrToDTg_febLeap));
                return false;
            }
        } else {
            if (GdtLocal.u8Day > 28) {
                // Send to user : " -> February only has 28 days out of leap year"
                Serial.print(F(TXT_fctMemberConvStrToDTg_febOutLeap));
                return false;
            }
        }
    }

    if ((GdtLocal.u8Hour < 0) || (GdtLocal.u8Hour > 23)) {
        // Send to user : " -> Hour out of range, it must be between 0 and 23"
        Serial.print(F(TXT_fctMemberConvStrToDTg_HourOutRange));
        return false;
    }

    if ((GdtLocal.u8Min < 0) || (GdtLocal.u8Min > 59)) {
        // Send to user : " -> Minute out of range, it must be between 0 and 59"
        Serial.print(F(TXT_fctMemberConvStrToDTg_MinuteOutRange));
        return false;
    }

    if ((GdtLocal.u8Sec < 0) || (GdtLocal.u8Sec > 59)) {
        // Send to user : " -> Second out of range, it must be between 0 and 59"
        Serial.print(F(TXT_fctMemberConvStrToDTg_SecondOutRange));
        return false;
    }

    // Copy in struct passed by address
    memcpy(&stRet, (void*)&GdtLocal, sizeof(stDateTimeGroup));

    return true;
}


/**
 * @fn verification of fields (an array of char) representing a number :
 *     - field not empty
 *     - numbers of characters
 *     - characters only numbers
 *     This function is call only by ConvStrToDTg menber function
 *
 * @param strStop   end of array char address
 * @param strStart  start of array char address
 * @param nbChar    maximum number of characters contained in the string
 * @return true     Field valid
 * @return false    Field error
 */
bool DTgChekField(char* strStop, char* strStart, uint8_t nbChar)
{
    uint8_t i, lenField;

    lenField = strlen(strStart);

    // if none char
    if (lenField == 0) {
        return false;
    }

    // if the number of characters does not match
    if (lenField != nbChar) {
        return false;
    }

    // if each char is not a number
    for (i = 0; i < strlen(strStart); i++) {
        if (isDigit(strStart[i]) == false) {
            return false;
        }
    }

    if (strStart + lenField > strStop) {
        return false;
    }

    return true;
}
