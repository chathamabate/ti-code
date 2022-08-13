#ifndef GAME_EXAMPLE_HPP
#define GAME_EXAMPLE_HPP

#include <stdint.h>
#include <cxxutil/game.h>

// Here is some example game code!
// The Game is that you can move a cell around the screen.
// There will be a menu state and a gameplay state?
// Total moves for the entire session will be stored in 
// a global state.

class MoveCounter {
    private:
        uint8_t counter;
    public:
        MoveCounter();
        uint8_t getCounter();
        void setCounter(uint8_t c);
};

class IntoStartScreen : public cxxutil::TransitionState<MoveCounter> {
    private:
        cxxutil::Request run() override;
    public:
        IntoStartScreen(MoveCounter *mc);
};

class StartScreen : public cxxutil::GameState<MoveCounter> {
    private:
        bool redraw;
        cxxutil::Request enter() override;
        cxxutil::Request update() override;
        void render() override;
        cxxutil::Request exit(uint8_t exit_code) override;
    public:
        StartScreen(MoveCounter *mc);
};

class IntoPlayScreen : public cxxutil::TransitionState<MoveCounter> {
    private:
        cxxutil::Request run() override;
    public:
        IntoPlayScreen(MoveCounter *mc);
};

class PlayScreen : public cxxutil::GameState<MoveCounter> {
    private:
        bool redraw;
        uint8_t pos_x, pos_y;

        cxxutil::Request enter() override;
        cxxutil::Request update() override;
        void render() override;
        cxxutil::Request exit(uint8_t exit_code) override;
    public:
        PlayScreen(MoveCounter *mc);
};

#endif
