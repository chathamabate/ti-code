#ifndef GAME_HPP
#define GAME_HPP

#include <stdint.h>

// Macro for very specific use in Game.
#define __RequestTypeCheck(req) \
    if ((req).type != RequestType::CONTINUE) { \
        return req; \
    }

namespace cxxutil {

    // In these class templates, T will be the type
    // of the global state held in the game.
    template<typename T> class Game;
    template<typename T> class GameState;
    template<typename T> class TransitionState;

    // Games need to hold ...
    //      A global state.
    //      A current game state.
    //      The ability to transition between game states.

    enum class RequestType {
        // All functions returning a Status can return
        // CONTINUE to signal that all went correctly.
        CONTINUE,  

        // EXIT signals some chain of events to end
        // due to some non erroneous reason.
        EXIT,       

        // FAIL requests immediate exit due to
        // some error.
        FAIL,       
    };

    // Request from a function outlining what to do next.
    typedef struct {
        RequestType type;

        // User customizable code to add detail to status.
        // Could be an error code in case of failure.
        // Also could be a code telling exit what transition
        // state to return.
        uint8_t code;  
    } Request;

    // Classic Continue, Exit, and Fail Requests
    extern const Request C_REQ, EX_REQ, F_REQ;

    // This is an abstract class which represents
    // some functionality which can only run once.
    class SingleRun {
        private:
            bool finished;
            Request endReq;

            // NOTE, this will only ever be called once.
            virtual Request run() = 0;

        public:
            SingleRun();
            bool hasFinished();
            Request getEndRequest();
    };

    template<typename T>
    class Game : SingleRun {
        private:
            T *globalState;
            TransitionState<T> *firstTransState;

            // Used to generate and store the first
            // global state. Dynamic data should only be
            // left over if the Request returned in a continue.
            virtual Request initGlobalState() = 0;

            // NOTE, the global state will be deleted
            // So make sure your global state implements
            // a destructor.
            
            // NOTE, this will be called after initGlobalState.
            // Will generate and store first Transition State.
            // Said transition will be used to generate first GameState.
            virtual Request initTransState() = 0;

            virtual Request run() override {
                Request req;
    
                req = this->initGlobalState();
                if (req.type != RequestType::CONTINUE) {
                    return req;
                }

                // This should store the first transition
                // state in firstTransState.
                req = this->initTransState();
                if (req.type != RequestType::CONTINUE) {
                    // NOTE, on exit case, the global state
                    // will be disposed of?
                    // Do we want this tho???? Don't know for sure...
                    delete this->globalState;
                    this->globalState = nullptr;

                    return req;
                }

                // Fill in at somepoint.
                TransitionState<T> *ts = firstTransState;
                firstTransState = nullptr;
               
                GameState<T> *gs = nullptr;

                while (req.type == RequestType::CONTINUE) {
                    // Here we have a valid trans state in ts. 
                    // Generate next game state.
                    req = ts->getEndRequest();

                    // This will always run regardless of end result.
                    // This is to ensure the transition state is 
                    // deleted.
                    gs = ts->getNextGS();

                    delete ts;
                    ts = nullptr;

                    if (req.type != RequestType::CONTINUE) {
                        break;
                    }

                    // Throw out last trans state.

                    // Execute game state!
                    req = gs->getEndRequest();

                    ts = gs->getNextTrans();

                    delete gs;
                    gs = nullptr;
                }

                // Do we need to check for fail here?
                return req;
            }

        protected:
            void setGlobalState(T *gls) {
                this->globalState = gls;
            }

            void setTransState(TransitionState<T> *ts) {
                this->transState = ts;
            }

        public:
            Game() {
                this->globalState = nullptr;
                this->transState = nullptr;
            }

            // This shouldn't need to be virtual
            // as everything of importance should be
            // stored in the Global state to be accessible
            // by child game states.
            ~Game() {
                if (this->firstTransState) {
                    delete this->firstTransState;
                }

                if (this->globalState) {
                    delete this->globalState;
                }
            }
    };


    template<typename T>
    class GameState : public SingleRun {
        private:
            uint16_t del; // Game loop delay.

            T *globalState;
            TransitionState<T> *nextTrans;

            virtual Request run() override {
                Request req = this->enter();
                
                if (req.type == RequestType::CONTINUE) {
                    req = this->update();
                    while (req.type == RequestType::CONTINUE) {
                        this->render();

                        delay(del);

                        req = this->update();
                    }
                }
                
                if (req.type == RequestType::EXIT) {
                    req = this->exit(req.code);
                }

                return req;
            }

            // Life Cycle methods for a GameState.

            // Enter a game state. Usually does initial
            // rendering or setup. Shouldn't ever return 
            // an EXIT request... but can.
            virtual Request enter() = 0;

            // Update state of game state, shouldn't do any
            // rendering.
            virtual Request update() = 0;

            // Only render game state, shouldn't do any
            // updating.
            virtual void render() = 0;

            // This will be called if enter or update request
            // EXIT's. If a fail is returned from enter or
            // update, this will not be called.
            //
            // It should construct the transition state to prepare
            // for the next game state. NOTE The new trans state should
            // be stored in nextTrans. 
            //
            // In the case the game is being exited, make sure not
            // to create some transition state... this will not 
            // be freed if so.
            virtual Request exit(uint8_t exit_code) = 0;

        protected:
            T *getGlobalState() {
                return this->globalState;
            }

            void setNextTrans(TransitionState<T> *t) {
                this->nextTrans = t;
            }

        public:
            GameState(uint16_t d, T *glbst) {
                this->del = d;
                this->globalState = glbst;
                this->nextTrans = nullptr;
            }

            virtual ~GameState() { }

            TransitionState<T> *getNextTrans() {
                return this->nextTrans;
            }
    };

    template<typename T>
    class TransitionState : public SingleRun {
        private:
            T *globalState;
            GameState<T> *nextGS;

        // NOTE, run() should be implemented as
        // creating the next GameState and storing it
        // in nextGS.
        //
        // If the game must be exited, make sure nextGS
        // is not initialized as it will not be freed.
        
        protected:
            T *getGlobalState() {
                return this->globalState;
            }

            void setNextGS(GameState<T> *ngs) {
                this->nextGS = ngs;
            }

        public:
            TransitionState(T *g) {
                this->globalState = g;
                this->finished = false;
                this->nextGS = nullptr;
            }

            virtual ~TransitionState() { }

            GameState<T> *getNextGS() {
                return this->nextGS;
            }
    };
}

#endif
