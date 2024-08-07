#pragma once

#include <cxxutil/core/mem.h>

namespace cxxutil { namespace app {

    template<typename G> 
    class AppState : public core::SafeObject {
    private:
        G * const globalState;

        virtual void init() = 0;
        virtual AppState<G> *run() = 0;
        virtual void finally() = 0;

    protected:
        AppState(uint8_t memChnl, G *gs) 
            : core::SafeObject(memChnl), globalState(gs) {
        }

        AppState(G *gs) 
            : AppState(core::CXX_DEF_CHNL, gs) {
        }

    public:
        virtual ~AppState() {
        }

        inline G *getGlobalState() const {
            return this->globalState;
        }

        inline AppState<G> *complete() {
            this->init();
            AppState<G> *result = this->run();
            this->finally();

            return result;
        }

        // NOTE: this will not delete init state.
        // Created states will be deleted though.
        static void runApp(AppState<G> *initState) {
            AppState<G> *currState = initState->complete();

            while (currState) {
                AppState<G> *nextState = currState->complete();
                delete currState;
                currState = nextState;
            }
        }
    };


    template<typename G>
    class LoopState : public AppState<G> {
    private:
        const uint16_t del;
        
        bool exitRequested;
        AppState<G> *nextState;

        bool redrawRequested;

        virtual void update() = 0;
        virtual void render() = 0; 

        virtual AppState<G> *run() override {
            while (!(this->exitRequested)) {
                this->update();

                if (this->redrawRequested) {
                    this->render();
                    this->redrawRequested = false;
                }

                // NOTE: Consider making this more variable...
                delay(this->del);
            } 

            return this->nextState;
        }

    protected:
        LoopState(uint8_t memChnl, G *gs, uint16_t d)
            : AppState<G>(memChnl, gs), del(d) {
            this->nextState = nullptr;
            this->exitRequested = false;
            this->redrawRequested = true;
        }

        LoopState(G *gs, uint16_t d) 
            : LoopState(core::CXX_DEF_CHNL, gs, d) {
        }

        inline void requestRedraw() {
            this->redrawRequested = true;
        }

        inline void requestExit(AppState<G> *ns) {
            this->nextState = ns;
            this->exitRequested = true;
        }
    public:
        virtual ~LoopState() {
        }
    };
     
}}
