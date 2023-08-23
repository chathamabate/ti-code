#pragma once

#include <cxxutil/core/mem.h>

namespace cxxutil { namespace app {

    // G for global state.
    template<typename G> class InternalState;
    template<typename G> class TransitionState;
    template<typename G> class AppState;

    // NOTE: this is not an abstract class,
    // but it is also not supposed to be created on its own
    // hence why the constructors and destructor are private.
    template<typename G> 
    class InternalState : public core::SafeObject {
    private:
        G * const globalState;
    protected:
        InternalState(uint8_t memChnl, G *gs) 
            : core::SafeObject(memChnl), globalState(gs) {
        }

        InternalState(G *gs) 
            : InternalState(core::CXX_DEF_CHNL, gs) {
        }

        ~InternalState() {
        }

    public:
        inline G *getGlobalState() const {
            return this->globalState;
        }
    };

    // NOTE: the below classes are somewhat identical.
    // However, there semantic meanings are different.
    
    template<typename G> 
    class TransitionState : public InternalState<G> {
    protected:
        TransitionState(uint8_t memChnl, G *gs) 
            : InternalState<G>(memChnl, gs) {
        }

        TransitionState(G *gs) 
            : TransitionState(core::CXX_DEF_CHNL, gs) {
        }
    public:
        virtual ~TransitionState() {
        }

        // This wil provide a pointer to the next AppState to 
        // use!
        virtual AppState<G> *transition() = 0;
    };

    template<typename G>
    class AppState : public InternalState<G> {
    protected:
        AppState(uint8_t memChnl, G *gs)
            : InternalState<G>(memChnl, gs) {
        }

        AppState(G *gs)
            : AppState(core::CXX_DEF_CHNL, gs) {
        }

    public:
        virtual ~AppState() {
        }

        virtual TransitionState<G> *run() = 0;
    };

    template<typename G>
    class LoopState : public AppState<G> {
    private:
        const uint16_t del;
        
        bool exitRequested;
        TransitionState<G> *exitTransition;

        bool redrawRequested;

        virtual void update() = 0;
        virtual void render() = 0;

    protected:
        LoopState(uint8_t memChnl, G *gs, uint16_t d)
            : AppState<G>(memChnl, gs), del(d) {
            this->exitTransition = nullptr;
            this->exitRequested = false;
            this->redrawRequested = true;
        }

        LoopState(G *gs, uint16_t d) 
            : LoopState(core::CXX_DEF_CHNL, gs, d) {
        }

        inline void requestRedraw() {
            this->exitRequested = true;
        }

        inline void requestExit(TransitionState<G> *et) {
            this->exitTransition = et;
            this->exitRequested = true;
        }
    public:
        virtual ~LoopState() {
        }

        virtual TransitionState<G> *run() override {
            while (!(this->exitRequested)) {
                this->update();

                if (this->redrawRequested) {
                    this->render();
                    this->redrawRequested = false;
                }

                delay(this->del);
            } 

            return this->exitTransition;
        }
    };
     
}}
