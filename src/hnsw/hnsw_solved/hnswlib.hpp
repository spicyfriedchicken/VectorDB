#pragma once
#include <queue>
#include <vector>
#include <iostream>
#include <string.h>

// This can be extended to store state for filtering (e.g. from a std::set)
class BaseFilterFunctor {
 public:
    virtual bool operator()(size_t id) { return true; }
    virtual ~BaseFilterFunctor() {};
};

template<typename dist_t>
class BaseSearchStopCondition {
 public:
    virtual void add_point_to_result(size_t label, const void *datapoint, dist_t dist) = 0;

    virtual void remove_point_from_result(size_t label, const void *datapoint, dist_t dist) = 0;

    virtual bool should_stop_search(dist_t candidate_dist, dist_t lowerBound) = 0;

    virtual bool should_consider_candidate(dist_t candidate_dist, dist_t lowerBound) = 0;

    virtual bool should_remove_extra() = 0;

    virtual void filter_results(std::vector<std::pair<dist_t, size_t >> &candidates) = 0;

    virtual ~BaseSearchStopCondition() {}
};

template <typename T>
class pairGreater {
 public:
    bool operator()(const T& p1, const T& p2) {
        return p1.first > p2.first;
    }
};

template<typename T>
static void writeBinaryPOD(std::ostream &out, const T &podRef) {
    out.write((char *) &podRef, sizeof(T));
}

template<typename T>
static void readBinaryPOD(std::istream &in, T &podRef) {
    in.read((char *) &podRef, sizeof(T));
}

template<typename dist_t>
using DISTFUNC = dist_t(*)(const void *, const void *, const void *);

template<typename dist_t>
class SpaceInterface {
 public:
    // virtual void search(void *);
    virtual size_t get_data_size() = 0;

    virtual DISTFUNC<dist_t> get_dist_func() = 0;

    virtual void *get_distance_function_parameters_ram() = 0;

    virtual ~SpaceInterface() {}
};

template<typename dist_t>
class AlgorithmInterface {
 public:
    virtual void addPoint(const void *datapoint, size_t label, bool replace_deleted = false) = 0;

    virtual std::priority_queue<std::pair<dist_t, size_t>>
        SearchKNN(const void*, size_t, BaseFilterFunctor* filter = nullptr) const = 0;

    // Return k nearest neighbor in the order of closer fist
    virtual std::vector<std::pair<dist_t, size_t>>
        SearchKNNCloserFirst(const void* query, size_t k, BaseFilterFunctor* filter = nullptr) const;

    virtual void saveIndex(const std::string &location) = 0;
    virtual ~AlgorithmInterface(){
    }
};

template<typename dist_t>
std::vector<std::pair<dist_t, size_t>>
AlgorithmInterface<dist_t>::SearchKNNCloserFirst(const void* query, size_t k,
                                                 BaseFilterFunctor* filter) const {
    std::vector<std::pair<dist_t, size_t>> result;

    // here SearchKNN returns the result in the order of further first
    auto ret = SearchKNN(query, k, filter);
    {
        size_t sz = ret.size();
        result.resize(sz);
        while (!ret.empty()) {
            result[--sz] = ret.top();
            ret.pop();
        }
    }

    return result;
}

#include "space_l2.hpp"
#include "space_ip.hpp"
#include "stop_condition.hpp"
#include "bruteforce.hpp"
#include "hnswalg.hpp"
