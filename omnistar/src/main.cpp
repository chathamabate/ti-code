
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <graphx.h>

#include <cxxutil/test.h>
#include <cxxutil/game.h>

using namespace cxxutil;

class SimpleGlobalState {
    private:
        uint8_t counter;
    public:
        SimpleGlobalState() { }

        void incCounter() {
            counter++;
        }

        uint8_t getCounter() {
            return counter;
        }
};

class FirstTrans : public TransitionState<SimpleGlobalState> {
    private:   
        virtual  Request run() override {
            this->setNextGS();
        }

    public:
        FirstTrans(Game<SimpleGlobalState> *g) : TransitionState(g) { }
};

class SimpleGame : public Game<SimpleGlobalState> {
    private:
        virtual Request initGlobalState() override {
            this->setGlobalState(new SimpleGlobalState());
        }

        virtual Request initTransState() override {
            this->setTransState(new FirstTrans(this));
        }

    public:
        SimpleGame() {

        }
};

class TransState1 : public TransitionState<SimpleGlobalState> {
    private:
        virtual  Request run() override {
        }

    public:
        TransState1(Game<SimpleGlobalState> *g) : TransitionState(g) { }

};

class GameState1 : public GameState<SimpleGlobalState> {
    private:
        virtual Request enter() override {
            return { .type = RequestType::CONTINUE, .code = 0 }  
        }

        virtual Request update() override {

        }

        virtual void render() override {
            gfx_FillScreen(0);
        }

        virtual Request exit(uint8_t exit_code) override {

        }
        
    public:
        GameState1(Game<SimpleGlobalState> *g) : GameState(g) { }
};

class TransState2 : public TransitionState<SimpleGlobalState> {

};

class GameState2 : public GameState<SimpleGlobalState> {
    private:
        virtual Request enter() override {

        }

        virtual Request update() override {

        }

        virtual void render() override {

        }

        virtual Request exit(uint8_t exit_code) override {

        }
};

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
