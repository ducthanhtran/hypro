/**
 * Specialization for a converter to a zonotope.
 * @file converterToZonotope.tpp
 * @author Simon Froitzheim
 *
 * @since	2015-12-17
 * @version	2015-12-17
 */

#include "Converter.h"
#include "../../util/Plotter.h"

//conversion from Zonotope to Zonotope (no differentiation between conversion modes - always EXACT)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const Zonotope& _source, const CONV_MODE mode ){
    return _source;
}
//TODO conversion from HPolytope to Zonotope (no differentiation between conversion modes - always OVER)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const HPolytope& _source, const CONV_MODE mode ){
    return Zonotope();
}

//conversion from Box to Zonotope (no differentiation between conversion modes - always EXACT)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const Box& _source, const CONV_MODE mode ){
    unsigned dim = _source.dimension();                                             //gets dimension from source object
    std::vector<carl::Interval<Number>> intervals = _source.boundaries();           //gets intervals from source object
    matrix_t<Number> generators = matrix_t<Number>::Zero(dim, dim);                 //defines an empty generator matrix for dim generators
    vector_t<Number> center = vector_t<Number>(dim);                                //defines an empty center vector

    for (unsigned i=0; i<dim; ++i){                                                 //for every dimension
        center[i] = (intervals[i].lower() + intervals[i].upper())/2;                //compute center component of the Box
    }

    for (unsigned i=0; i<dim; ++i){                                                 //for every dimension
        generators(i, i) = (intervals[i].upper() - intervals[i].lower())/ 2;        //compute the corresponding generator for that dimension by computing the length of the interval and dividing it by 2
    }

    return Zonotope(center, generators);
}

//TODO exact conversion (maybe)
//TODO doesn't work properly
//conversion from V-Polytope to Zonotope (no differentiation between conversion modes - always OVER)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const VPolytope& _source, const CONV_MODE mode ){
    //overapproximation
    //if( mode == OVER){
        //gets dimension from source object
        unsigned dim = _source.dimension();
        
        //gets vertices from source object
        typename VPolytopeT<Number,Converter>::pointVector vertices = _source.vertices();
        
        //computes an oriented Box as special Zonotope around the source object (returns hyperplanes)
        std::vector<Hyperplane<Number>> planes = computeOrientedBox(vertices);
        HPolytope hpoly = HPolytope(planes);
        
        //TODO remove later
        Plotter<Number>& plotter = Plotter<Number>::getInstance();

        plotter.setObjectColor(plotter.addObject(hpoly.vertices()), colors[red]);
        
        //converts computed box H -> V
        auto vpoly = Converter<Number>::toVPolytope(hpoly);
        //gets vertices of box
        typename VPolytopeT<Number,Converter>::pointVector newVertices = vpoly.vertices();
        
        //defines empty generator matrix and center vector
        matrix_t<Number> generators = matrix_t<Number>::Zero(dim, dim);
        vector_t<Number> center = vector_t<Number>::Zero(dim);
        
        //gets number of vertices
        unsigned size = newVertices.size();
        //only continue if object is really an oriented box (i.e. it has 2^n vertices)
        assert (size == std::pow(2 , dim));
        
        //computes the centroid of the Zonotope (arithmetic mean)
          for (unsigned i=0; i < size; ++i){
              center += newVertices[i].rawCoordinates();
         }  
         center = center*( ((Number) 1)/size);
        
        //defines empty distances vector for the generators
         vector_t<Number> distances = vector_t<Number>::Zero(dim);
         
         //defines a matrix with one exemplary point out of each set of halfspaces (on the border) for distance computation
         matrix_t<Number> planePoints = matrix_t<Number>::Zero(dim, dim);
         //for every dimension       
         for (unsigned i=0; i < dim; ++i){
             //read out normal of current halfspace pair
             vector_t<Number> normal = planes[2*i].normal();
             
             std::cout << "Normal: " << convertVecToDouble(normal) << std::endl;
             //only continue if normal is non-zero
             assert (normal != vector_t<Number>::Zero(normal.rows()));
             //for every dimension
             for (unsigned j=0; j < dim; ++j){
                 //construct point on the hyperplane by computing the intersection point with one of the axes (zero check ensures that plane is not parallel to that axis)
                 if (normal(j) != 0){
                     vector_t<Number> p = vector_t<Number>::Zero(dim);
                     p(j) = 1;
                     planePoints.row(i) = p*(planes[2*i].offset()/normal(j));
                     break;
                 }  
             }
             
         }
         //computes distances from center to each pair of halfspaces        
         
         for (unsigned i=0; i < dim; ++i){
             vector_t<Number> normal = planes[2*i].normal();
             
             
             
             Number normalDiff = normal.dot(center) - normal.dot(planePoints.row(i));
             //eliminates some fractional digits for improved computation time 
             normalDiff = carl::ceil(normalDiff*fReach_DENOMINATOR)/ (Number) fReach_DENOMINATOR;
             Number euclid = norm(normal, false);
             
             std::cout << "NormalLength: " << carl::toDouble(euclid) <<  std::endl;
             //eliminates some fractional digits for improved computation time 
             euclid = carl::ceil(euclid*fReach_DENOMINATOR)/ (Number) fReach_DENOMINATOR;
             
             assert (euclid > 0);
             if (normalDiff < 0){
                 distances(i) = -1*(normalDiff)/euclid;
             } else {
                 distances(i) = normalDiff/euclid;
             }
         }
         
         std::cout << "Distances:" << convertVecToDouble(distances) << std::endl;
         
         //computes scaling factors for normals in order to compute the generators later on
         std::pair<Number, Number> scaling = std::pair<Number, Number>();
         for (unsigned i=0; i < dim; ++i){
             vector_t<Number> normal = planes[2*i].normal();
             Number distancePow = distances(i)*distances(i);
             Number normalPow = normal.dot(normal);
             Number powDiv = distancePow/normalPow;
             //eliminates some fractional digits for improved computation time 
             powDiv = carl::ceil(powDiv*fReach_DENOMINATOR)/ (Number) fReach_DENOMINATOR;
             scaling = carl::sqrt_safe(powDiv);
             //computes generators
             generators.col(i) = scaling.second*normal;
             
             std::cout << "ScalingFirst: " << carl::toDouble(scaling.first) << "ScalingSecond:" << carl::toDouble(scaling.second) << std::endl;
         }
         
         
         
     

         
      
         return Zonotope(center, generators);
        
    //}
}

//TODO conversion from Support Function to Zonotope (no differentiation between conversion modes - always OVER)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const SupportFunction& _source, const CONV_MODE mode){
    return Zonotope();
}
