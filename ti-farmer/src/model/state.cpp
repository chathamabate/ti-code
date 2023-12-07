

#include "./state.h"

using namespace tif::model;

bool PlanetState::plant(uint8_t r, uint8_t c, uint8_t cropInd) {
    cell_state_t *cell = this->getCellStatePtr(r, c);

    if (!(this->isPlantablePtr(cell))) {
        return false;
    }

    // Wait... but is it unlocked yet???
    return true;
}

bool PlanetState::harvest(uint8_t r, uint8_t c) {
    return false;
}

bool PlanetState::clear(uint8_t r, uint8_t c) {
    return false;
}

bool PlanetState::place(uint8_t r, uint8_t c, uint8_t featureInd) {
    return false;
}



