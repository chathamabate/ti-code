#ifndef GAME_HPP
#define GAME_HPP

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

    enum class Status {
        CONTINUE,   // Continue the game or game state.
        EXIT,       // Exit a game state.
        FAIL,       // Game halts due to error.
    };

    // Helper struct for passing status information between
    // states and life cycle methods.
    typedef struct {
        Status status;
        const char *info;
    } StatusInfo;

    template<typename T>
    class Game {
        private:
            T *globalState;

            GameState<T> *gameState;
            TransitionState<T> *transState;

        protected:
            // Used to generate and store the first
            // global state. 
            virtual StatusInfo initGlobalState() = 0;

            // NOTE, the global state will be deleted
            // in the game's destructor... So make
            // sure your global state implements
            // a destructor.
            
            // NOTE, this will be called after initGlobalState.
            // Will generate and store first Transition State.
            // Said transition will be used to generate first GameState.
            virtual StatusInfo initTransState() = 0;

        public:
            Game();
            virtual ~Game();

            // Global state must be accessible for game states and transition
            // states.
            T *getGlobalState();

            // Run the game and return how it exited.
            StatusInfo run();
    };


    template<typename T>
    class GameState {
        private:
            Game<T> *game;

        protected:
            // Life Cycle methods for a GameState.

            // Enter a game state. Usually does initial
            // rendering or setup.
            virtual StatusInfo enter() = 0;

            // Update state of game state, shouldn't do any
            // rendering.
            virtual StatusInfo update() = 0;

            // Only render game state, shouldn't do any
            // updating.
            virtual StatusInfo render() = 0;

            // Exit will only be called when all previous life
            // cycle methods have executed correctly.
            // It should construct the transition state to prepare
            // for the next game state. 
            //
            // NOTE, this should not do any deallocation...
            // This should be saved for the destructor.
            virtual TransitionState<T> *exit() = 0;

        public:
            GameState(Game<T> *g);
            virtual ~GameState();

            // Run the game state.
            // Will return a TransitionState
            // for creating the next state.
            TransitionState<T> *runState();
    };

    template<typename T>
    class TransitionState {
        private:
            Game<T> *game;
            StatusInfo transInfo;

        public:
            TransitionState(Game<T> g, StatusInfo ti);
            virtual ~TransitionState();

            StatusInfo getTransInfo();

            // Create the next game state.
            // returns null if this is an exit transition.
            // NOTE, this should never error!
            virtual GameState<T> *transition();
    };
}

#endif
