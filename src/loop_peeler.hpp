/**
 * Copyright 2016 Andreas Schäfer
 *
 * Distributed under the Boost Software License, Version 1.0. (See accompanying
 * file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef FLAT_ARRAY_LOOP_PEELER_HPP
#define FLAT_ARRAY_LOOP_PEELER_HPP

#include <libflatarray/config.h>
#include <libflatarray/detail/sibling_short_vec_switch.hpp>

/**
 * This is a shim to ease handling of unaligned or not vectorizable
 * iterations at the begin/end of loops. It will invoke FUNCTION with
 * a suitable variant of SHORT_VEC (with its arity adjusted) to that
 * the main chunk of the iterations will be running with full
 * vectorization (as given by SHORT_VEC) and only the initial
 * (possibly unaligned) and trailing (less than SHORT_VEC's arity)
 * iterations will be done with an arity of 1 (i.e. scalar).
 *
 * X is expected to be increased by FUNCTION (e.g. by passing it via
 * reference).
 */
#define LIBFLATARRAY_LOOP_PEELER(SHORT_VEC_TYPE, COUNTER_TYPE,          \
                                 X, END_X, FUNCTION, ARGS...)           \
    LIBFLATARRAY_LOOP_PEELER_IMPLEMENTATION(                            \
        , SHORT_VEC_TYPE, COUNTER_TYPE, X, END_X, FUNCTION, ARGS)

/**
 * Same as LIBFLATARRAY_LOOP_PEELER(), but for use in templates
 */
#define LIBFLATARRAY_LOOP_PEELER_TEMPLATE(SHORT_VEC_TYPE, COUNTER_TYPE, \
                                          X, END_X, FUNCTION, ARGS...)  \
    LIBFLATARRAY_LOOP_PEELER_IMPLEMENTATION(                            \
        typename, SHORT_VEC_TYPE, COUNTER_TYPE, X, END_X, FUNCTION, ARGS)

#define LIBFLATARRAY_LOOP_PEELER_IMPLEMENTATION(                        \
    TYPENAME, SHORT_VEC_TYPE, COUNTER_TYPE, X, END_X, FUNCTION, ARGS...) \
    {                                                                   \
        typedef SHORT_VEC_TYPE lfa_local_short_vec;                     \
        typedef TYPENAME LibFlatArray::detail::flat_array::             \
            sibling_short_vec_switch<SHORT_VEC_TYPE, 1>::VALUE          \
            lfa_local_scalar;                                           \
                                                                        \
        COUNTER_TYPE remainder = (X) % (lfa_local_short_vec::ARITY);    \
        COUNTER_TYPE next_stop = remainder ?                            \
            (X) + (lfa_local_short_vec::ARITY) - remainder :            \
            (X);                                                        \
                                                                        \
        FUNCTION<lfa_local_scalar   >(X, next_stop, ARGS);              \
        FUNCTION<lfa_local_short_vec>(X, (END_X),   ARGS);              \
        FUNCTION<lfa_local_scalar   >(X, (END_X),   ARGS);              \
    }

#endif

#endif
