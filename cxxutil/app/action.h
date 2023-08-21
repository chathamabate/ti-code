#pragma once

#include <cxxutil/core/mem.h>

namespace cxxutil { namespace app {

    // NOTE:
    // An action is going to be the core unit of organization
    // for all apps.
    class Action : public core::SafeObject {
    private: 
        // NOTE: these are all initially implemented as empty.
        
        // Init should set up the dynamic memory needed
        // for this action. 
        virtual void init();

        // Run should obviously run the action.
        // It should return a pointer to ANOTHER action (never this).
        // If there is no other action to continue to, it
        // should return NULL.
        virtual Action *run();

        // This should cleanup all memory created in
        // init().
        virtual void finally();

    public:
        Action(uint24_t memChnl);
        Action();
        virtual ~Action();

        inline Action *complete() {
            this->init();
            Action *result = this->run();
            this->finally();

            return result;
        }

        // NOTE: This will run actions consecutively until
        // one returns NULL. All actions created will be deleted
        // before this function returns.
        //
        // The given action "a" will NOT be deleted by this call.
        static void begin(Action *a);
    };

    class LoopAction : public Action {
    private:
        virtual Action *run() override;

        // NOTE: right now the full delay is always used
        // consider improving this at somepoint to account
        // for the time used while updating/rendering.
        uint16_t del;

        bool exitRequested;
        Action *nextAction;

        bool redrawRequested;

        virtual void update();
        virtual void render();
    protected:
        inline void requestRedraw() {
            this->redrawRequested = true;
        }

        inline void requestExit(Action *next) {
            this->exitRequested = true;
            this->nextAction = next;
        }
    public:
        LoopAction(uint24_t memChnl);
        LoopAction();
        virtual ~LoopAction();
    };
}}
