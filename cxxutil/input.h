#pragma once

#include <stdint.h>
#include <keypadc.h>
#include <cxxutil/mem.h>

/* Keyboard group 1 */
#define CXX_KEY_Graph    0
#define CXX_KEY_Trace    1
#define CXX_KEY_Zoom     2
#define CXX_KEY_Window   3
#define CXX_KEY_Yequ     4
#define CXX_KEY_2nd      5
#define CXX_KEY_Mode     6
#define CXX_KEY_Del      7

/* Keyboard group 1 - 83 Premium CE key names */
#define CXX_KEY_Fenetre  CXX_KEY_Window
#define CXX_KEY_Fx       CXX_KEY_Yequ
#define CXX_KEY_Suppr    CXX_KEY_Del

/* Keyboard group 2 */
#define CXX_KEY_Sto     8 
#define CXX_KEY_Ln      9 
#define CXX_KEY_Log     10 
#define CXX_KEY_Square  11 
#define CXX_KEY_Recip   12 
#define CXX_KEY_Math    13 
#define CXX_KEY_Alpha   14 

/* Keyboard group 2 - 83 Premium CE key names */
#define CXX_KEY_TglExact CXX_KEY_Recip

/* Keyboard group 3 */
#define CXX_KEY_0        15 
#define CXX_KEY_1        16 
#define CXX_KEY_4        17 
#define CXX_KEY_7        18 
#define CXX_KEY_Comma    19 
#define CXX_KEY_Sin      20 
#define CXX_KEY_Apps     21 
#define CXX_KEY_GraphVar 22 

/* Keyboard group 3 - 83 Premium CE key names */
#define CXX_KEY_Trig     CXX_KEY_Sin
#define CXX_KEY_Matrice  CXX_KEY_Apps

/* Keyboard group 4 */
#define CXX_KEY_DecPnt  23 
#define CXX_KEY_2       24 
#define CXX_KEY_5       25 
#define CXX_KEY_8       26 
#define CXX_KEY_LParen  27 
#define CXX_KEY_Cos     28 
#define CXX_KEY_Prgm    29 
#define CXX_KEY_Stat    30 

/* Keyboard group 4 - 83 Premium CE key names */
#define CXX_KEY_Resol    CXX_KEY_Cos

/* Keyboard group 5 */
#define CXX_KEY_Chs     31 
#define CXX_KEY_3       32 
#define CXX_KEY_6       33 
#define CXX_KEY_9       34 
#define CXX_KEY_RParen  35 
#define CXX_KEY_Tan     36 
#define CXX_KEY_Vars    37 

/* Keyboard group 5 - 83 Premium CE key names */
#define CXX_KEY_Frac     CXX_KEY_Tan

/* Keyboard group 6 */
#define CXX_KEY_Enter   38 
#define CXX_KEY_Add     39 
#define CXX_KEY_Sub     40 
#define CXX_KEY_Mul     41 
#define CXX_KEY_Div     42 
#define CXX_KEY_Power   43 
#define CXX_KEY_Clear   44 

/* Keyboard group 5 - 83 Premium CE key names */
#define CXX_KEY_Annul    CXX_KEY_Clear

/* Keyboard group 7 */
#define CXX_KEY_Down    45 
#define CXX_KEY_Left    46 
#define CXX_KEY_Right   47 
#define CXX_KEY_Up      48 

#define CXX_KEY_NumKeys CXX_KEY_Up + 1

namespace cxxutil {
    typedef uint8_t cxx_key_t;

    class KeyManager : SafeObject {
    private:
        static KeyManager *singleton;
        KeyManager();
    
    public:
        static KeyManager *getInstance();

        // Need some sort of key focusing here...
    };
}


