#include "keys.h"
#include "misc.h"
#include <stdlib.h>

kb_lkey_t c_key_map[c_NumKeys] = {
    /* Keyboard group 1 */
    kb_KeyGraph,     
    kb_KeyTrace,     
    kb_KeyZoom,      
    kb_KeyWindow,    
    kb_KeyYequ,      
    kb_Key2nd,       
    kb_KeyMode,      
    kb_KeyDel,       

    /* Keyboard group 2 */
    kb_KeySto,       
    kb_KeyLn,        
    kb_KeyLog,       
    kb_KeySquare,    
    kb_KeyRecip,     
    kb_KeyMath,      
    kb_KeyAlpha,     

    /* Keyboard group 3 */
    kb_Key0,         
    kb_Key1,         
    kb_Key4,         
    kb_Key7,         
    kb_KeyComma,     
    kb_KeySin,       
    kb_KeyApps,      
    kb_KeyGraphVar,  

    /* Keyboard group 4 */
    kb_KeyDecPnt,    
    kb_Key2,         
    kb_Key5,         
    kb_Key8,         
    kb_KeyLParen,    
    kb_KeyCos,       
    kb_KeyPrgm,      
    kb_KeyStat,      

    /* Keyboard group 5 */
    kb_KeyChs,      
    kb_Key3,        
    kb_Key6,        
    kb_Key9,        
    kb_KeyRParen,   
    kb_KeyTan,      
    kb_KeyVars,     

    /* Keyboard group 6 */
    kb_KeyEnter,    
    kb_KeyAdd,      
    kb_KeySub,      
    kb_KeyMul,      
    kb_KeyDiv,      
    kb_KeyPower,    
    kb_KeyClear,    

    /* Keyboard group 7 */
    kb_KeyDown,     
    kb_KeyLeft,     
    kb_KeyRight,    
    kb_KeyUp
};

// Way of noting the a key is unfocused in the key map.
#define UNFOCUSED c_NumKeys

// Hacky solution for initializing the key map to all UNFOCUSED.
#define _UF_3   UNFOCUSED, UNFOCUSED, UNFOCUSED
#define _UF_12  _UF_3, _UF_3, _UF_3, _UF_3 
#define _UF_48  _UF_12, _UF_12, _UF_12, _UF_12
#define _UF_49  _UF_48, UNFOCUSED

static uint8_t num_focused_keys;
focused_key *focused_keys = NULL;
uint8_t key_map[c_NumKeys] = { _UF_49 };
uint8_t repeat_delay = 1; // Starts with no delay.

void set_focused_keys(const c_key_t *keys, uint8_t num_keys) {
    focused_key *focused_keys_temp = focused_keys;
    uint8_t key_map_temp[c_NumKeys];

    // First, copy key_map into key_map_temp.
    uint8_t i;
    for (i = 0; i < c_NumKeys; i++) {
        key_map_temp[i] = key_map[i];
        key_map[i] = UNFOCUSED; // Unfocus all keys in the real key map.
    }

    // Create new focused keys array.
    num_focused_keys = num_keys;

    // Will be NULL if num_focused_keys == 0.
    focused_keys = (focused_key *)safe_malloc(sizeof(focused_key) * num_keys);

    for (i = 0; i < num_focused_keys; i++) {
        focused_keys[i].key = keys[i];
        focused_keys[i].count = key_map_temp[keys[i]] == UNFOCUSED 
            ? 0 
            : focused_keys_temp[key_map_temp[keys[i]]].count;

        key_map[keys[i]] = i;
    }

    // Finally, free the old focused keys array.
    if (!focused_keys_temp) {
        free(focused_keys_temp);
    }
}

void scan_focused_keys(void) {
    kb_Scan(); // Perform initial Scan.

    uint8_t i;
    for (i = 0; i < num_focused_keys; i++) {
        if (kb_IsDown(to_kb_lkey_t(focused_keys[i].key))) {
            if (focused_keys[i].count < repeat_delay) {
                focused_keys[i].count++;
            }
        } else {
            focused_keys[i].count = 0;
        }
    }
}
