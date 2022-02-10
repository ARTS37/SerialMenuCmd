/**
 * @file SerialMenuCmdLang.h
 *
 * @mainpage Serial Menu Command header file, languages
 *
 * @section description Description
 * This file contains dialog messages for user. 
 * 
 * @section circuit Circuit
 * This library is not attached to a device.
 * 
 * @section libraries Libraries
 * None
 * 
 * @section notes Notes
 * None at this time
 * 
 * @section todo TODO
 * Translate and integrate in other languages
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

/**
 * @brief Language
 *        language default is ENGLISH. To change messages language, set the
 *        name of the desired language below, example :
 *        #define [ENGLISH or FRENCH]
 *        otherwise, create 

 */

//Define Language, option -> replace by FRENCH 
#define ENGLISH  

//check if a language is selected
#if !(defined(ENGLISH) || defined(FRENCH))
    //if no language defined then default language is ENGLISH
    #define ENGLISH
#endif

#if defined(ENGLISH)
                            //                   0        1         2         3         4         5         6    
                            //Size string char  "123456789012345678901234567890123456789012345678901234567890123"
//function member "begin"
#define  TXT_fctMemberBegin_InvalidArguments    "structure, number of commands or text of prompt are invalid"
#define  TXT_fctMemberBegin_OversizeCmdTxt      "Maximum number of characters is reached in a command text"
#define  TXT_fctMemberBegin_OversizePrompt      "Maximum number of characters is reached in Command prompt"

//function member "ShowMenu"
#define  TXT_fctMemberShowMenu_NoMenu           "No Menu in memory"
#define  TXT_fctMemberShowMenu_PBInterf         "Interface mistake with library SerialSerialMenuCmd"

//function member "UserRequest"
#define  TXT_fctMemberUserRequest_TooArgs       " -> too many arguments ! (one character only)"
#define  TXT_fctMemberUserRequest_NoMatching    " -> No command code matching !"
#define  TXT_fctMemberUserRequest_Cancellation  " -> cancellation !"

#elif defined(FRENCH)
                            //                   0        1         2         3         4         5         6    
                            //Size string char  "123456789012345678901234567890123456789012345678901234567890123"
//function member "begin"
                                                 
#define  TXT_fctMemberBegin_InvalidArguments    "La structure, le nombre de commande ou l'invite est invalide" 
#define  TXT_fctMemberBegin_OversizeCmdTxt      "Un text de commande dépasse le maximum de caractère possible"
#define  TXT_fctMemberBegin_OversizePrompt      "le mamximum de caractère dans l'invite de commande est dépassé"

//function member "ShowMenu"
#define  TXT_fctMemberShowMenu_NoMenu           "Aucun menu disponible"
#define  TXT_fctMemberShowMenu_PBInterf         "Problème d'interface avec la bibliothèque SerialMenuCmd"

//function member "UserRequest"
#define  TXT_fctMemberUserRequest_TooArgs       "-> trop d'arguments transmis (1 seul possible) !"
#define  TXT_fctMemberUserRequest_NoMatching    " -> aucune commande ne correspond au code transmis !"
#define  TXT_fctMemberUserRequest_Cancellation  " -> Annulation !"

#endif