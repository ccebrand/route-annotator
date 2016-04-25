#pragma once

#include "types.hpp"
#include <boost/geometry/index/rtree.hpp>

#include <sparsehash/sparse_hash_map>
#include <stxxl/unordered_map>

/**
 * The in-memory database holds all the useful data in memory.
 * Data is added here by the Extractor, then used by
 * the RouteAnnotator.
 */
struct Database
{
  public:
    /**
     * A map of internal node id pairs to the way they belong to
     * TODO: support multiple ways???
     */
    google::sparse_hash_map<internal_nodepair_t, wayid_t> pair_way_map;

    /**
     * Stores the start/end indexes for the tags for a way.  Values
     * here refer to the key_value_pairs vector.
     */
    std::vector<tagrange_t> way_tag_ranges;

    /**
     * holds the key and value indexes for a tag.  The values in
     * way_tag_ranges refer to this vector.
     */
    std::vector<keyvalue_index_t> key_value_pairs;

    /**
     * The RTree we use to find internal nodes using coordinates.
     */
    std::unique_ptr<boost::geometry::index::rtree<value_t, boost::geometry::index::rstar<8>>> rtree;

    /**
     * The map of external (OSM 64 bit) node ids to our internal
     * node ID values (32 bit, to save space)
     */
    external_internal_map_t external_internal_map;

    /**
     * Adds a string to our character buffer
     *
     * @param str a C-style null-terminated string
     * @return the ID of the string so it can be found later
     */
    stringid_t addstring(const char *str);

    /**
     * Gets a string from the character buffer
     *
     * @param stringid the id of the desired string, as returned by
     *   addstring earlier
     * @return the requested string
     */
    std::string getstring(const stringid_t stringid) const;

    /**
     * Builds the RTree and reclaims memory by discarding temporary data
     * and shrinking vectors that have auto-grown.  Needs to be called after
     * all OSM data parsing has been added.
     */
    void compact();

    /**
     * Dumps some info about what's in our database
     */
    void dump() const;

  private:
    // The character data for all strings
    std::vector<char> string_data;
    // The start/end positions of each string in the string_data buffer
    std::vector<stringoffset_t> string_offsets;
    // A temporary lookup table so that we can re-use strings
    //std::unordered_map<std::string, std::uint32_t> string_index;
    struct HashFunctor2
    { std::size_t operator()(const std::string &x) const { return std::hash<std::string>()(x); } };
    struct CompareLess2
    { bool operator()(const std::string &a, const std::string &b) const { return a < b; } };
    //stxxl::unordered_map<std::string, std::uint32_t, HashFunctor2, CompareLess2, 4096, 4096> string_index;

    google::sparse_hash_map<std::string, std::uint32_t> string_index;
};
