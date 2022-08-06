#include "game.h"

using namespace cxxutil;

const Request C_REQ = {
    .type = RequestType::CONTINUE,
    .code = 0
};

const Request EX_REQ = {
    .type = RequestType::EXIT,
    .code = 0
};

const Request F_REQ = {
    .type = RequestType::FAIL,
    .code = 0
};

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

