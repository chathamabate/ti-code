#include "game.h"

using namespace cxxutil;


SingleRun::SingleRun() {
    this->finished = false;
}

bool SingleRun::hasFinished() {
    return this->finished;
}

Request SingleRun::getEndRequest() {
    if (!this->finished) {
        this->endReq = this->run(); 
        this->finished = true;
    }

    return this->endReq;
}

