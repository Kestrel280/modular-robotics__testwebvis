#include <vector>
#include "Lattice.h"

#ifndef MODULAR_ROBOTICS_CONFIGURATIONSPACE_H
#define MODULAR_ROBOTICS_CONFIGURATIONSPACE_H

// Verbosity Constants (Don't change these)
#define CS_LOG_NONE 0
#define CS_LOG_FINAL_DEPTH 1
#define CS_LOG_EVERY_DEPTH 2
/* Verbosity Configuration
 * NONE: No output from BFS
 * FINAL_DEPTH: Output final depth and configuration upon BFS completion
 * EVERY_DEPTH: Output current depth and configuration every time BFS depth increases
 */
#define CONFIG_VERBOSE CS_LOG_FINAL_DEPTH

class HashedState {
private:
    size_t seed;
public:
    HashedState();

    explicit HashedState(size_t seed);

    explicit HashedState(const CoordTensor<bool>& state);

    HashedState(const HashedState& other);

    [[nodiscard]]
    size_t GetSeed() const;

    void HashLattice(const Lattice& lattice);

    void HashCoordTensor(const CoordTensor<bool>& state);

    bool operator==(const HashedState& other) const;
};

namespace std {
    template<>
    struct hash<HashedState> {
        size_t operator()(const HashedState& state) const;
    };
}

class Configuration {
private:
    Configuration* parent = nullptr;
    std::vector<Configuration*> next;
    CoordTensor<bool> _state;
    HashedState hash;
public:
    int depth = 0;
    explicit Configuration(CoordTensor<bool> state);

    ~Configuration();

    std::vector<CoordTensor<bool>> MakeAllMoves(Lattice& lattice);

    void AddEdge(Configuration* configuration);

    Configuration* GetParent();

    std::vector<Configuration*> GetNext();

    [[nodiscard]]
    const CoordTensor<bool>& GetState() const;

    [[nodiscard]]
    const HashedState& GetHash() const;

    void SetStateAndHash(const CoordTensor<bool>& state);

    void SetParent(Configuration* configuration);

    friend std::ostream& operator<<(std::ostream& out, const Configuration& config);
};

namespace ConfigurationSpace {
    std::vector<Configuration*> BFS(Configuration* start, Configuration* final, Lattice& lattice);

    std::vector<Configuration*> FindPath(Configuration* start, Configuration* final);
}

#endif //MODULAR_ROBOTICS_CONFIGURATIONSPACE_H
