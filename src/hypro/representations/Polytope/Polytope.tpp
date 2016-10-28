/*
 * File:   Polytope.tpp
 * Author: Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 *
 * @since       2014-03-20
 * @version     2015-01-21
 */

#include "Polytope.h"

namespace hypro {
template <typename Number>
Polytope<Number>::Polytope()
	: mPointsUpToDate( true ) {
}

template <typename Number>
Polytope<Number>::Polytope( const Polytope &orig )
	: mPolyhedron( orig.mPolyhedron ), mPoints( orig.mPoints ), mPointsUpToDate( orig.mPointsUpToDate ) {
}

template <typename Number>
Polytope<Number>::~Polytope() {
}

template <typename Number>
Polytope<Number>::Polytope( unsigned dimension )
	: mPolyhedron( dimension, Parma_Polyhedra_Library::EMPTY ), mPoints(), mPointsUpToDate( true ) {
}

template <typename Number>
Polytope<Number>::Polytope( const Point<Number> &_point )
	: mPolyhedron( C_Polyhedron( polytope::pplDimension( _point ) + 1, Parma_Polyhedra_Library::EMPTY ) ) {
	// std::cout << "Try Ppint: " << _point << std::endl;
	mPolyhedron.add_generator( polytope::pointToGenerator( _point ) );
	mPoints.push_back( _point );
	mPointsUpToDate = true;
}

template <typename Number>
Polytope<Number>::Polytope( const typename std::vector<Point<Number>> &points ) {
	// mPolyhedron.initialize();
	// std::cout << "Try Ppints" << std::endl;
	mPolyhedron =
		  Parma_Polyhedra_Library::C_Polyhedron( polytope::pplDimension( points ) + 1, Parma_Polyhedra_Library::EMPTY );
	for ( auto &pointSetIt : points ) {
		mPolyhedron.add_generator( polytope::pointToGenerator( pointSetIt ) );
		mPoints.push_back( pointSetIt );
	}
	mPointsUpToDate = true;
}

template <typename Number>
Polytope<Number>::Polytope( const typename std::vector<vector_t<Number>> &points ) {
	mPolyhedron =
		  Parma_Polyhedra_Library::C_Polyhedron( polytope::pplDimension( points ), Parma_Polyhedra_Library::EMPTY );
	for ( auto pointIt = points.begin(); pointIt != points.end(); ++pointIt ) {
		Generator tmp = polytope::pointToGenerator( *pointIt );
		mPolyhedron.add_generator( tmp );
		Point<Number> tmpPoint = Point<Number>( *pointIt );
		mPoints.push_back( tmpPoint );
	}
	mPointsUpToDate = true;
}

template <typename Number>
Polytope<Number>::Polytope( const matrix_t<Number> &A, const vector_t<Number> &b ) {
	assert( A.rows() == b.rows() );
	mPolyhedron = Parma_Polyhedra_Library::C_Polyhedron( A.cols(), Parma_Polyhedra_Library::UNIVERSE );
	for ( unsigned rowIndex = 0; rowIndex < A.rows(); ++rowIndex ) {
		// std::cout << "Process row " << rowIndex << std::endl;
		Parma_Polyhedra_Library::Linear_Expression polynom;
		polynom.set_space_dimension( A.cols() );
		for ( unsigned columIndex = 0; columIndex < A.cols(); ++columIndex ) {
			// std::cout <<"(" << columIndex << ") Matrix Coefficient: " <<
			// (A(rowIndex,columIndex)*fReach_DENOMINATOR)
			// << std::endl;
			polynom.set_coefficient( hypro::VariablePool::getInstance().pplVarByIndex( columIndex ),
									 carl::toDouble( A( rowIndex, columIndex ) * fReach_DENOMINATOR ) );
			// polynom.set_coefficient(hypro::VariablePool::getInstance().pplVarByIndex(columIndex),
			// A(rowIndex,columIndex).toDouble());
			// std::cout <<
			// hypro::VariablePool::getInstance().pplVarByIndex(columIndex) << " = "
			// <<
			// A(rowIndex,columIndex).toDouble() << std::endl;
		}
		// std::cout << "Vector Coefficient: " <<
		// -(b(rowIndex,0)*fReach_DENOMINATOR) << std::endl;
		polynom.set_inhomogeneous_term( - carl::toDouble( b( rowIndex, 0 ) * fReach_DENOMINATOR ) );
		// polynom.set_inhomogeneous_term(-b(rowIndex,0).toDouble());
		Parma_Polyhedra_Library::Constraint constraint;
		constraint = polynom <= 0;

		// std::cout << "Add Polynom." << std::endl;
		mPolyhedron.add_constraint( constraint );
		// mPolyhedron.add_generator(gen);
	}
	mPointsUpToDate = false;
}

template <typename Number>
Polytope<Number>::Polytope( const matrix_t<Number> &A ) {
	mPolyhedron = Parma_Polyhedra_Library::C_Polyhedron( A.rows(), Parma_Polyhedra_Library::UNIVERSE );
	for ( unsigned rowIndex = 0; rowIndex < A.rows(); ++rowIndex ) {
		Parma_Polyhedra_Library::Linear_Expression polynom;
		polynom.set_space_dimension( A.cols() );
		for ( unsigned columIndex = 0; columIndex < A.cols(); ++columIndex ) {
			// std::cout <<
			// hypro::VariablePool::getInstance().pplVarByIndex(columIndex) << " = "
			// <<
			// A(rowIndex,columIndex).toDouble() << std::endl;
			polynom.set_coefficient( hypro::VariablePool::getInstance().pplVarByIndex( columIndex ),
									 carl::toDouble( A( rowIndex, columIndex ) ) );
		}
		Parma_Polyhedra_Library::Constraint constraint;
		constraint = polynom <= 0;

		mPolyhedron.add_constraint( constraint );
		// mPolyhedron.add_generator(gen);
	}
	mPointsUpToDate = false;
}

template <typename Number>
Polytope<Number>::Polytope( const C_Polyhedron &_rawPoly )
	: mPolyhedron( _rawPoly ), mPoints(), mPointsUpToDate( false ) {
}

template <typename Number>
bool Polytope<Number>::empty() const {
	return mPolyhedron.is_empty();
}

template <typename Number>
void Polytope<Number>::addPoint( const Point<Number> &point ) {
	Generator tmp = polytope::pointToGenerator( point );
	if ( mPolyhedron.space_dimension() != tmp.space_dimension() ) {
		mPolyhedron.add_space_dimensions_and_embed( tmp.space_dimension() );
	}
	mPolyhedron.add_generator( tmp );
	mPoints.push_back( point );
}

template <typename Number>
void Polytope<Number>::updatePoints() const {
	if ( !mPointsUpToDate ) {
		std::cout << __func__ << std::endl;
		mPoints.clear();
		std::set<Parma_Polyhedra_Library::Variable, Parma_Polyhedra_Library::Variable::Compare> variables =
			  hypro::polytope::variables( mPolyhedron );
		for ( auto &generator : mPolyhedron.generators() ) {
			// TODO: Call generatorToPoint only with the variables occuring in the
			// actual generator (some might be
			// degenerated)
			Point<Number> tmp = polytope::generatorToPoint<Number>( generator, variables );
			/*
			bool found = false;
			for ( auto &point : mPoints ) {
				if ( point == tmp ) {
					found = true;
					break;
				}
			}
			if ( !found ) {
				mPoints.push_back( tmp );
			}
			*/
			mPoints.push_back( tmp );  // we assume that the generators are already reduced.
		}
		mPointsUpToDate = true;
	}
}

template <typename Number>
const std::vector<Point<Number>> &Polytope<Number>::vertices() const {
	/*
	typename Point<Number>::pointSet result;
	std::set<Parma_Polyhedra_Library::Variable,
	Parma_Polyhedra_Library::Variable::Compare> variables =
	hypro::polytope::variables(mPolyhedron);
	for(auto& generator : mPolyhedron.generators())
	{
			// TODO: Call generatorToPoint only with the variables occuring in the
	actual generator (some might be
	degenerated)
			Point<Number> tmp = polytope::generatorToPoint<Number>(generator,
	variables);
			result.insert(tmp);
	}
	return result;
	*/
	if ( !mPointsUpToDate ) {
		updatePoints();
	}
	return mPoints;
}

template <typename Number>
std::vector<Point<Number>> &Polytope<Number>::rVertices() {
	/*
	if (!mPointsUpToDate) {
			updatePoints();
	}*/
	return mPoints;
}

// returns the fan of the Polytope
template <typename Number>
const polytope::Fan<Number> &Polytope<Number>::fan() {
	return mFan;
}

template <typename Number>
polytope::Fan<Number> &Polytope<Number>::rFan() {
	return mFan;
}

// sets the fan of the Polytope
template <typename Number>
void Polytope<Number>::setFan( const polytope::Fan<Number> &_fan ) {
	mFan = _fan;
}

template <typename Number>
void Polytope<Number>::calculateFan() {
	std::vector<Facet<Number>> facets = convexHull( mPoints );
	std::set<Point<Number>> preresult;
	for ( unsigned i = 0; i < facets.size(); i++ ) {
		for ( unsigned j = 0; j < facets[i].vertices().size(); j++ ) {
			preresult.insert( facets[i].vertices().at( j ) );
		}
	}
	polytope::Fan<Number> fan;
	for ( auto &point : preresult ) {
		polytope::Cone<Number> *cone = new polytope::Cone<Number>();
		for ( unsigned i = 0; i < facets.size(); i++ ) {
			for ( unsigned j = 0; j < facets[i].vertices().size(); j++ ) {
				if ( point == facets[i].vertices().at( j ) ) {
					std::vector<Ridge<Number>> ridges = getRidges( facets[i] );
					for ( unsigned m = 0; m < ridges.size(); m++ ) {
						if ( checkInsideRidge( ridges[m], point ) ) {
							std::vector<Facet<Number>> conefacets = shareRidge( facets, ridges[m] );

							matrix_t<Number> matrix = matrix_t<Number>( conefacets.size(), point.size() );
							for ( unsigned k = 1; k < conefacets.size(); k++ ) {
								for ( unsigned l = 0; l < conefacets[k].getNormal().size(); l++ ) {
									matrix( k, l ) = conefacets[k].getNormal()( l );
								}
							}

							for ( unsigned j = 0; j < point.size(); j++ ) {
								matrix( 0, j ) = 1;
								if ( matrix.fullPivLu().rank() == point.size() ) {
									break;
								} else {
									matrix( 0, j ) = 0;
								}
							}
							vector_t<Number> b = vector_t<Number>::Zero( conefacets.size() );
							b( 0 ) = 1;
							vector_t<Number> result = matrix.fullPivHouseholderQr().solve( b );

							cone->add( std::shared_ptr<Halfspace<Number>>(
								  new Halfspace<Number>( result, result.dot( point.rawCoordinates() ) ) ) );
							// cone->add(std::make_shared<Halfspace<Number>>(Halfspace<Number>(result,
							// result.dot(point.rawCoordinates()))));
						}
					}
				}
			}
		}
		fan.add( cone );
	}
	mFan = fan;
}

template <typename Number>
void Polytope<Number>::print() const {
	std::cout << "[";
	for ( auto &generator : mPolyhedron.generators() ) {
		generator.print();
	}
	std::cout << "]" << std::endl;
}

template <typename Number>
void Polytope<Number>::writeToFile( std::string _filename ) const {
	/*
	std::ofstream outputFile;
	outputFile.open(_filename);
	outputFile << *this;
	outputFile.close();
	*/
}

template <typename Number>
const C_Polyhedron &Polytope<Number>::rawPolyhedron() const {
	return mPolyhedron;
}

template <typename Number>
C_Polyhedron &Polytope<Number>::rRawPolyhedron() {
	return mPolyhedron;
}

template <typename Number>
std::size_t Polytope<Number>::dimension() const {
	return hypro::polytope::pplDimension( mPolyhedron );
}

template <typename Number>
Polytope<Number> Polytope<Number>::linearTransformation( const matrix_t<Number> &A, const vector_t<Number> &b ) const {
	using namespace Parma_Polyhedra_Library;
	Polytope<Number> result;

	std::vector<Parma_Polyhedra_Library::Variable> variables;
	for ( unsigned i = 0; i < A.rows(); ++i ) variables.push_back( VariablePool::getInstance().pplVarByIndex( i ) );

	const Generator_System generators = this->mPolyhedron.generators();

	// Create Eigen::Matrix from Polytope
	Eigen::Matrix<Number, Eigen::Dynamic, Eigen::Dynamic> polytopeMatrix( variables.size(),
																		  polytope::gsSize( generators ) );
	unsigned gCount = 0;

	Number coefficient;
	Number divisor;
	Number value;

	for ( Generator_System::const_iterator generatorIt = generators.begin(); generatorIt != generators.end();
		  ++generatorIt ) {
		unsigned vCount = 0;
		// Assuming the divisor stays the same in one generator
		divisor = (int)raw_value( generatorIt->divisor() ).get_si();
		for ( auto &var : variables ) {
			coefficient = (int)raw_value( generatorIt->coefficient( var ) ).get_si();
			value = coefficient / divisor;

			polytopeMatrix( vCount, gCount ) = value;
			++vCount;
		}
		++gCount;
	}

	// std::cout << __func__ << ": PolytopeMatrix: " << std::endl <<
	// polytopeMatrix << std::endl;

	// apply lineartransformation
	Eigen::Matrix<Number, Eigen::Dynamic, Eigen::Dynamic> res( variables.size(), polytope::gsSize( generators ) );

	// std::cout << __func__ << ": ARows: " << A.rows() << ", ACols: " << A.cols()
	// << ", polyRows: " <<
	// polytopeMatrix.rows() << ", polyCols: " << polytopeMatrix.cols() << ",
	// bRows: " << b.rows() << ", bCols: " <<
	// b.cols() << std::endl;

	// std::cout << __func__ << ": b:" << std::endl << b << std::endl;
	if ( b.rows() != 0 ) {
		res = ( A * polytopeMatrix );
		Eigen::Matrix<Number, Eigen::Dynamic, Eigen::Dynamic> tmp( res.rows(), res.cols() );
		for ( unsigned m = 0; m < tmp.rows(); ++m )
			for ( unsigned n = 0; n < tmp.cols(); ++n ) {
				tmp( m, n ) = b( m );
			}
		res += tmp;
	} else {
		res = ( A * polytopeMatrix );
	}

	// std::cout << "[EIGEN] linear transformation result: " << std::endl;
	// std::cout << res << std::endl;

	// clear actual generators and add new ones
	std::vector<vector_t<Number>> ps;
	for ( unsigned i = 0; i < res.cols(); ++i ) {
		// std::cout << res.col(i) << std::endl;
		vector_t<Number> t = vector_t<Number>( res.rows() );
		for ( unsigned j = 0; j < res.rows(); ++j ) t( j ) = res.col( i )( j );
		ps.push_back( t );
	}
	C_Polyhedron tmp = Parma_Polyhedra_Library::C_Polyhedron( res.rows(), Parma_Polyhedra_Library::EMPTY );

	std::vector<Point<Number>> newPoints;
	std::vector<Point<Number>> tmpPoints;

	for ( auto &pointSetIt : ps ) {
		tmp.add_generator( polytope::pointToGenerator( pointSetIt ) );
		Point<Number> tmpPoint = Point<Number>( pointSetIt );
		newPoints.push_back( tmpPoint );  // for mPoints
		tmpPoints.push_back( tmpPoint );  // for mNeighbors for each point
	}
	result.mPolyhedron = tmp;

	// update neighbor relations
	/*
	for(unsigned pointIndex = 0; pointIndex < mPoints.size(); ++pointIndex)
	{
			//std::cout << "Create neighbor for original node: " <<
	mPoints.at(pointIndex) << " , which is the new: " <<
	newPoints.at(pointIndex) << std::endl;
			std::vector<Point<Number> > tmpNeighbors =
	mPoints.at(pointIndex).neighbors();
			for(unsigned neighborIndex = 0 ; neighborIndex <  tmpNeighbors.size();
	++neighborIndex)
			{
					for(unsigned refPoint = 0; refPoint < mPoints.size();
	++refPoint)
					{
							if( mPoints.at(refPoint) ==
	tmpNeighbors.at(neighborIndex))
							{
									//std::cout << "Add neighbor: " <<
	tmpNeighbors.at(neighborIndex) << ", which is new " <<
	*tmpPoints.at(refPoint) << std::endl;
									newPoints.at(pointIndex).addNeighbor(tmpPoints.at(refPoint));
							}
					}
			}
	}
	*/

	result.setPointsUpToDate( false );

	return result;
}

template <typename Number>
Polytope<Number> Polytope<Number>::minkowskiSum( const Polytope<Number> &rhs ) const {
	Polytope<Number> result;
	/*
	typedef Point<Number> point;
	// initialize algorithm
	point::pointSet resultSet;

	// initialize sink node
	point sink = *this->points().begin() + *rhs.points().begin();
	point current = sink;

	// determine deltaK
	int deltaK = 0; // TODO

	std::pair<int, int> neighborCounter = std::make_pair(1,0);
	std::pair<int, int> maxCounter = std::make_pair(2, deltaK);

	do
	{
			while( neighborCounter < maxCounter )
	} while ( current != sink || neighborCounter != (2, deltaK));
	*/

	// TODO remove
	// std::cout.setstate(std::ios::failbit);

	// unelegant version creating the powerset of all points and reducing it
	// afterwards
	// std::cout << "Result before: " << std::endl;
	result = Parma_Polyhedra_Library::C_Polyhedron( 0, EMPTY );
	// result.print();

	assert( this->dimension() == rhs.dimension() );

	for ( auto &genA : mPolyhedron.generators() ) {
		Point<Number> tmpA = polytope::generatorToPoint<Number>( genA, polytope::variables( mPolyhedron ) );
		for ( auto &genB : rhs.rawPolyhedron().generators() ) {
			// std::cout << __func__ << " Generator: " << genB << std::endl;
			Point<Number> tmpB = polytope::generatorToPoint<Number>( genB, polytope::variables( rhs.rawPolyhedron() ) );

			// std::cout << __func__ << " Point: " << tmpB << std::endl;

			// std::cout << "Points in Hausdorff Poly: " << tmpB << std::endl;
			// std::cout << "tmpA: " << tmpA << std::endl;

			Point<Number> res = tmpA.extAdd( tmpB );

			// std::cout << "Add point: " << res << std::endl;
			result.addPoint( res );
			// std::cout << "Intermediate result:" << std::endl;
			// result.print();
			// std::cout << std::endl;
		}
	}
	// std::cout << "Result:";
	// result.print();
	// std::cout << std::endl;
	// result = result.hull();

	mPointsUpToDate = false;
	// TODO remove
	// std::cout.clear();
	return result;
}

/**
 * @author: Chris K.
 * Minkowski Sum computation based on Fukuda
 */
template <typename Number>
Polytope<Number> Polytope<Number>::altMinkowskiSum( Polytope<Number> &rhs ) {
	Polytope<Number> result;
	// TODO compute adjacency for this & rhs vertices (currently manually defined
	// within the tests)
	result = Parma_Polyhedra_Library::C_Polyhedron( 0, EMPTY );
	std::vector<Point<Number>> alreadyExploredVertices;

	/**
	 * Preprocessing
	 */

	// delta = max vertex degree
	int delta_1 = this->computeMaxVDegree();
	int delta_2 = rhs.computeMaxVDegree();

	// initVertex = initial extreme point & root of spanning tree
	Point<Number> initVertex = this->computeInitVertex( rhs );
	result.addPoint( initVertex );
	alreadyExploredVertices.push_back( initVertex );
#ifdef fukuda_DEBUG
	std::cout << "---------------" << std::endl;
	std::cout << "following Vertex is part of result: " << initVertex << std::endl;
	std::cout << "---------------" << std::endl;
#endif

	// compute the maximizer vector (& its target) for the initial extreme point
	// -> necessary for localSearch()
	Point<Number> sinkMaximizerTarget;
	vector_t<Number> sinkMaximizerVector = polytope::computeMaximizerVector( sinkMaximizerTarget, initVertex );

	// compute the normal cone of the initial extreme point
	polytope::Cone<Number> *cone = polytope::computeCone( initVertex, sinkMaximizerVector );
	// add this normal cone to the fan of the polytope
	result.rFan().add( cone );

	// set currentVertex to initVertex
	Point<Number> currentVertex = initVertex;

	// counter initially (1,0)
	std::pair<int, int> counter;
	counter.first = 1;
	counter.second = 0;

/**
 * Reverse Search Algorithm
 */
#ifdef fukuda_DEBUG
	std::cout << "-------------------------" << std::endl;
	std::cout << "-------------------------" << std::endl;
	std::cout << "The Preprocessing Ends here - Start of Algorithm" << std::endl;
	std::cout << "-------------------------" << std::endl;
	std::cout << "-------------------------" << std::endl;
#endif

	Point<Number> nextVertex;
	std::vector<std::pair<int, int>> counterMemory;
	bool alreadyExplored;
	std::map<Point<Number>, Point<Number>> parentMap;

	do {
		while ( counter.first < 2 || ( counter.first == 2 && counter.second < delta_2 ) ) {
#ifdef fukuda_DEBUG
			std::cout << "Max. Vertex Degree in Poly 1: " << delta_1 << std::endl;
			std::cout << "Max. Vertex Degree in Poly 2: " << delta_2 << std::endl;
#endif

			alreadyExplored = false;

			// increment counter by 1
			if ( counter.second == delta_1 ) {
				counter.first = 2;
				counter.second = 1;
			} else {
				counter.second += 1;
			}

#ifdef fukuda_DEBUG
			std::cout << "Counter tuple: (" << counter.first << "," << counter.second << ")" << std::endl;
#endif

			// choose next Vertex, only continue if one exists
			if ( polytope::adjOracle( nextVertex, currentVertex, counter ) ) {
				// set neighbors of the vertices accordingly - the adjacency oracle
				// confirmed neighborship
				// TODO problem: addNeighbor requires a pointer
				// currentVertex.addNeighbor(&nextVertex);
				// nextVertex.addNeighbor(&currentVertex);

				for ( unsigned i = 0; i < alreadyExploredVertices.size(); ++i ) {
					if ( nextVertex == alreadyExploredVertices.at( i ) ) {
						alreadyExplored = true;
#ifdef fukuda_DEBUG
						std::cout << "Vertex has already been explored: " << nextVertex << std::endl;
#endif
					}
				}
				if ( alreadyExplored ) {
#ifdef fukuda_DEBUG
					std::cout << "continue with next loop iteration" << std::endl;
					std::cout << "-------------------------" << std::endl;
#endif
					// dont traverse back and forth between two vertices
					continue;
				}
				Point<Number> localSearchVertex = result.localSearch( nextVertex, sinkMaximizerTarget );
				parentMap.insert( std::make_pair( nextVertex, localSearchVertex ) );
				if ( localSearchVertex == currentVertex ) {
					// reverse traverse
					currentVertex = nextVertex;

					// add to result Poly
					result.addPoint( currentVertex );
#ifdef fukuda_DEBUG
					std::cout << "---------------" << std::endl;
					std::cout << "following Vertex is part of result: " << currentVertex << std::endl;
					std::cout << "---------------" << std::endl;
#endif

					alreadyExploredVertices.push_back( currentVertex );
					// store the current counter value - needed if DFS comes back this
					// vertex
					counterMemory.push_back( counter );
#ifdef fukuda_DEBUG
					std::cout << "---------------" << std::endl;
					std::cout << "Counter Memory Stack - add Counter: (" << counter.first << "," << counter.second
							  << ")" << std::endl;
					std::cout << "Already explored Vertices: " << alreadyExploredVertices << std::endl;
					std::cout << "---------------" << std::endl;
#endif

					counter.first = 1;
					counter.second = 0;
				}
			}
		}
#ifdef fukuda_DEBUG
		std::cout << "---------------" << std::endl;
		std::cout << "While Loop left" << std::endl;
		std::cout << "---------------" << std::endl;
#endif
		if ( currentVertex != initVertex ) {
			// forward traverse
			// currentVertex = result.localSearch(currentVertex, sinkMaximizerTarget);
			// instead of computing the local Search result again, retrieve the parent
			// from the parentMap
			currentVertex = parentMap.at( currentVertex );

#ifdef fukuda_DEBUG
			std::cout << "Local Search finished" << std::endl;
			std::cout << "counterMemory size: " << counterMemory.size() << std::endl;
#endif

			// restore counter such that adjOracle(currentVertex,counter) = temp
			// use the "stack" counterMemory for that
			counter = counterMemory.at( counterMemory.size() - 1 );
			counterMemory.pop_back();

#ifdef fukuda_DEBUG
			std::cout << "---------------" << std::endl;
			std::cout << "Counter restored to: (" << counter.first << "," << counter.second << ")" << std::endl;
			std::cout << "While Loop Condition: "
					  << ( ( currentVertex != initVertex ) || ( counter.first != 2 && counter.second != delta_2 ) )
					  << std::endl;
			std::cout << "CurrrentVertex != initVertex: " << ( currentVertex != initVertex ) << std::endl;
			std::cout << "Counter != (2,2): " << ( counter.first != 2 || counter.second != delta_2 ) << std::endl;
			std::cout << "---------------" << std::endl;
#endif
		}

	} while ( ( currentVertex != initVertex ) || ( counter.first != 2 || counter.second != delta_2 ) );

	mPointsUpToDate = false;

	return result;
}

template <typename Number>
Polytope<Number> Polytope<Number>::intersect( const Polytope<Number> &rhs ) const {
	// std::cout << __func__ << ": " << *this << " and " << rhs << std::endl;
	if ( rhs.dimension() == 0 ) {
		return Polytope<Number>();
	} else {
		C_Polyhedron res = mPolyhedron;
		res.intersection_assign( rhs.rawPolyhedron() );
		Polytope<Number> result = Polytope<Number>( res );

		return result;
	}
}

template <typename Number>
Polytope<Number> Polytope<Number>::intersectHalfspace( const Halfspace<Number> &rhs ) const {
	if ( rhs.dimension() == 0 ) {
		return Polytope<Number>();
	} else {
		C_Polyhedron res = mPolyhedron;
		Parma_Polyhedra_Library::Linear_Expression ls;

		ls.set_space_dimension( rhs.dimension() );
		for ( unsigned rowIndex = 0; rowIndex < rhs.dimension(); ++rowIndex ) {
			ls.set_coefficient( hypro::VariablePool::getInstance().pplVarByIndex( rowIndex ),
								double( rhs.normal()( rowIndex ) * fReach_DENOMINATOR ) );
		}
		ls.set_inhomogeneous_term( -double( rhs.offset() * fReach_DENOMINATOR ) );

		Parma_Polyhedra_Library::Constraint constraint;
		constraint = ls <= 0;

		res.add_constraint( constraint );
		Polytope<Number> result = Polytope<Number>( res );

		return result;
	}
}

template <typename Number>
Polytope<Number> Polytope<Number>::intersectHalfspaces( const matrix_t<Number> &_mat,
														 const vector_t<Number> &_vec ) const {
	Polytope<Number> tmp( _mat, _vec );
	return this->intersect( tmp );
}

template <typename Number>
Polytope<Number> Polytope<Number>::hull() const {
	// Generator_System gs = mPolyhedron.minimized_generators();
	// Polytope<Number> result = Polytope<Number>(C_Polyhedron(gs));

	if ( !mPointsUpToDate ) {
		// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
		updatePoints();
	}
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	std::vector<std::shared_ptr<Facet<Number>>> hull = convexHull( mPoints ).first;
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	std::set<Point<Number>> preresult;

	for ( unsigned i = 0; i < hull.size(); i++ ) {
		for ( unsigned j = 0; j < hull[i]->rVertices().size(); j++ ) {
			// std::cout << "Unite created point: " <<
			// hull[i]->vertices().at(j).rawCoordinates().transpose() <<
			// std::endl;

			preresult.insert( hull[i]->rVertices().at( j ) );

			// it->joinNeighbors(hull[i]->rVertices().at(j).neighbors());

			// std::cout<<((Point<Number>)*(preresult.find(hull[i]->rVertices().at(j)))).neighbors().size()<<std::endl;

			/*
					std::cout << "Set after insert: ";
					for(const auto& point : preresult) {
					std::cout << point.rawCoordinates().transpose() << ", ";
					}
					std::cout << std::endl;
			*/
		}
	}
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	std::vector<Point<Number>> points;
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	for ( auto &point : preresult ) {
		points.push_back( point );
	}
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	Polytope<Number> result = Polytope<Number>( points );
	// std::cout<<__func__ << " : " <<__LINE__ <<std::endl;
	mPointsUpToDate = false;

	return result;
}

template <typename Number>
bool Polytope<Number>::contains( const Point<Number> &point ) const {
	return mPolyhedron.contains( Polytope<Number>( point ).rawPolyhedron() );
}

template <typename Number>
bool Polytope<Number>::contains( const Polytope<Number> &poly ) const {
	return mPolyhedron.contains( poly.rawPolyhedron() );
}

template <typename Number>
Polytope<Number> Polytope<Number>::unite( const Polytope<Number> &rhs ) const {
	if ( !mPointsUpToDate ) {
		updatePoints();
	}

	/*
	C_Polyhedron res = mPolyhedron;
	res.poly_hull_assign(rhs.rawPolyhedron());
	Polytope<Number> result = Polytope<Number>(res);
	*/
	if ( rhs.dimension() == 0 ) {
		return Polytope<Number>( this->vertices() );
	} else {
		std::vector<Point<Number>> unitedVertices = rhs.vertices();
		unitedVertices.insert( unitedVertices.end(), this->vertices().begin(), this->vertices().end() );
		assert( unitedVertices.size() == this->vertices().size() + rhs.vertices().size() );

		// std::cout << "United vertices: " << std::endl;
		// for(const auto& vertex : unitedVertices)
		//	std::cout << vertex.rawCoordinates().transpose() << std::endl;

		// std::cout << "Ping" << std::endl;
		std::vector<std::shared_ptr<Facet<Number>>> hull = convexHull( unitedVertices ).first;
		// std::cout << "Ping" << std::endl;

		std::set<Point<Number>> preresult;
		for ( unsigned i = 0; i < hull.size(); i++ ) {
			for ( unsigned j = 0; j < hull[i]->vertices().size(); j++ ) {
				// std::cout << "Unite created point: " <<
				// hull[i]->vertices().at(j).rawCoordinates().transpose() <<
				// std::endl;

				/*
				if((preresult.find(hull[i]->vertices().at(j))) != preresult.end()){
						Point<Number> pt = *(preresult.find(hull[i]->vertices().at(j)));
						std::vector<Point<Number>> neighbors =
				hull[i]->vertices().at(j).neighbors();
						for(auto& neigh:neighbors){
								pt.addNeighbor(neigh);
						}
				}
				else {*/
				preresult.insert( hull[i]->vertices().at( j ) );
				//}

				//	std::cout << "Set after insert: ";
				//	for(const auto& point : preresult) {
				//	std::cout << point.rawCoordinates().transpose() << ", ";
				//	}
				//	std::cout << std::endl;
			}
		}
		std::vector<Point<Number>> points;
		for ( auto &point : preresult ) {
			points.push_back( point );
		}

		Polytope<Number> result = Polytope<Number>( points );
		return result;
	}
	//    	return this->hull();
}

template <typename Number>
Number Polytope<Number>::supremum() const {
	Number max = 0;
	for ( auto &point : this->vertices() ) {
		Number inftyNorm = hypro::Point<Number>::inftyNorm( point );
		max = max > inftyNorm ? max : inftyNorm;
	}
	return max;
}

/*
template<typename Number>
Polytope<Number>& Polytope<Number>::operator= (const Polytope<Number>& rhs)
{
  if (this != &rhs)
  {
		Polytope<Number> tmp(rhs);
		std::swap(*this, tmp);
  }
  return *this;
}
*/

template <typename Number>
bool operator==( const Polytope<Number> &rhs, const Polytope<Number> &lhs ) {
	return ( rhs.rawPolyhedron() == lhs.rawPolyhedron() );
}
template <typename Number>
bool operator!=( const Polytope<Number> &rhs, const Polytope<Number> &lhs ) {
	return ( rhs.rawPolyhedron() != lhs.rawPolyhedron() );
}

/**
 * @author: Chris K.
 * in the following: Utility functions for altMinkowskiSum()
 */

/**
 * returns the maximum vertex degree in a polytope
 */
template <typename Number>
int Polytope<Number>::computeMaxVDegree() {
	std::vector<Point<Number>> points = this->points();
	int max = 0;

	for ( typename std::vector<Point<Number>>::iterator it = points.begin(); it != points.end(); ++it ) {
		if ( max < it->neighbors().size() ) {
			max = it->neighbors().size();
		}
	}

	return max;
}

/**
 * returns the vertex with maximum x-value for a given polytope
 */
template <typename Number>
Point<Number> Polytope<Number>::computeMaxPoint() {
	Point<Number> result;

	if ( !mPoints.empty() ) {
		result = *( mPoints.begin() );
		for ( typename std::vector<Point<Number>>::iterator it = mPoints.begin(); it != mPoints.end(); ++it ) {
			assert( it->dimension() == result.dimension() );
			assert( it->hasDimension( result.coordinates().begin()->first ) );
			Number coeff = it->coordinate( result.coordinates().begin()->first );
			if ( result.coordinates().begin()->second < coeff ) {
				result = *it;
			}
		}
	}
	return result;
}

/**
 * returns one vertex of the sum polytope P = P1+P2
 */
template <typename Number>
Point<Number> Polytope<Number>::computeInitVertex( Polytope<Number> _secondPoly ) {
	Point<Number> p1 = this->computeMaxPoint();
	Point<Number> p2 = _secondPoly.computeMaxPoint();

	Point<Number> res = p1.extAdd( p2 );

	// remember how the resulting point is composed (v= v1+v2)
	res.addToComposition( p1 );
	res.addToComposition( p2 );
	return res;
}

/**
 * the Local Search as per Fukuda's paper
 * computes the parent of a given vertex w.r.t the sink of the spanning tree
 */
template <typename Number>
Point<Number> Polytope<Number>::localSearch( Point<Number> &_vertex, Point<Number> &_sinkMaximizerTarget ) {
#ifdef fukuda_DEBUG
	std::cout << "-------------------------" << std::endl;
	std::cout << "in the following: Local Search for Vertex " << _vertex << std::endl;
	std::cout << "-------------------------" << std::endl;
#endif

	// compute the maximizer vector of the currently considered vertex
	Point<Number> maximizerTarget;
	vector_t<Number> maximizerVector = polytope::computeMaximizerVector( maximizerTarget, _vertex );

	// compute the ray direction (a vector)
	vector_t<Number> ray = polytope::computeEdge( maximizerTarget, _sinkMaximizerTarget );

#ifdef fukuda_DEBUG
	std::cout << "Starting Point of Ray: " << maximizerTarget << std::endl;
	std::cout << "End Point of Ray: " << _sinkMaximizerTarget << std::endl;
#endif

	// compute the normal cone of _vertex
	polytope::Cone<Number> *cone = polytope::computeCone( _vertex, maximizerVector );

	// iterate through all planes and check which one intersects with the ray
	Number factor;
	Point<Number> origin = cone->origin();
	Halfspace<Number> intersectedPlane;

	std::vector<Halfspace<Number> *> planes = cone->get();

#ifdef fukuda_DEBUG
	std::cout << "-----------------" << std::endl;
	std::cout << "Ray: " << ray << std::endl;
#endif

	for ( typename std::vector<Halfspace<Number> *>::iterator it = planes.begin(); it != planes.end(); ++it ) {
		if ( ( *it )->intersection( factor, ray ) ) {
#ifdef fukuda_DEBUG
			std::cout << "Intersection found " << std::endl;
#endif
			intersectedPlane = *( *it );
			break;
		}
	}

	Point<Number> secondOrigin;

#ifdef fukuda_DEBUG
	std::cout << "-----------------" << std::endl;
	std::cout << "Normal of Intersection Plane: " << intersectedPlane.normal() << std::endl;
	std::cout << "Offset of Intersection Plane: " << intersectedPlane.offset() << std::endl;
	std::cout << "-----------------" << std::endl;
#endif

	std::vector<vector_t<Number>> decompositionEdges = polytope::computeEdgeSet( _vertex );

	for ( unsigned i = 0; i < decompositionEdges.size(); i++ ) {
		Number dotProduct = intersectedPlane.normal().dot( decompositionEdges.at( i ) );
		Number normFactor = intersectedPlane.normal().norm() * decompositionEdges.at( i ).norm();

		// has to be done...
		dotProduct = std::round( dotProduct.toDouble() * 1000000 );
		normFactor = std::round( normFactor.toDouble() * 1000000 );

#ifdef fukuda_DEBUG
		std::cout << "Dot Product: " << dotProduct << std::endl;
		std::cout << "Norm Factor: " << normFactor << std::endl;
		std::cout << "Parallelism Factor: " << dotProduct / normFactor << std::endl;
		std::cout << "Value of the if condition: " << ( dotProduct / normFactor == 1 ) << std::endl;
#endif

		if ( ( dotProduct / normFactor == 1 + EPSILON ) || ( dotProduct / normFactor == 1 - EPSILON ) ||
			 ( dotProduct / normFactor == -1 + EPSILON ) || ( dotProduct / normFactor == -1 - EPSILON ) ||
			 ( dotProduct / normFactor == -1 ) || ( dotProduct / normFactor == 1 ) ) {
#ifdef fukuda_DEBUG
			std::cout << "Parallel Edge found" << std::endl;
			std::cout << "-----------------" << std::endl;
#endif

			// we have to find out from which decomposition our edge came from
			// and accordingly initialize a counter that describes the direction
			unsigned half = decompositionEdges.size() / 2;

			std::pair<int, int> counter;
			if ( i + 1 <= half ) {
				counter.first = 1;
				counter.second = i + 1;
			} else {
				counter.first = 2;
				counter.second = i + 1 - half;
			}

			// now we have to retrieve the new vertex in this edge direction, using
			// the adjacency oracle
			// the result is stored in secondOrigin
			bool res = polytope::adjOracle( secondOrigin, _vertex, counter );
			if ( res ) {
				break;
			}
		}
	}

	// add this normal cone to fan of polytope
	this->mFan.add( cone );

#ifdef fukuda_DEBUG
	std::cout << "-------------------------" << std::endl;
	std::cout << "Local Search result: " << secondOrigin << std::endl;
	std::cout << "-------------------------" << std::endl;
#endif

	return secondOrigin;
}
}