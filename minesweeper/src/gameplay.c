
#include <cutil/gameloop.h>
#include <cutil/keys.h>

#define FOCUSED_KEYS_LEN 1
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear    
}

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return NULL;
}

static const loc_life_cycle *update_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static void render_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_gameplay(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    return NULL;
}

const loc_life_cycle GAMEPLAY = {
    .enter = enter_gameplay,
    .update = update_gameplay,
    .render = render_gameplay,
    .exit = exit_gameplay
};
