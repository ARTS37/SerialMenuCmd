# __*Serial Menu Command*__ Library

This Arduino library constitutes a basic Command Line Interface (CLI).

It easy to use and low consumption RAM memory.

</br>
</br>

## Context

It is not always possible, I would even say on the contrary, it is rare to achieve a directly functional system, entirely autonomous after the upload. As far as development is concerned, this phase requires debugging operations which are carried out at the request of the user (such as for example stopping a process to find out the value of a parameter). In operation, the system may require an interaction with the user in order to modify the operation (for example acquisition and then restitution of data) or else configure the system according to environmental conditions (calibration, taring, positioning mechanism, etc.).

From the point of view of the arduino board, it is a question of scanning the data arriving on the serial port and when a code is identified, it is necessary to execute the corresponding command. Obviously, to remain easily exploitable by the user, the interaction must be explicit or in other words informational and contextual (text, more text!).

Personally, before embarking on this development, like Jack Sparrow traveling the oceans in search of the rare pearl, I scoured the internet to find the precious library. There are many solutions but they did not meet my requirements: either it consumes too much RAM memory (very quickly the application flirts with the maximum memory limit) or it is (in my opinion) too complicated to put implemented.
___
</br>

## Synopsis

So, the SerialMenuCmd library how it works. it's simple, just :

* include header file -> <SerialMenuCmd.h>
* instantiate the library class
* Fill a struct with the following items :
  * Text discribing the command
  * Command activation code
  * Function to be execute
* Initialize library in arduino fonction *setup()*
* In the arduino function *loop()* regularly call the function which scans the serial data and if a code is identified then the corresponding command must be executed.

___
</br>

## Template

```C++
/* Includes Files (libraries) ============================================ */
#include <Arduino.h>       //Library only needed with Visual Studio Code
#include <SerialMenuCmd.h> //Library referred to in this example

...

/* Instanciation ========================================================= */
SerialMenuCmd myMmuCmd;

...

/* Structure list of command ============================================= */

//Declaration texts of menu
tMenuCmdTxt txt1_DoIt1[] = "1 - Just Do It 1";
tMenuCmdTxt txt2_DoIt2[] = "2 - Just Do It 2";
....
tMenuCmdTxt txt3_DoIta[] = "a - Just Do It a";
tMenuCmdTxt txt4_DoItb[] = "b - Just Do It b";
...
tMenuCmdTxt txt5_DisplayMenu[] = "? - ";

//Declaration text of prompt
tMenuCmdTxt txt_Prompt[] = "User";

//Prototype of function which are callback by the library
void cmd1_DoIt1(void);
void cmd2_DoIt1(void);
...
void cmd1_DoIta(void);
void cmd2_DoItb(void);
...

//Initialization of structure
//type of data
//array sMenuTxt, code character , function   (Reminder the code character must be printable character)
stMenuCmd list[] = {
    {txt1_DoIt1, '1', cmd1_DoIt1},
    {txt2_DoIt2, '2', cmd2_DoIt2},
    ...
    {txt3_DoIta, 'a', cmd3_DoIta},
    {txt4_DoItb, 'b', cmd4_DoItb},
    ...
    {txt5_DisplayMenu, '?', []() { myMmuCmd.ShowMenu();
                                   myMmuCmd.giveCmdPrompt();}}};


//KmenuCount contains the number of command
#define NbCmds sizeof(list) / sizeof(stMenuCmd)


/* Implementation of functions ========================================= */


/**
 * @brief Standard function Arduino for initialisation
 * 
 */
void setup()
{
  //Don't forget, CLI based on serial communication link
  Serial.begin(115200);


  if (myMmuCmd.begin(list, NbCmds, txt_Prompt) == false)
  {
    while (true)
    {
      //Error
    }
  }

  //Display Menu and Command Prompt
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

  CmdCode = myMmuCmd.UserRequest();

  ...
  
  if (CmdCode != 0)
  {
    myMmuCmd.ExeCommand(CmdCode);
  }

  ...
}

void cmd1_DoIt1(void)
{...}

void cmd1_DoIt2(void)
{...}
...
void cmd1_DoIta(void)
{...}

void cmd1_DoItb(void)
{...}
...
```

</br>

>Note
>
> the user chooses the names of texts and functions. The proposed example just facilitates the implementation
>
> texts must be type *tMenuCmdTxt*
>
> texts and functions that are grouped together in the structure must be declared beforehand
>
> Instead of a function pointer it is possible to implement a lambda function [ ] ( ) {  }
>
> if the function *[Name Instance].begin(...)* fails, the program must be interrupted -> *while(true) {...}*
>
> regularly call *[Name Instance].UserRequest()*, if a command is detected (return> 0), then execute the corresponding command *[Name Instance].ExeCommand(Num. Comd.)*
___
</br>

## Specifications

function offered by the library

| Function | Description                                                               |
| ------ | ------------------------------------------------------------------------- |
| SerialMenuCmd   | Constructor (Class)|
| begin | Initialisation, arguments : 1st struct stMenuCmd, 2nd Number of commands, 3rd prompt text|
| ExeCommand    | Execute a command, argument is number of command, return true if ok else error code (MENUCMD_ERROR_OVERRANGE_INDEX = -1)|
|getNbCmds|Return number of commands|
|ShowMenu| Display menu (list of texts command)|
|UserRequest|Scan serial Flow to identify commands codes, return 0 if no command identified eless number command.|
|getStrValue|If return true then the String object, passed in argument, contains a digital value in text format (it needed to convert with atoi or atof)|
|getStrofChar|If return true then the String object, passed in argument, contains a string of character|
|ConvStrToDTg|If return true then the struct stDateTimeGroup, passed in argument, contains date and date define by user|

___
</br>

## Version

|Version|Changelog|Contributor|
|------ | --------|-----------|
|V1.0.0|Initial release|Armand Rolland Europe, France, Bretagne</br> Armand.rolland71@gmail.com|
|V1.1.0|Added a function to convert a character string into elements that compose a date and a time|Armand Rolland Europe, France, Bretagne</br> Armand.rolland71@gmail.com|
|V1.1.1|Adapt library to work with external terminals|Armand Rolland Europe, France, Bretagne</br> Armand.rolland71@gmail.com|
|V1.1.2|Full compliance with terminals emulators and code restyling|Armand Rolland Europe, France, Bretagne</br> Armand.rolland71@gmail.com|

___
</br>

## Installation - importing a zip library

First, download the library as a ZIP, which is done by clicking the green ''Clone or download'' button and then clicking ''Download ZIP''.

Once downloaded, go to the Arduino IDE, click <ins>__*Sketch > Include Library > Add .zip Library*__</ins> and select zip file.

Library is distributed as a ZIP file. The name of the folder is the name of library. Inside the folder will be :

* a subfolder "*Examples*" which contains 3 examples
* a subfolder "*src*" which contains sources files (SerialMenuCmd.h, SerialMenuCmdLand.h and SerialMenuCmd.cpp)
* Keywords.txt
* licence.txt
* library.properties
* readme.md

</br>

For information, libraries are inside sketchbooks folder. By default, sketchbooks folder name is "*Arduino*" and is located in Documents folder of user. As shown in the following tree structure, libraries is subfolder of Arduino folder.
</br>

```bash
Root c:\ (by default)
├── ...
├── Users
│   ├── ...
│   └── [User name]
        ├── Documents
            ├── ...
            └── Arduino
                ├── ...
                └── Libraries
                    ├── ...
                    └── SerialMenuCmd
                        ├── examples
                        │   ├── exemple1
                        │   │   └── example1.ino
                        │   ├── exemple2
                        │   │   └── example2.ino
                        │   └── exemple3
                        │       └── example3.ino (require RTC board DS1302)
                        ├── src
                        │   ├── SerialMenuCmd.h
                        │   ├── SerialMenuCmdLang.h
                        │   └── SerialMenuCmd.cpp
                        ├── Keywords.txt
                        ├── library.properties
                        ├── Licence.txt
                        └── readme.md                   
```

>Note
>
>if the location of the sketchbooks has been modified during the installation of the software, it can be found by consulting edit box *Sketchbook location* in dialog box <ins>__*File > Preferences*__</ins>.

___
</br>

## Internationalization

Messages sent to the user are by default in English. In the current version, messages can also be sent in French. To do this, modify the following preprocessor macro in the SerialMenuCmdLang.h file :

```C++
#define ENGLISH
```

by

```C++

#define FRENCH
```

See the paragraph "__*Installation - importing a zip library*__" to locate *SerialMenuCmdLang.h* file.
___
</br>

## Compliance with terminals emulators

Serial monitor is an essential tool when developing an embedded system project. For this reason, it is directly incorporated into IDEs, or in other words, Integrated Development Environment. However, the operation of the constituted device (after development) may also require serial communication with user. In this situation, the IDE's serial monitor is not the most appropriate tool. To do this, there is a multitude of serial communication software called terminal emulators, many of which are open source. Compliance with terminal emulators aims to facilitate their operation by the user without the need for special skills. apart from the baudrate, normally no other specification is to be configured. The configuration is standard : 8N1 (Data, Stop and Parity bits) and all 'Carriage Return' and 'Line Feed' modes are accepted (CR, LF or CRLF).
___
</br>

## Todo

For the youngest but also for the beginners who speak neither English nor French it can be interesting to translate the information messages in other languages... Thus, it is easier to interact with the device.
</br>
Contributors are Welcome !

___
</br>

## Licence & Copyright

MIT License

Copyright (c) 2022 Armand ROLLAND

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
