
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>

#include <cxxutil/test.h>
#include <cxxutil/game.h>

using namespace cxxutil;
/*

class G : public Game<int> {
    public:
    G() : Game() {
         
    }

    ~G() override {
        // Do nothing.
    }

    private:
    StatusInfo initGlobalState() override {
        return {
            .status = Status::CONTINUE,
            .code = 0
        };
    }

    StatusInfo initTransState() override {
        return {
            .status = Status::CONTINUE,
            .code = 0
        };
    }
};
*/
class GS : public GameState<int> {
    public:
        GS(Game<int> *g) : GameState<int>(g) { }

    protected:
        Request enter() override {
            return {};
        }

        Request update() override {
            return {};
        }

        void render() override {
        }

        Request exit(uint8_t exit_code) override {
            return {};
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
