#include <cxxutil/input.h>

using namespace cxxutil;

kb_lkey_t KeyManager::cxxKeyMap[CXX_KEY_NumKeys] = {
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

KeyManager *KeyManager::singleton = nullptr;
KeyManager *KeyManager::getInstance() {
    if (!singleton) {
        singleton = new KeyManager();
    }

    return singleton;
}

KeyManager::KeyManager() : SafeObject(CXX_KEY_CHNL) {
    for (uint8_t i = 0; i < CXX_KEY_NumKeys; i++) {
        this->keyMap[i] = CXX_KEY_UNFOCUSED;
    }

    this->repeatDelay = 1;
    this->keyCounts = new SafeArray<cxx_key_count_t>(CXX_KEY_CHNL, 0);
}

KeyManager::~KeyManager() {
    // keyCounts will never be NULL!
    delete this->keyCounts;
}

void KeyManager::setRepeatDelay(uint8_t repeatDelay) {
    // NOTE: All focused keys will have there counts set back to 0.
    for (uint8_t i = 0; i < this->keyCounts->getLen(); i++) {
        this->keyCounts->getPtr(i)->count = 0;
    }

    this->repeatDelay = repeatDelay;
}

void KeyManager::setFocusedKeys(cxx_key_t *keys, uint8_t len) {
    // NOTE: Once again, all key counts will be set to 0.
    // This is different than the C library's behavoir.

    // Unfocus all focused keys.
    for (uint8_t i = 0; i < this->keyCounts->getLen(); i++) {
        this->keyMap[this->keyCounts->get(i).key] = CXX_KEY_UNFOCUSED;
    }

    delete this->keyCounts;
    this->keyCounts = new SafeArray<cxx_key_count_t>(CXX_KEY_CHNL, len);

    // Focus given keys.
    for (uint8_t i = 0; i < len; i++) {
        this->keyMap[keys[i]] = i;
        this->keyCounts->set(i, {.key = keys[i], .count = 0}); 
    }
}

void KeyManager::scanFocusedKeys() {
    kb_Scan();

    for (uint8_t i = 0; i < this->keyCounts->getLen(); i++) {
        if (kb_IsDown(cxxKeyMap[this->keyCounts->get(i).key])) {
            if (this->keyCounts->get(i).count < this->repeatDelay) {
                this->keyCounts->getPtr(i)->count++;
            }
        } else {
            this->keyCounts->getPtr(i)->count = 0;
        }
    }
}

void KeyManager::unfocusAll() {
    // unfocus all focused keys.
    for (uint8_t i = 0; i < this->keyCounts->getLen(); i++) {
        this->keyMap[this->keyCounts->get(i).key] = CXX_KEY_UNFOCUSED;
    }

    // Change key counts with new empty array.
    delete this->keyCounts;
    this->keyCounts = new SafeArray<cxx_key_count_t>(CXX_KEY_CHNL, 0);
}




