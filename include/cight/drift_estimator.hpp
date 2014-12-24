#ifndef DEJAVU_DRIFT_ESTIMATOR_H
#define DEJAVU_DRIFT_ESTIMATOR_H

#include <cight/memory.hpp>
#include <cight/stream_matcher.hpp>
#include <cight/settings.hpp>

#include <clarus/core/list.hpp>

#include <string>

namespace cight {
    class Estimator;
}

class cight::Estimator {
    Memory teach;

    Memory replay;

    StreamMatcher matcher;

    int bins;

    int window;

public:
    /*
    Creates a new drift estimator, using the giving folder for storage.
    */
    Estimator(int bins, int window, size_t range, StreamMatcher matcher);

    /*
    Virtual destructor. Enforces polymorphism. Do not remove.
    */
    virtual ~Estimator();

    /*
    Calculates a vector of drift estimate likelihoods.
    */
    cv::Mat operator () ();

    /*
    Erases the estimator's memory, forcing it to rebuild its image base on the next
    call.
    */
    void reset();
};

#endif
