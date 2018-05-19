/*******************************************************************************
* Name            : settings.h
* Project         : DungeonMap
* Module          : App
* Description     : Stores all defines used to control settings
* Copyright       : Tanner Voas.
* Creation Date   : 5/18/2018
* Original Author : Tanner Voas
*
******************************************************************************/
#ifndef SRC_SETTINGS_H_
#define SRC_SETTINGS_H_

// Math Constants
#define E_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062f

// File Loading Settings
#define MAX_FILE_LINE_LENGTH 1024

// World Manager Settings
#define MAX_OBJECT_COUNT 1000

// Debuging Options
// --- MSGLVL Options
#define MSGLVL_ 0
#define MSGLVL(L, N) (if (MSGLVL_ >= L) {N})
#define _LOG(N) (printf(N))
#define MSG1(N) (MSGLVL(1,_LOG(N)))  // Runs if MSGLVL_ >= 1 | Low Messages
#define MSG2(N) (MSGLVL(2,_LOG(N)))  // Runs if MSGLVL_ >= 2 | Medium Messages
#define MSG3(N) (MSGLVL(3,_LOG(N)))  // Runs if MSGLVL_ >= 3 | High Messages
// --- MAINDRIVER Options
#define DEBUG_MAINDRIVER 1

#endif  // SRC_SETTINGS_H_
