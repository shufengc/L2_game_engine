#include "Map.h"

MapCell Map::empty_cell;

// Remove an actor from the cell.
// Return true if the actor is successfully removed, false otherwise.
// bool MapCell::remove_actor(Actor *actor) {
//     for (size_t i = 0; i < actors.size(); i++) {
//         if (actors[i] == actor) {
//             // Swap the actor with the last actor and pop the last actor
//             actors[i] = actors.back();
//             actors.pop_back();
//             return true;
//         }
//     }
//     return false;
// }

// Print the map for debugging.
void Map::print() const {
    std::cout << "size: " << cells.size() << std::endl;
    size_t min_size = cells.empty() ? 0 : std::numeric_limits<size_t>::max();
    size_t max_size = 0;
    for (auto &pair : cells) {
        size_t sz = pair.second.get_actors().size();
        std::cout << sz;

#ifdef DEBUG
        for (auto &actor : pair.second.get_actors()) {
            // std::cout << " " << actor->name << "-" << actor->get_id();
            std::cout << " " << actor;
        }
#endif
        std::cout << std::endl;

        if (sz < min_size) {
            min_size = sz;
        }
        if (sz > max_size) {
            max_size = sz;
        }
    }
    std::cout << "min size: " << min_size << std::endl;
    std::cout << "max size: " << max_size << std::endl;
}
