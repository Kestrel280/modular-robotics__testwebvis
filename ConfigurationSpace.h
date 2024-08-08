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
#define CONFIG_VERBOSE CS_LOG_EVERY_DEPTH

class BFSExcept : std::exception {
public:
    [[nodiscard]]
    const char * what() const noexcept override;
};

// For comparing the state of a lattice and a configuration
class HashedState {
private:
    size_t seed;
public:
    HashedState();

    explicit HashedState(size_t seed);

    explicit HashedState(const std::set<ModuleBasic>& modData);

    HashedState(const HashedState& other);

    [[nodiscard]]
    size_t GetSeed() const;

    bool operator==(const HashedState& other) const;

    bool operator!=(const HashedState& other) const;
};

namespace std {
    template<>
    struct hash<HashedState> {
        size_t operator()(const HashedState& state) const;
    };
}

// For tracking the state of a lattice
class Configuration {
private:
    Configuration* parent = nullptr;
    std::vector<Configuration*> next;
    std::set<ModuleBasic> _nonStatModData;
    HashedState hash;
    int cost;
public:
    int depth = 0;

    explicit Configuration(std::set<ModuleBasic> modData);

    ~Configuration();

    std::vector<std::set<ModuleBasic>> MakeAllMoves();

    void AddEdge(Configuration* configuration);

    Configuration* GetParent();

    std::vector<Configuration*> GetNext();

    [[nodiscard]]
    const HashedState& GetHash() const;

    [[nodiscard]]
    const std::set<ModuleBasic>& GetModData() const;

    void SetParent(Configuration* configuration);

    friend std::ostream& operator<<(std::ostream& out, const Configuration& config);

    int GetCost();

    void SetCost(int cost);

    float Heuristic(Configuration* final);

    int SymmetricDifferenceHeuristic(Configuration* final);
};

namespace ConfigurationSpace {
    extern int depth;

    std::vector<Configuration*> BFS(Configuration* start, Configuration* final);

    std::vector<Configuration*> BFSParallelized(Configuration* start, Configuration* final);

    std::vector<Configuration*> AStar(Configuration* start, Configuration* final);

    std::vector<Configuration*> FindPath(Configuration* start, Configuration* final);
}

#endif //MODULAR_ROBOTICS_CONFIGURATIONSPACE_H
