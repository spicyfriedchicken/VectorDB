#pragma once
#include "hnswlib.hpp"

// Computes the dot product of two vectors. 
static float InnerProduct(const void *pVect1, const void *pVect2, const void *qty_ptr) {
    size_t qty = *((size_t *) qty_ptr);
    float res = 0;
    for (unsigned i = 0; i < qty; i++) {
        res += ((float *) pVect1)[i] * ((float *) pVect2)[i];
    }
    return res;
}

// Turns dot product into a distance metric
static float InnerProductDistance(const void *pVect1, const void *pVect2, const void *qty_ptr) {
    return 1.0f - InnerProduct(pVect1, pVect2, qty_ptr);
}

class InnerProductSpace : public SpaceInterface<float> {
    DISTFUNC<float> distance_function_;
    size_t data_size_;
    size_t dim_;

 public:
    InnerProductSpace(size_t dim) {
        distance_function_ = InnerProductDistance;
        dim_ = dim;
        data_size_ = dim * sizeof(float);
    }

    size_t get_data_size() {
        return data_size_;
    }

    DISTFUNC<float> get_dist_func() {
        return distance_function_;
    }

    void *get_distance_function_parameters_ram() {
        return &dim_;
    }

~InnerProductSpace() {}
};

