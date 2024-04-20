#ifndef __MAP_H__
#define __MAP_H__

#include "Actor.h"
#include <cstdint>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// A cell in the map.
// A cell can contain multiple actors.
struct MapCell {
    MapCell() {}

    // Add an actor to the cell.
    void add_actor(Actor *actor) { actors.insert(actor->get_id()); }

    // Remove an actor from the cell.
    void remove_actor(Actor *actor) { actors.erase(actor->get_id()); }

    // Get the actors in the cell.
    const std::unordered_set<int> &get_actors() const { return actors; }

  private:
    std::unordered_set<int> actors;
};

// A custom hash function for glm::ivec2.
//! THIS FUNCTION IS TOO SLOW.
class IVec2CustomerHash {
  public:
    std::size_t operator()(const glm::ivec2 &c) const { return std::hash<int>()(c.x) ^ std::hash<int>()(c.y); }
};

// The game map.
class Map {
    using KeyType = uint64_t;

  public:
    Map() {}

    // Add an actor to the map.
    void add_actor(int x, int y, Actor *actor) {
#ifdef DEBUG
        // if (cells[to_key(x, y)].get_actors().size() > 1000) {
        //     std::cout << "Warning: too many actors at (" << x << ", " << y << ") "
        //               << cells[to_key(x, y)].get_actors().size() << std::endl;
        // }
#endif
        cells[to_key(x, y)].add_actor(actor);
    }

    // Remove an actor from the map.
    void remove_actor(int x, int y, Actor *actor) { cells[to_key(x, y)].remove_actor(actor); }

    // Get the cell at a position.
    const MapCell &get_cell(int x, int y) const {
        auto it = cells.find(to_key(x, y));
        return it == cells.end() ? empty_cell : it->second;
    }

    // Get the cell at a position.
    MapCell &get_cell(int x, int y) {
        auto it = cells.find(to_key(x, y));
        return it == cells.end() ? empty_cell : it->second;
    }

    // Reset the map.
    void reset() { cells.clear(); }

    // Print the map for debugging.
    void print() const;

  private:
    static MapCell empty_cell; // An empty cell.

    // Convert x, y to a key.
    // The key is used to index the cells.
    // Combine x and y (two 32-bit signed integers) into a 64-bit unsigned integer.
    KeyType to_key(int x, int y) const { return ((uint64_t)((uint32_t)x) << 32) | ((uint32_t)y); }

  private:
    std::unordered_map<KeyType, MapCell> cells;
};

class RangeMap {
  public:
#ifdef DEBUG
    int dbg_cnt_moved = 0;
    int dbg_cnt_not_moved = 0;
#endif

    RangeMap() = default;
    RangeMap(float cell_width, float cell_height) : cell_width(cell_width), cell_height(cell_height) {}

    // Add an actor to the map.
    void add_actor(float x, float y, Actor *actor, Collider &collider) {
        if (collider.empty()) {
            return;
        }

        int x1 = static_cast<int>((x - collider.half_w) / cell_width);
        int y1 = static_cast<int>((y - collider.half_h) / cell_height);
        int x2 = static_cast<int>((x + collider.half_w) / cell_width);
        int y2 = static_cast<int>((y + collider.half_h) / cell_height);

        for (int x = x1; x <= x2; x++) {
            for (int y = y1; y <= y2; y++) {
                map.add_actor(x, y, actor);
            }
        }
    }

    // Update the actor's position in the map.
    void update_actor(float old_x, float old_y, float new_x, float new_y, Actor *actor, Collider &collider) {
        if (collider.empty()) {
            return;
        }

        int old_x1 = static_cast<int>((old_x - collider.half_w) / cell_width);
        int old_y1 = static_cast<int>((old_y - collider.half_h) / cell_height);
        int old_x2 = static_cast<int>((old_x + collider.half_w) / cell_width);
        int old_y2 = static_cast<int>((old_y + collider.half_h) / cell_height);

        int new_x1 = static_cast<int>((new_x - collider.half_w) / cell_width);
        int new_y1 = static_cast<int>((new_y - collider.half_h) / cell_height);
        int new_x2 = static_cast<int>((new_x + collider.half_w) / cell_width);
        int new_y2 = static_cast<int>((new_y + collider.half_h) / cell_height);

        if (old_x1 != new_x1 || old_y1 != new_y1 || old_x2 != new_x2 || old_y2 != new_y2) {
            // Remove the actor from the old cells.
            for (int x = old_x1; x <= old_x2; x++) {
                for (int y = old_y1; y <= old_y2; y++) {
                    map.remove_actor(x, y, actor);
                }
            }

            // Add the actor to the new cells.
            for (int x = new_x1; x <= new_x2; x++) {
                for (int y = new_y1; y <= new_y2; y++) {
                    map.add_actor(x, y, actor);
                }
            }
#ifdef DEBUG
            dbg_cnt_moved++;
#endif
        }

#ifdef DEBUG
        else {
            dbg_cnt_not_moved++;
        }
#endif
    }

    // Get all the cells that the collider occupies.
    std::vector<std::reference_wrapper<const MapCell>> get_cells(float x, float y, const Collider &collider) const {
        if (collider.empty()) {
            return {};
        }

        std::vector<std::reference_wrapper<const MapCell>> result;
        int x1 = static_cast<int>((x - collider.half_w) / cell_width);
        int y1 = static_cast<int>((y - collider.half_h) / cell_height);
        int x2 = static_cast<int>((x + collider.half_w) / cell_width);
        int y2 = static_cast<int>((y + collider.half_h) / cell_height);
        result.reserve((x2 - x1 + 1) * (y2 - y1 + 1));
        for (int x = x1; x <= x2; x++) {
            for (int y = y1; y <= y2; y++) {
                result.emplace_back(map.get_cell(x, y));
            }
        }
        return result;
    }

    // Add an actor to the map. Use the actor's current position as the position.
    inline void add_actor(Actor *actor, Collider &collider) {
        return add_actor(actor->transform_position_x, actor->transform_position_y, actor, collider);
    }

    // Update the actor in the map. Use the actor's current position as the new position.
    inline void update_actor(float old_x, float old_y, Actor *actor, Collider &collider) {
        return update_actor(old_x, old_y, actor->transform_position_x, actor->transform_position_y, actor, collider);
    }

    // Get all the cells that the collider occupies.
    inline std::vector<std::reference_wrapper<const MapCell>> get_cells(const Actor &actor,
                                                                        const Collider &collider) const {
        return get_cells(actor.transform_position_x, actor.transform_position_y, collider);
    }

    // Reset the map.
    void reset() { map.reset(); }

    // Print the map for debugging.
    void print() const { map.print(); }

  private:
    Map map;
    float cell_width = 1.0;
    float cell_height = 1.0;
};

#endif // __MAP_H__
