#pragma once
#include "hnswlib.hpp"

static float
L2Sqr(const void *pVect1v, const void *pVect2v, const void *qty_ptr) {
    float *pVect1 = (float *) pVect1v;
    float *pVect2 = (float *) pVect2v;
    size_t qty = *((size_t *) qty_ptr);

    float res = 0;
    for (size_t i = 0; i < qty; i++) {
        float t = *pVect1 - *pVect2;
        pVect1++;
        pVect2++;
        res += t * t;
    }
    return (res);
}

class L2Space : public SpaceInterface<float> {
    DISTFUNC<float> distance_function_;
    size_t data_size_;
    size_t dim_;

 public:
    L2Space(size_t dim) {
        distance_function_ = L2Sqr;
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

    ~L2Space() {}
};

static int
L2SqrI4x(const void *__restrict pVect1, const void *__restrict pVect2, const void *__restrict qty_ptr) {
    size_t qty = *((size_t *) qty_ptr);
    int res = 0;
    unsigned char *a = (unsigned char *) pVect1;
    unsigned char *b = (unsigned char *) pVect2;

    qty = qty >> 2;
    for (size_t i = 0; i < qty; i++) {
        res += ((*a) - (*b)) * ((*a) - (*b));
        a++;
        b++;
        res += ((*a) - (*b)) * ((*a) - (*b));
        a++;
        b++;
        res += ((*a) - (*b)) * ((*a) - (*b));
        a++;
        b++;
        res += ((*a) - (*b)) * ((*a) - (*b));
        a++;
        b++;
    }
    return (res);
}

static int L2SqrI(const void* __restrict pVect1, const void* __restrict pVect2, const void* __restrict qty_ptr) {
    size_t qty = *((size_t*)qty_ptr);
    int res = 0;
    unsigned char* a = (unsigned char*)pVect1;
    unsigned char* b = (unsigned char*)pVect2;

    for (size_t i = 0; i < qty; i++) {
        res += ((*a) - (*b)) * ((*a) - (*b));
        a++;
        b++;
    }
    return (res);
}

class L2SpaceI : public SpaceInterface<int> {
    DISTFUNC<int> distance_function_;
    size_t data_size_;
    size_t dim_;

 public:
    L2SpaceI(size_t dim) {
        if (dim % 4 == 0) {
            distance_function_ = L2SqrI4x;
        } else {
            distance_function_ = L2SqrI;
        }
        dim_ = dim;
        data_size_ = dim * sizeof(unsigned char);
    }

    size_t get_data_size() {
        return data_size_;
    }

    DISTFUNC<int> get_dist_func() {
        return distance_function_;
    }

    void *get_distance_function_parameters_ram() {
        return &dim_;
    }

    ~L2SpaceI() {}
};
