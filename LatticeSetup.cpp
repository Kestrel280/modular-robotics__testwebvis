#include "LatticeSetup.h"
#include "ConfigurationSpace.h"
#include "MetaModule.h"

namespace LatticeSetup {
    void setupFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file " << filename << std::endl;
            return;
        }
        nlohmann::json j;
        file >> j;
        Lattice::InitLattice(j["order"], j["axisSize"]);
        for (const auto& module : j["modules"]) {
            std::vector<int> position = module["position"];
            std::transform(position.begin(), position.end(), position.begin(),
                        [](int coord) { return coord; });
            std::valarray<int> coords(position.data(), position.size());
            if (module.contains("color")) {
                Lattice::AddModule(coords, module["static"], module["color"]);
            } else {
                Lattice::AddModule(coords, module["static"]);
            }
        }
        Lattice::BuildMovableModules();
    }

    Configuration* setupFinalFromJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file " << filename << std::endl;
            return nullptr;
        }
        nlohmann::json j;
        file >> j;
        CoordTensor<bool> desiredState(Lattice::Order(), Lattice::AxisSize(), false);
        CoordTensor<std::string> colors(Lattice::Order(), Lattice::AxisSize(), "");
        for (const auto& module : j["modules"]) {
            std::vector<int> position = module["position"];
            std::transform(position.begin(), position.end(), position.begin(),
                        [](int coord) { return coord; });
            std::valarray<int> coords(position.data(), position.size());
            desiredState[coords] = true;
            if (module.contains("color")) {
                colors[coords] = module["color"];
            }
        }
        return new Configuration(desiredState, colors);
    }

    void setupInitial(const std::string& filename) {
        Lattice::InitLattice(2, 9);
        std::vector<std::vector<char>> image;
        int x = 0;
        int y = 0;
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file " << filename << std::endl;
            return;
        }
        std::string line;
        while (std::getline(file, line)) {
            for (char c : line) {
                if (c == '1') {
                    std::valarray<int> coords = {x, y};
                    Lattice::AddModule(coords);
                } else if (c == '@') {
                    std::valarray<int> coords = {x, y};
                    Lattice::AddModule(coords, true);
                }
                x++;
            }
            x = 0;
            y++;
        }
        Lattice::BuildMovableModules();
    }

    Configuration* setupFinal(const std::string& filename) {
        int x = 0;
        int y = 0;
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Unable to open file " << filename << std::endl;
            return nullptr;
        }
        CoordTensor<bool> desiredState(Lattice::Order(), Lattice::AxisSize(), false);
        CoordTensor<std::string> colors(Lattice::Order(), Lattice::AxisSize(), "");
        std::string line;
        while (std::getline(file, line)) {
            for (char c : line) {
                if (c == '1') {
                    std::valarray<int> coords = {x, y};
                    desiredState[coords] = true;
                }
                x++;
            }
            x = 0;
            y++;
        }
        return new Configuration(desiredState, colors);
    }

    void setUpMetamodule(MetaModule* metamodule) {
        Lattice::InitLattice(metamodule->order, metamodule->size);
        for (const auto &coord: metamodule->coords) {
            Lattice::AddModule(coord);
        }
    }

    void setUpTiling() {
        Lattice::InitLattice(MetaModuleManager::order, MetaModuleManager::axisSize);
        for (int i = 0; i < MetaModuleManager::axisSize / MetaModuleManager::metamodules[0]->size; i++) {
            for (int j = 0; j < MetaModuleManager::axisSize / MetaModuleManager::metamodules[0]->size; j++) {
                if ((i%2==0 && j&1) || (i&1 && j%2 == 0)) {
                    for (const auto &coord: MetaModuleManager::metamodules[5]->coords) {
                        std::valarray<int> newCoord = {MetaModuleManager::metamodules[5]->size * i, MetaModuleManager::metamodules[5]->size * j};
                        Lattice::AddModule(coord + newCoord);
                    }
                } else {
                    for (const auto &coord: MetaModuleManager::metamodules[0]->coords) {
                        std::valarray<int> newCoord = {MetaModuleManager::metamodules[0]->size * i, MetaModuleManager::metamodules[0]->size * j};
                        Lattice::AddModule(coord + newCoord);
                    }
                }
            }
        }
        // for (const auto &coord: MetaModuleManager::metamodules[0]->coords) {
        //     Lattice::AddModule(coord);
        // }
        // for (const auto &coord: MetaModuleManager::metamodules[5]->coords) {
        //     std::valarray<int> newCoord = {3, 0};
        //     Lattice::AddModule(coord + newCoord);
        // }
        // for (const auto &coord: MetaModuleManager::metamodules[0]->coords) {
        //     std::valarray<int> newCoord = {3, 3};
        //     Lattice::AddModule(coord + newCoord);
        // }
        // for (const auto &coord: MetaModuleManager::metamodules[5]->coords) {
        //     std::valarray<int> newCoord = {0, 3};
        //     Lattice::AddModule(coord + newCoord);
        // }
    }
};