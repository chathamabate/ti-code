#ifndef GAME_HPP
#define GAME_HPP

#include <stdint.h>

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
    class Game {
        private:
            T *globalState;

            TransitionState<T> *transState;
            GameState<T> *gameState;

            // Used to generate and store the first
            // global state. 
            virtual Request initGlobalState() = 0;

            // NOTE, the global state will be deleted
            // in the game's destructor... So make
            // sure your global state implements
            // a destructor.
            
            // NOTE, this will be called after initGlobalState.
            // Will generate and store first Transition State.
            // Said transition will be used to generate first GameState.
            virtual Request initTransState() = 0;

        public:
            Game() {
                globalState = nullptr;
                transState = nullptr;
                gameState = nullptr;
            }

            virtual ~Game() {
                if (gameState) {
                    delete gameState;
                }

                if (transState) {
                    delete transState;
                }

                if (globalState) {
                    delete globalState;
                }
            }

            // Global state must be accessible for game states and transition
            // states.
            T *getGlobalState() {
                return this->globalState;
            }

            // Run the game and return how it exited.
            Request run() {
                Request si;
                return si;
                
            }
    };


    template<typename T>
    class GameState : public SingleRun {
        private:
            Game<T> *game;
            TransitionState<T> *nextTrans;

            virtual Request run() override {
                Request req = this->enter();
                
                if (req.type == RequestType::CONTINUE) {
                    req = this->update();
                    while (req.type == RequestType::CONTINUE) {
                        this->render();
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
            // for the next game state. The new trans state should
            // be stored in nextTrans. 
            virtual Request exit(uint8_t exit_code) = 0;

        public:
            GameState(Game<T> *g) {
                this->game = g;
                this->nextTrans = nullptr;
            }

            virtual ~GameState() { }
    };

    template<typename T>
    class TransitionState : public SingleRun {
        private:
            Game<T> *game;
            GameState<T> *nextGS;

        // NOTE, run() should be implemented as
        // creating the next GameState.

        public:
            TransitionState(Game<T> g) {
                this->game = g;
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
