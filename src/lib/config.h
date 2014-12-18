#pragma once

#include <set>
#include <map>

#include "types.h"
#include <carl/core/MultivariatePolynomial.h>
#include <carl/numbers/numbers.h>
#include <carl/core/Variable.h>
#include <carl/core/carlLoggingHelper.h>
#include <carl/util/SFINAE.h>
//#include "util/eigenTypetraits.h"
#include <eigen3/Eigen/Dense>
#include <eigen3/unsupported/Eigen/src/MatrixFunctions/MatrixExponential.h>
#include "util/VariablePool.h"

// needed for vector outstream operator:
using carl::operator<<;

#define reachLin_VERSION_MAJOR 0
#define reachLin_VERSION_MINOR 1

#define DEBUG 1

#define DEFAULT_DIM 5
#define MAXIMAL_DIMENSION 5

// Do we eliminate dimensions according to a dynamic heuristic.
#define DYNAMICHEURISTIC 0
#define reachLin_PRIMITIVES_LOCATION "../polyhedra/"
#define reachLin_NR_PRIMITIVES 100

#define PI_UP 3.141592655
#define PI_DN 3.141592654


#ifdef SUPPORT_MPFR
#include <mpfr.h>
typedef mpfr_t number_t;
#else
typedef double number_t;
#endif

typedef carl::FLOAT_T<number_t> number;
typedef carl::MultivariatePolynomial<number> Polynomial;
typedef std::map<carl::Variable, unsigned> varIdMap;
typedef std::set<carl::Variable> variableSet;
//typedef Eigen::Matrix<number, Eigen::Dynamic, Eigen::Dynamic> matrix;
//typedef Eigen::Matrix<number, Eigen::Dynamic, 1> vector;

namespace hypro
{
template<typename Number>
using vector_t = Eigen::Matrix<Number, Eigen::Dynamic, 1>;

template<typename Number>
using matrix_t = Eigen::Matrix<Number, Eigen::Dynamic, Eigen::Dynamic>;

template<typename Number>
using scalar_t = Number;
}
/**
 * author: ckugler
 * Defines for reachability algorithm based on polytopes
 */
#define fReach_TIMEBOUND 10
#define fReach_TIMEDISCRETIZATION 10
#define fReach_DENOMINATOR 1000000
//define for debugging: triggers console output
//#define fReach_DEBUG

/**
 * author: ckugler
 * Defines for implementation of Fukuda's Minkowski Sum algorithm
 */
#define EPSILON 0.000001
#define POS_CONSTANT 100
//define for debugging: triggers console output
//#define fukuda_DEBUG

namespace Eigen
{
    template<> struct NumTraits<number>
    {
        enum
        {
            IsComplex = 0,
            IsInteger = 0,
            ReadCost = 1,
            AddCost = 1,
            MulCost = 1, 
            IsSigned = 1,
            RequireInitialization = 1
        };

        typedef number Real;
        typedef number NonInteger;
        typedef number Nested;

        static inline Real epsilon() { return Real(0); }
        static inline Real dummy_precision()
        {
            // make sure to override this for floating-point types
            return Real(0);
        }
        //static inline number highest() { return number::maxVal(); }
        //static inline number lowest()  { return number::minVal(); }
    };
}
