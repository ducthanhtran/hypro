/**
 * Specialization for a converter to a zonotope.
 * @file converterToZonotope.tpp
 * @author Simon Froitzheim
 *
 * @since	2015-12-17
 * @version	2016-03-17
 */

#include "Converter.h"

//conversion from Zonotope to Zonotope (no differentiation between conversion modes - always EXACT)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const Zonotope& _source, const CONV_MODE  ){
    return _source;
}
//conversion from H-Polytope to Zonotope (no differentiation between conversion modes - always OVER)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const HPolytope& _source, const CONV_MODE mode ){
    //converts source object into a v-polytope
    auto temp = toVPolytope(_source, mode);

    //conversion is from here done just like V -> Zonotope
    Zonotope res = toZonotope(temp, mode);

    return res;
}

//conversion from Box to Zonotope (no differentiation between conversion modes - always EXACT)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const Box& _source, const CONV_MODE  ){
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


//conversion from V-Polytope to Zonotope (no differentiation between conversion modes - always OVER)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const VPolytope& _source, const CONV_MODE mode ){
    //overapproximation
    //if( mode == OVER){
        //gets dimension from source object
        unsigned dim = _source.dimension();

        //gets vertices from source object
        typename VPolytopeT<Number,Converter>::pointVector vertices = _source.vertices();

        //computes an oriented Box as special Zonotope around the source object (returns Halfspaces)
        PrincipalComponentAnalysis<Number> pca(vertices);
        std::vector<Halfspace<Number>> planes = pca.box();
        HPolytope hpoly = HPolytope(planes);

        //converts computed box H -> V
        auto vpoly = toVPolytope(hpoly, mode);

        //gets vertices of box
        typename VPolytopeT<Number,Converter>::pointVector newVertices = vpoly.vertices();

        //defines empty generator matrix and center vector
        matrix_t<Number> generators = matrix_t<Number>::Zero(dim, dim);
        vector_t<Number> center = vector_t<Number>::Zero(dim);

        //computes the centroid of the Zonotope (arithmetic mean)
        center = computeArithmeticMeanPoint(newVertices);

        //defines empty distances vector for the generators
		vector_t<Number> distances = vector_t<Number>::Zero(dim);

		//defines a matrix with one exemplary point out of each set of halfspaces (on the border) for distance computation
		matrix_t<Number> planePoints = matrix_t<Number>::Zero(dim, dim);
		//for every dimension
		for (unsigned i=0; i < dim; ++i){
			//read out normal of current halfspace pair
			vector_t<Number> normal = planes[2*i].normal();

			//only continue if normal is non-zero
			assert (normal != vector_t<Number>::Zero(normal.rows()));
			//for every dimension
			for (unsigned j=0; j < dim; ++j){
				//construct point on the Halfspace by computing the intersection point with one of the axes (zero check ensures that plane is not parallel to that axis)
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
			normalDiff = carl::ceil(Number(normalDiff * Number(fReach_DENOMINATOR)))/ Number(fReach_DENOMINATOR);
			Number euclid = norm(normal, false);

			//eliminates some fractional digits for improved computation time
			Number euclid1 = euclid* (Number) fReach_DENOMINATOR;
			Number euclid2 = (Number) fReach_DENOMINATOR;
			Number euclid3 = carl::ceil(euclid1);
			euclid  = euclid3/euclid2;
			//euclid = carl::ceil(euclid*fReach_DENOMINATOR)/ (Number) fReach_DENOMINATOR;

			assert (euclid > 0);
			if (normalDiff < 0){
				distances(i) = -1*(normalDiff)/euclid;
			} else {
				distances(i) = normalDiff/euclid;
			}
		}

		for (unsigned i=0; i < dim; ++i){
			//computes scaling factors for normals in order to compute the generators later on
			vector_t<Number> normal = planes[2*i].normal();
			Number distancePow = distances(i)*distances(i);
			Number normalPow = normal.dot(normal);
			Number powDiv = distancePow/normalPow;
			//eliminates some fractional digits for improved computation time
			powDiv = carl::ceil(Number(powDiv * Number(fReach_DENOMINATOR)))/ Number(fReach_DENOMINATOR);
			std::pair<Number, Number> scaling = carl::sqrt_safe(powDiv);
			//computes generators
			generators.col(i) = scaling.second*normal;
		}

		return Zonotope(center, generators);

    //}
}


//conversion from Support Function to Zonotope (OVER or ALTERNATIVE)
//ALTERNATIVE computes a set of boundary points which then go to pca for an oriented rectangular hull before checking whether the source object is really in that box and maybe expanding it)
template <typename Number>
typename Converter<Number>::Zonotope Converter<Number>::toZonotope( const SupportFunction& _source, const CONV_MODE mode, unsigned numberOfDirections){
    Zonotope res;
    if (mode == OVER) {
         //gets dimension of source object
        unsigned dim = _source.dimension();

         //computes a vector of template directions based on the dimension and the requested number of directions which should get evaluated
        std::vector<vector_t<Number>> templateDirections = computeTemplate<Number>(dim, numberOfDirections);
         //only continue if size of the vector is not greater than the upper bound for maximum evaluations (uniformly distributed directions for higher dimensions yield many necessary evaluations)
         assert (templateDirections.size() <= std::pow(numberOfDirections, dim));
         //creates a matrix with one row for each direction and one column for each dimension
        matrix_t<Number> templateDirectionMatrix = matrix_t<Number>(templateDirections.size(), dim);

         //fills the matrix with the template directions
        for (unsigned i=0; i<templateDirections.size();++i){
            templateDirectionMatrix.row(i) = templateDirections[i];
         }

        //lets the support function evaluate the offset of the halfspaces for each direction
        std::vector<EvaluationResult<Number>> offsets = _source.multiEvaluate(templateDirectionMatrix);
        assert(offsets.size() == std::size_t(templateDirectionMatrix.rows()));
        std::vector<std::size_t> boundedConstraints;
         for(unsigned offsetIndex = 0; offsetIndex < offsets.size(); ++offsetIndex){
                 if(offsets[offsetIndex].errorCode != SOLUTION::INFTY)
                        boundedConstraints.push_back(offsetIndex);
        }
        matrix_t<Number> constraints = matrix_t<Number>(boundedConstraints.size(), dim);
        vector_t<Number> constants = vector_t<Number>(boundedConstraints.size());
        std::size_t pos = boundedConstraints.size()-1;
        while(!boundedConstraints.empty()){
                constraints.row(pos) = templateDirectionMatrix.row(boundedConstraints.back());
                constants(pos) = offsets[boundedConstraints.back()].supportValue;
                boundedConstraints.pop_back();
                --pos;
        }

        //constructs a H-Polytope out of the computed halfspaces
         HPolytope samplePoly = HPolytope(constraints, constants);

        //converts H-Polytope into a V-Polytope
        auto sampleVPoly = toVPolytope(samplePoly, mode);

        //conversion is from here done just like V -> Zonotope
        res = toZonotope(sampleVPoly, mode);
    }

    if (mode == ALTERNATIVE){
        //gets dimension of source object
        unsigned dim = _source.dimension();

        //computes a vector of template directions based on the dimension and the requested number of directions which should get evaluated
        std::vector<vector_t<Number>> templateDirections = computeTemplate<Number>(dim, numberOfDirections);
        //only continue if size of the vector is not greater than the upper bound for maximum evaluations (uniformly distributed directions for higher dimensions yield many necessary evaluations)
        assert (templateDirections.size() <= std::pow(numberOfDirections, dim));
        //creates a matrix with one row for each direction and one column for each dimension
        matrix_t<Number> templateDirectionMatrix = matrix_t<Number>(templateDirections.size(), dim);

        //fills the matrix with the template directions
        for (unsigned i=0; i<templateDirections.size();++i){
               templateDirectionMatrix.row(i) = templateDirections[i];
        }

        //lets the support function evaluate the offset of the halfspaces for each direction
        std::vector<EvaluationResult<Number>> offsets = _source.multiEvaluate(templateDirectionMatrix);
        assert(offsets.size() == std::size_t(templateDirectionMatrix.rows()));
        std::vector<std::size_t> boundedConstraints;
        for(unsigned offsetIndex = 0; offsetIndex < offsets.size(); ++offsetIndex){
              if(offsets[offsetIndex].errorCode != SOLUTION::INFTY)
                       boundedConstraints.push_back(offsetIndex);
        }

        //builds a pointVector from boundary points with the evaluation results (uses only results that are not infinity (i.e. where a bound exists))
        std::vector<Point<Number>> points = std::vector<Point<Number>>(boundedConstraints.size());
        std::size_t pos = boundedConstraints.size()-1;
        while(!boundedConstraints.empty()){
              points[pos] = offsets[boundedConstraints.back()].optimumValue;
              boundedConstraints.pop_back();
              --pos;
        }

        //computes an oriented Box as special Zonotope around the boundary points(returns Halfspaces and may not be an overapproximation yet)
        PrincipalComponentAnalysis<Number> pca(points);
        std::vector<Halfspace<Number>> planes = pca.box();

        //gets number of planes
        unsigned numberOfPlanes = planes.size();

        //defines an empty vector for the oriented box normals
        matrix_t<Number> normals = matrix_t<Number>(numberOfPlanes, dim);

        //defines an empty vector for the oriented box offsets
        vector_t<Number> newOffsets = vector_t<Number>(numberOfPlanes);

        //reads normals and offsets out
        for (unsigned i=0; i< numberOfPlanes; ++i){
            normals.row(i) = planes[i].normal();
            newOffsets(i) = planes[i].offset();
        }

        //defines an empty vector for the source object offsets in ORH normal directions)
        vector_t<Number> oldOffsets = vector_t<Number>(numberOfPlanes);

        //evaluates source object in these 2*d directions to secure an overapproximation
        //lets the support function evaluate the offset of the halfspaces for each direction
        std::vector<EvaluationResult<Number>> sourceOffsets = _source.multiEvaluate(normals);
        assert(sourceOffsets.size() == std::size_t(normals.rows()));
        for(unsigned offsetIndex = 0; offsetIndex < sourceOffsets.size(); ++offsetIndex){
              oldOffsets(offsetIndex) = sourceOffsets[offsetIndex].supportValue;
              if(sourceOffsets[offsetIndex].errorCode == SOLUTION::INFTY)
                  //source object is unbounded
                  //TODO dealing with the unbounded case (maybe introducing unbounded zonotopes?)
                  assert(false);
        }

        //takes the maximum out of the two computed offsets to ensure an overapproximation
        for (unsigned i=0; i<numberOfPlanes; ++i){
            Number maxValue = newOffsets(i) > oldOffsets(i) ? newOffsets(i) : oldOffsets(i);
            planes[i].setOffset(maxValue);
        }

        //overapproximated hpolytope (now for certain)
        HPolytope hpoly = HPolytope(planes);

        //converts computed box H -> V
        auto vpoly = toVPolytope(hpoly, mode);
        //gets vertices of box
        typename VPolytopeT<Number,Converter>::pointVector newVertices = vpoly.vertices();

        //defines empty generator matrix and center vector
        matrix_t<Number> generators = matrix_t<Number>::Zero(dim, dim);
        vector_t<Number> center = vector_t<Number>::Zero(dim);

        //computes the centroid of the Zonotope (arithmetic mean)
        center = computeArithmeticMeanPoint(newVertices);

        //defines empty distances vector for the generators
         vector_t<Number> distances = vector_t<Number>::Zero(dim);

         //defines a matrix with one exemplary point out of each set of halfspaces (on the border) for distance computation
         matrix_t<Number> planePoints = matrix_t<Number>::Zero(dim, dim);
         //for every dimension
         for (unsigned i=0; i < dim; ++i){
             //read out normal of current halfspace pair
             vector_t<Number> normal = planes[2*i].normal();


             //only continue if normal is non-zero
             assert (normal != vector_t<Number>::Zero(normal.rows()));
             //for every dimension
             for (unsigned j=0; j < dim; ++j){
                 //construct point on the Halfspace by computing the intersection point with one of the axes (zero check ensures that plane is not parallel to that axis)
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
             normalDiff = carl::ceil(Number(normalDiff * Number(fReach_DENOMINATOR)))/ Number(fReach_DENOMINATOR);
             Number euclid = norm(normal, false);

             //eliminates some fractional digits for improved computation time
             Number euclid1 = euclid* (Number) fReach_DENOMINATOR;
             Number euclid2 = (Number) fReach_DENOMINATOR;
             Number euclid3 = carl::ceil(euclid1);
             euclid  = euclid3/euclid2;
             //euclid = carl::ceil(euclid*fReach_DENOMINATOR)/ (Number) fReach_DENOMINATOR;

             assert (euclid > 0);
             if (normalDiff < 0){
                 distances(i) = -1*(normalDiff)/euclid;
             } else {
                 distances(i) = normalDiff/euclid;
             }
         }

         for (unsigned i=0; i < dim; ++i){
             //computes scaling factors for normals in order to compute the generators later on
             vector_t<Number> normal = planes[2*i].normal();
             Number distancePow = distances(i)*distances(i);
             Number normalPow = normal.dot(normal);
             Number powDiv = distancePow/normalPow;
             //eliminates some fractional digits for improved computation time
             powDiv = carl::ceil(Number(powDiv * Number(fReach_DENOMINATOR)))/ Number(fReach_DENOMINATOR);
             std::pair<Number, Number> scaling = carl::sqrt_safe(powDiv);
             //computes generators
             generators.col(i) = scaling.second*normal;
         }

         res = Zonotope(center, generators);
    }
    return res;

}

