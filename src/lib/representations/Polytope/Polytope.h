/* 
 * File:   Polytope.h
 * Author: Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * 
 * @since       2014-03-20
 * @version     2014-04-04
 */

#pragma once

#include <map>
#include <cassert>
#include <string>
#include <sstream>
#include <gmp.h>
#include <ppl.hh>
#include <carl/core/Variable.h>

#include "util.h"
#include "../GeometricObject.h"
#include "../../datastructures/Point.h"

#include "../Polytopes/util.h"


using namespace Parma_Polyhedra_Library;

namespace hypro
{
    template<typename Number>
    class Polytope : hypro::GeometricObject<Number>
    {
    private:
        C_Polyhedron mPolyhedron;
        std::vector<Point<Number>> mPoints;

    public:
        Polytope();
        Polytope(const Polytope& orig);
        Polytope(unsigned dimension);
        Polytope(const carl::Variable& _var, double _val);
        Polytope(const Point<Number>& point);
        Polytope(const typename std::vector<Point<Number>>& points);
        Polytope(const typename std::vector<Eigen::Matrix<carl::FLOAT_T<Number>,Eigen::Dynamic,1>>& points);
        Polytope(const matrix& A, const vector& b);
        Polytope(const matrix& A);
        Polytope(const C_Polyhedron& _rawPoly);
        
        virtual ~Polytope();
        
        /**
         * Getters and Setters
         */
        
        /**
         * Returns true, if the generatorset is empty.
         * @return 
         */
        bool isEmpty() const;
        
        /**
         * Adds a point to the polytope. Note that if the point lies inside the
         * current polytope, this has no effect.
         * @param point
         */
        void addPoint(const Point<Number>& point);
        
        /**
         * Returns the set of points which form the polytope.
         * @return Pointset.
         */
        const std::vector<Point<Number>>& points() const;
        
        /**
         * Prints the polytopes' generators obtained from the PPL to stdout.
         */
        void print() const;
        
        /**
         * Writes the polytope to a file in a matlab format.
         * @param _filename The filename of the output file.
         */
        void writeToFile(std::string _filename) const;
        
        friend std::ostream& operator<<(std::ostream& lhs, const Polytope<Number>& rhs)
        {
            using namespace Parma_Polyhedra_Library::IO_Operators;
            lhs << "[";
            for(auto& generator : rhs.rawPolyhedron().generators())
            {
                lhs << generator;
            }
            lhs << "]" << std::endl;
            return lhs;
        }
        
        /**
         * PPL related functions
         */
        const C_Polyhedron& rawPolyhedron() const;
        C_Polyhedron& rRawPolyhedron();
        
        /**
         * Geometric Object interface
         */
        unsigned int dimension() const;
        bool linearTransformation(Polytope<Number>& result, const matrix& A, const vector& b = vector()) const;
        
        bool minkowskiSum(Polytope<Number>& result, const Polytope<Number>& rhs) const;
        // implemented according to Komei Fukuda 2004
        bool altMinkowskiSum(Polytope<Number>& result, const Polytope<Number>& rhs) const;
        bool intersect(Polytope<Number>& result, const Polytope<Number>& rhs);
        bool hull(Polytope<Number>& result);
        bool contains(const Point<Number>& point);
        bool contains(const Polytope<Number>& poly);
        bool unite(Polytope<Number>& result, const Polytope<Number>& rhs);
        
        /**
         * Polytope related
         */
        Number hausdorffError(const Number& delta, const Eigen::Matrix<carl::FLOAT_T<Number>, Eigen::Dynamic, Eigen::Dynamic>& matrix);
        
        
        //Polytope<Number>& operator= (const Polytope<Number>& rhs);

        /**
         * Auxiliary functions
         */
        int computeMaxVDegree();
        Point<Number> computeMaxPoint();
        Point<Number> computeInitVertex(Polytope<Number> _secondPoly);
        Point<Number> localSearch(Point<Number> _vertex);

    };
} // namespace

#include "Polytope.tpp"
