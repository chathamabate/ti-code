#pragma once

#include <stdint.h>
#include <keypadc.h>
#include <cxxutil/core/mem.h>

namespace cxxutil {
namespace core {

    typedef uint8_t cxx_key_t;
    
    /* Keyboard group 1 */
    constexpr cxx_key_t CXX_KEY_Graph    = 0;
    constexpr cxx_key_t CXX_KEY_Trace    = 1;
    constexpr cxx_key_t CXX_KEY_Zoom     = 2;
    constexpr cxx_key_t CXX_KEY_Window   = 3;
    constexpr cxx_key_t CXX_KEY_Yequ     = 4;
    constexpr cxx_key_t CXX_KEY_2nd      = 5;
    constexpr cxx_key_t CXX_KEY_Mode     = 6;
    constexpr cxx_key_t CXX_KEY_Del      = 7;

    /* Keyboard group 1 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_Fenetre  = CXX_KEY_Window;
    constexpr cxx_key_t CXX_KEY_Fx       = CXX_KEY_Yequ;
    constexpr cxx_key_t CXX_KEY_Suppr    = CXX_KEY_Del;

    /* Keyboard group 2 */
    constexpr cxx_key_t CXX_KEY_Sto     = 8; 
    constexpr cxx_key_t CXX_KEY_Ln      = 9; 
    constexpr cxx_key_t CXX_KEY_Log     = 10; 
    constexpr cxx_key_t CXX_KEY_Square  = 11; 
    constexpr cxx_key_t CXX_KEY_Recip   = 12; 
    constexpr cxx_key_t CXX_KEY_Math    = 13; 
    constexpr cxx_key_t CXX_KEY_Alpha   = 14; 

    /* Keyboard group 2 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_TglExact = CXX_KEY_Recip;

    /* Keyboard group 3 */
    constexpr cxx_key_t CXX_KEY_0        = 15; 
    constexpr cxx_key_t CXX_KEY_1        = 16; 
    constexpr cxx_key_t CXX_KEY_4        = 17; 
    constexpr cxx_key_t CXX_KEY_7        = 18; 
    constexpr cxx_key_t CXX_KEY_Comma    = 19; 
    constexpr cxx_key_t CXX_KEY_Sin      = 20; 
    constexpr cxx_key_t CXX_KEY_Apps     = 21; 
    constexpr cxx_key_t CXX_KEY_GraphVar = 22;

    /* Keyboard group 3 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_Trig     = CXX_KEY_Sin;
    constexpr cxx_key_t CXX_KEY_Matrice  = CXX_KEY_Apps;

    /* Keyboard group 4 */
    constexpr cxx_key_t CXX_KEY_DecPnt  = 23;
    constexpr cxx_key_t CXX_KEY_2       = 24;
    constexpr cxx_key_t CXX_KEY_5       = 25;
    constexpr cxx_key_t CXX_KEY_8       = 26;
    constexpr cxx_key_t CXX_KEY_LParen  = 27;
    constexpr cxx_key_t CXX_KEY_Cos     = 28;
    constexpr cxx_key_t CXX_KEY_Prgm    = 29;
    constexpr cxx_key_t CXX_KEY_Stat    = 30;

    /* Keyboard group 4 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_Resol    = CXX_KEY_Cos;

    /* Keyboard group 5 */
    constexpr cxx_key_t CXX_KEY_Chs     = 31;
    constexpr cxx_key_t CXX_KEY_3       = 32;
    constexpr cxx_key_t CXX_KEY_6       = 33;
    constexpr cxx_key_t CXX_KEY_9       = 34;
    constexpr cxx_key_t CXX_KEY_RParen  = 35;
    constexpr cxx_key_t CXX_KEY_Tan     = 36;
    constexpr cxx_key_t CXX_KEY_Vars    = 37;

    /* Keyboard group 5 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_Frac     = CXX_KEY_Tan;

    /* Keyboard group 6 */
    constexpr cxx_key_t CXX_KEY_Enter   = 38;
    constexpr cxx_key_t CXX_KEY_Add     = 39;
    constexpr cxx_key_t CXX_KEY_Sub     = 40;
    constexpr cxx_key_t CXX_KEY_Mul     = 41;
    constexpr cxx_key_t CXX_KEY_Div     = 42;
    constexpr cxx_key_t CXX_KEY_Power   = 43;
    constexpr cxx_key_t CXX_KEY_Clear   = 44;

    /* Keyboard group 5 - 83 Premium CE key names */
    constexpr cxx_key_t CXX_KEY_Annul    = CXX_KEY_Clear;

    /* Keyboard group 7 */
    constexpr cxx_key_t CXX_KEY_Down    = 45;
    constexpr cxx_key_t CXX_KEY_Left    = 46;
    constexpr cxx_key_t CXX_KEY_Right   = 47;
    constexpr cxx_key_t CXX_KEY_Up      = 48;

    constexpr cxx_key_t CXX_KEY_NumKeys = CXX_KEY_Up + 1;

    constexpr cxx_key_t CXX_KEY_UNFOCUSED = CXX_KEY_NumKeys;

    extern const kb_lkey_t CXX_KEY_MAP[CXX_KEY_NumKeys];

    typedef struct {
        cxx_key_t key;
        uint8_t cnt; // count.
    } cxx_key_count_t;
    
    // Key manager will not be initializable by the user.
    // Can be multiple alive at once.... if that is what the user wants.
    class KeyManager : public SafeObject {
    private:
        // Maps cxx_key_t's to their index in the keyCounts array.
        // (If they are focused)
        //
        // If they are not focused, keyMap[key] will be equal to 
        // CXX_KEY_UNFOCUSED.
        uint8_t keyMap[CXX_KEY_NumKeys];
        
        SafeArray<cxx_key_count_t> *keyCounts;
        uint8_t repeatDelay;

    public:
        KeyManager();
        KeyManager(uint8_t memChnl);

        ~KeyManager();

        void setRepeatDelay(uint8_t repeatDelay);
        void setFocusedKeys(const cxx_key_t *keys, uint8_t len);
        void scanFocusedKeys();
        void unfocusAll();

        // NOTE: in below functions behavoir is undefined if
        // key is not focused!

        inline bool isKeyPressed(cxx_key_t key) const {
            const uint8_t count = this->keyCounts->get(this->keyMap[key]).cnt;

            return count == 1 || count == this->repeatDelay;
        }

        inline bool isKeyDown(cxx_key_t key) const {
            return this->keyCounts->get(this->keyMap[key]).cnt >= 1;
        }
    };
}
}
