
#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cxxutil/test.h>
#include <cxxutil/game.h>

using namespace cxxutil;

class FrameCounter {
    private:
        uint8_t counter;
    public:
        FrameCounter() {
            this->counter = 0;
        }

        void incCounter() {
            counter++;
        }

        uint8_t getCounter() {
            return counter;
        }
};

#define GS_DELAY 100

#define CS_KEYS_LEN 3
const c_key_t CS_KEYS[CS_KEYS_LEN] = {
    c_8, c_5, c_Enter
};

const Request C_REQ = {.type = RequestType::CONTINUE, .code = 0};

class ColorSelector : public GameState<FrameCounter> {
    private:
        uint8_t bg_color;

        virtual Request enter() override {
            set_focused_keys(CS_KEYS, CS_KEYS_LEN);
            gfx_SetTextScale(2, 4);
            return C_REQ;
        }

        virtual Request update() override {
            scan_focused_keys();

            // Exit to frame display state.
            if (key_press(c_Enter)) {
                return { .type = RequestType::EXIT, .code = 0 };
            }

            if (key_press(c_8)) {
                this->bg_color++;
            } else if (key_press(c_5)) {
                this->bg_color--;
            }

            return C_REQ;
        }

        virtual void render() override {
            gfx_FillScreen(this->bg_color);

            char buff[4];
            sprintf(buff, "%d", this->getGlobalState()->getCounter());
            gfx_PrintStringXY(buff, (LCD_WIDTH - gfx_GetStringWidth(buff)) / 2, 50);

            gfx_SwapDraw();
        }

        virtual Request exit(uint8_t exit_code) override {
            // Set up transition state!!!!
        }
   public:
        ColorSelector(FrameCounter *glbst, uint8_t bg) : GameState(GS_DELAY, glbst) {
            this->bg_color = bg;
        }
};

#define CNT_KEYS_LEN 2
const c_key_t CNT_KEYS[CNT_KEYS_LEN] = {
    c_Enter, c_Clear
};

class CounterState : public GameState<FrameCounter> {
    private:
        virtual Request enter() override {
            set_focused_keys(CNT_KEYS, CNT_KEYS_LEN);
            gfx_SetTextScale(2, 4);
            return C_REQ;
        }

        virtual Request update() override {
            scan_focused_keys();


            return C_REQ;
        }

        virtual void render() override {
            char buff[4];
            sprintf(buff, "%d", this->getGlobalState()->getCounter());
            gfx_PrintStringXY(buff, (LCD_WIDTH - gfx_GetStringWidth(buff)) / 2, 50);

            gfx_SwapDraw();
        }

        virtual Request exit(uint8_t exit_code) override {
            // Set up transition state!!!!
        }
   public:
        ColorSelector(Game<FrameCounter> *g, uint8_t bg) : GameState(g) {
            this->bg_color = bg;
        }
};

int main(void) {

    // Lambdas Work!!!!
    // auto f = [] (int a, int b) {return a + b;};
    
    // One or two more fields per shape not a big deal...
    Shape *shapes[] = {
        new Rectangle(3, 5),
        new Square(2)
    };
    
    os_ClrHomeFull();

    for (int i = 0; i < 2; i++) {
        delete shapes[i];
        os_NewLine();
    }

    while (!os_GetCSC());

    return 0;
}
