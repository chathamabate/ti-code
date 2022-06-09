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
    kb_KeyUp,       
};

static uint8_t num_focused_keys;
focused_key *focused_keys = NULL;
uint8_t key_map[c_NumKeys];
uint8_t repeat_delay = 1; // Starts with no delay.

void set_focused_keys(const c_key_t *keys, uint8_t num_keys) {
    if (!focused_keys) {
        free(focused_keys);
    }

    num_focused_keys = num_keys;

    // Way of seeting no focused keys...
    if (keys == NULL || num_keys == 0) {
        focused_keys = NULL;
    }

    focused_keys = (focused_key *)safe_malloc(sizeof(focused_key) * num_keys);

    uint8_t i;
    for (i = 0; i < num_keys; i++) {
        focused_keys[i].key = keys[i];
        focused_keys[i].count = 0;

        key_map[keys[i]] = i;
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
