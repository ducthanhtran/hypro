/**
 * @file    OrthogonalPolyhedron.tpp
 * @author  Sebastian Junges
 * @author	Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @author  Benedikt Seidl
 *
 * @since   2014-03-17
 * @version 2015-07-09
 */

namespace hypro {
template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron()
	: mGrid(), mBoundaryBox(), mBoxUpToDate( false ) {
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const Vertex<Number> &_vertex )
	: mGrid(), mBoundaryBox(), mBoxUpToDate( false ) {
	mGrid.insert( _vertex.point(), _vertex.color() );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const VertexContainer<Number> &_vertices )
	: mGrid( _vertices.vertices() ), mBoundaryBox(), mBoxUpToDate( false ) {
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const std::set<Vertex<Number>> &_vertices )
	: mGrid( _vertices ), mBoundaryBox(), mBoxUpToDate( false ) {
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const std::vector<Vertex<Number>> &_vertices )
	: mGrid( _vertices ), mBoundaryBox(), mBoxUpToDate( false ) {
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const OrthogonalPolyhedron<Number, Converter, Type> &copy )
	: mGrid( copy.vertices() ), mBoundaryBox(), mBoxUpToDate( false ) {
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type>::OrthogonalPolyhedron( const OrthogonalPolyhedron<Number, Converter, Type> &&move )
	: mGrid( std::move( move.mGrid ) ), mBoundaryBox(), mBoxUpToDate( false ) {
}

/**********************************
 * Getter & setter
 **********************************/

/**
 * Returns the list of variables of this polyhedron
 */
template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<carl::Variable> OrthogonalPolyhedron<Number, Converter, Type>::variables() const {
	return mGrid.variables();
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<Vertex<Number>> OrthogonalPolyhedron<Number, Converter, Type>::vertices() const {
	return mGrid.vertices();
}

/**
 * Returns and if necessary calculates the boundary box.
 */
template <typename Number, typename Converter, ORTHO_TYPE Type>
BoxT<Number,Converter> OrthogonalPolyhedron<Number, Converter, Type>::boundaryBox() const {
	if ( !mBoxUpToDate ) {
		updateBoundaryBox();
	}
	return mBoundaryBox;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
const Grid<Number> &OrthogonalPolyhedron<Number, Converter, Type>::grid() const {
	return mGrid;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
void OrthogonalPolyhedron<Number, Converter, Type>::addVertex( const Vertex<Number> &_vertex ) {
	mGrid.insert( _vertex );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
void OrthogonalPolyhedron<Number, Converter, Type>::addVertices( const std::vector<Vertex<Number>> &_vertices ) {
	for ( const auto &vertex : _vertices ) mGrid.insert( vertex );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::empty() const {
	return mGrid.empty();
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::size_t OrthogonalPolyhedron<Number, Converter, Type>::size() const {
	return mGrid.size();
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::size_t OrthogonalPolyhedron<Number, Converter, Type>::dimension() const {
	return mGrid.dimension();
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
Number OrthogonalPolyhedron<Number, Converter, Type>::supremum() const {
	// TODO
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::isVertex( const Point<Number> &_point ) const {
	for ( unsigned dimension : this->dimension() ) {
		if ( !isOnIEdge( _point, dimension ) ) return false;
	}
	return true;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::isOnIEdge( const Point<Number> &_point, unsigned i ) const {
	std::vector<Point<Number>> iNeighbors = mGrid.iNeighborhood( _point, i );
	for ( const auto &neighbor : iNeighbors ) {
		if ( mGrid.colorAt( neighbor ) != mGrid.colorAt( mGrid.iPredecessor( neighbor, i ) ) ) return true;
	}
	return false;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::isInternal( const Point<Number> &_point ) const {
	return ( mGrid.colorAt( _point ) == true );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::isExternal( const Point<Number> &_point ) const {
	return !isInternal( _point );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::color( const Point<Number> &_point ) const {
	return mGrid.colorAt( _point );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<Point<Number>> OrthogonalPolyhedron<Number, Converter, Type>::iNeighborhood( const Point<Number> &_point,
																			  unsigned i ) const {
	return mGrid.iNeighborhood( _point, i );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<Point<Number>> OrthogonalPolyhedron<Number, Converter, Type>::iNegNeighborhood( const Point<Number> &_point,
																				 unsigned i ) const {
	return mGrid.iNeighborhood( mGrid.iPredecessor( _point, i ), i );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<Point<Number>> OrthogonalPolyhedron<Number, Converter, Type>::neighborhood( const Point<Number> &_point ) const {
	return std::move( mGrid.neighborhood( _point ) );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<Point<Number>> OrthogonalPolyhedron<Number, Converter, Type>::iSlice( unsigned i, Number pos ) const {
	// TODO: ATTENTION, this is the induced version, transform!
	return std::move( mGrid.iSlice( i, pos ) );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::iProjection( unsigned i ) const {
	// TODO: Required for dimension reduction and more sophisticated containment
	// methods.
}

/**********************************
 * Geometric Object functions
 **********************************/

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::linearTransformation(
	  const matrix_t<Number> &A, const vector_t<Number> &b ) const {
	/*
	VertexContainer<Number> newVertices(mVertices);
	newVertices.linearTransformation(A, b);
	OrthogonalPolyhedron<Number, Converter, Type> result = OrthogonalPolyhedron<Number,
	Type>(newVertices);
	// TODO: undefined behavior, does not update colors of vertices nor fix the
	non-parallel edges
	return result;
	*/
	OrthogonalPolyhedron<Number, Converter, Type> result;
	return result;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::minkowskiSum(
	  const OrthogonalPolyhedron<Number, Converter, Type> &rhs ) const {
	// TODO
	// Definition: A+B = { a + b | a ∈ A, b ∈ B}
	// Idea: put one polyhedron on all the vertices of the other one
	// Problem: Which vertices to connect, which to remove?
	// Thoughts: do two vertices belong to former neighbors?
	//           store the "color" of a moved polyhedron
	OrthogonalPolyhedron<Number, Converter, Type> result;
	return result;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::intersect(
	  const OrthogonalPolyhedron<Number, Converter, Type> &rhs ) const {
	std::vector<Vertex<Number>> potentialVertices;
	std::vector<Vertex<Number>> v1 = this->vertices();
	std::vector<Vertex<Number>> v2 = rhs.vertices();
	std::vector<Vertex<Number>> resVertices;

	// potential vertices are all original vertices and their pairwise
	// componentwise-max.
	potentialVertices.insert( potentialVertices.end(), v1.begin(), v1.end() );
	potentialVertices.insert( potentialVertices.end(), v2.begin(), v2.end() );
	for ( const auto &vA : v1 ) {
		for ( const auto &vB : v2 ) {
			potentialVertices.emplace_back( Point<Number>::coeffWiseMax( vA.point(), vB.point() ) );
		}
	}
	std::unique( potentialVertices.begin(), potentialVertices.end() );

	Grid<Number> aCombination = Grid<Number>::combine( this->mGrid, rhs.grid() );
	Grid<Number> bCombination = Grid<Number>::combine( this->mGrid, rhs.grid() );

	for ( const auto &vertex : v1 ) {
		aCombination.insert( vertex.point(), vertex.color() );
	}

	for ( const auto &vertex : v2 ) bCombination.insert( vertex.point(), vertex.color() );

	for ( auto pIt = potentialVertices.begin(); pIt != potentialVertices.end(); ++pIt ) {
		// std::cout << "Consider potential vertex " << *pIt << std::endl;

		std::vector<Point<Number>> neighborsA = aCombination.neighborhood( pIt->point() );
		std::vector<Point<Number>> neighborsB = bCombination.neighborhood( pIt->point() );

		// ATTENTION: As both combination-grids should be identical, the neighbor
		// order should be too - we assume that
		// here!
		assert( neighborsA.size() == neighborsB.size() );
		std::vector<Vertex<Number>> vertices;
		for ( unsigned id = 0; id < neighborsA.size(); ++id ) {
			// white dominates
			if ( aCombination.colorAt( neighborsA[id] ) == true && bCombination.colorAt( neighborsB[id] ) == true ) {
				vertices.emplace_back( neighborsA[id], true );
			} else {
				vertices.emplace_back( neighborsA[id], false );
			}
		}

		// std::cout << "Unified neighborhood: ";
		// for(const auto& v : vertices)
		//	std::cout << v << " ";

		// std::cout << std::endl;

		Grid<Number> tmp( vertices );
		if ( tmp.isVertex( pIt->point() ) ) {
			bool color = tmp.colorAt( pIt->point() );
			// std::cout << "Is vertex " << pIt->point() << " " << color << std::endl;
			resVertices.emplace_back( pIt->point(), color );
		}
	}

	// std::cout << "Computed vertices: " << std::endl;
	// for(const auto& v : resVertices ) {
	//	std::cout << v << std::endl;
	//}

	return ( OrthogonalPolyhedron<Number, Converter, Type>( std::move( resVertices ) ) );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::hull() const {
	if ( !mBoxUpToDate ) {
		updateBoundaryBox();
	}

	int nrofVertices = pow( 2, dimension() );
	VertexContainer<Number> container;
	Vertex<Number> vertex;

	for ( int vertexNr = 0; vertexNr < nrofVertices; vertexNr++ ) {
		int i = 0;
		for ( std::size_t p = 0; p < mGrid.dimension(); ++p ) {
			// look if the bit of the current dimension is set
			if ( ( vertexNr >> i++ ) & 1 ) {
				vertex[p] = mBoundaryBox.interval( p ).upper();
			} else {
				vertex[p] = mBoundaryBox.interval( p ).lower();
			}
		}
		vertex.setColor( vertexNr == 0 );
		container.insert( vertex );
	}
	OrthogonalPolyhedron<Number, Converter, Type> result = OrthogonalPolyhedron<Number, Converter, Type>( container );

	return result;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::contains( const Point<Number> &point ) const {
	bool color = mGrid.colorAt( point );
	// std::cout << __func__ << " Point " << point << ": color " << color <<
	// std::endl;
	return ( color || ( !color && mGrid.isOnFacet( point ) ) );
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
bool OrthogonalPolyhedron<Number, Converter, Type>::contains( const OrthogonalPolyhedron<Number, Converter, Type> &_other ) const {
	for ( const auto &vertex : _other.vertices() ) {
		if ( !contains( vertex.point() ) ) return false;
	}
	return true;
}

template <typename Number, typename Converter, ORTHO_TYPE Type>
OrthogonalPolyhedron<Number, Converter, Type> OrthogonalPolyhedron<Number, Converter, Type>::unite(
	  const OrthogonalPolyhedron<Number, Converter, Type> &rhs ) const {
	/* Algorithm: create combined grids each, for each potential vertex determine
	 * vertex condition, black dominates,
	 * same as for intersection, just inverted:
	 * If vertex condition holds, color = color in originals, black dominates.
	 */

	std::vector<Vertex<Number>> potentialVertices;
	std::vector<Vertex<Number>> v1 = this->vertices();
	std::vector<Vertex<Number>> v2 = rhs.vertices();
	std::vector<Vertex<Number>> resVertices;

	// potential vertices are all original vertices and their pairwise
	// componentwise-max.
	potentialVertices.insert( potentialVertices.end(), v1.begin(), v1.end() );
	potentialVertices.insert( potentialVertices.end(), v2.begin(), v2.end() );
	for ( const auto &vA : v1 ) {
		for ( const auto &vB : v2 ) {
			potentialVertices.emplace_back( Point<Number>::coeffWiseMax( vA.point(), vB.point() ) );
		}
	}
	std::unique( potentialVertices.begin(), potentialVertices.end() );

	Grid<Number> aCombination = Grid<Number>::combine( this->mGrid, rhs.grid() );
	Grid<Number> bCombination = Grid<Number>::combine( this->mGrid, rhs.grid() );

	for ( const auto &vertex : v1 ) {
		aCombination.insert( vertex.point(), vertex.color() );
	}

	for ( const auto &vertex : v2 ) bCombination.insert( vertex.point(), vertex.color() );

	for ( auto pIt = potentialVertices.begin(); pIt != potentialVertices.end(); ++pIt ) {
		// std::cout << "Consider potential vertex " << *pIt << std::endl;

		std::vector<Point<Number>> neighborsA = aCombination.neighborhood( pIt->point() );
		std::vector<Point<Number>> neighborsB = bCombination.neighborhood( pIt->point() );

		// ATTENTION: As both combination-grids should be identical, the neighbor
		// order should be too - we assume that
		// here!
		assert( neighborsA.size() == neighborsB.size() );
		std::vector<Vertex<Number>> vertices;
		for ( unsigned id = 0; id < neighborsA.size(); ++id ) {
			// black dominates
			if ( aCombination.colorAt( neighborsA[id] ) == false && bCombination.colorAt( neighborsB[id] ) == false ) {
				vertices.emplace_back( neighborsA[id], false );
			} else {
				vertices.emplace_back( neighborsA[id], true );
			}
		}

		// std::cout << "Unified neighborhood: ";
		// for(const auto& v : vertices)
		// 	std::cout << v << " ";

		// std::cout << std::endl;

		Grid<Number> tmp( vertices );
		if ( tmp.isVertex( pIt->point() ) ) {
			bool color = tmp.colorAt( pIt->point() );
			// std::cout << "Is vertex " << pIt->point() << " " << color << std::endl;
			resVertices.emplace_back( pIt->point(), color );
		}
	}

	return ( OrthogonalPolyhedron<Number, Converter, Type>( std::move( resVertices ) ) );
}

/**********************************
 * Other functions
 **********************************/

template <typename Number, typename Converter, ORTHO_TYPE Type>
std::vector<std::vector<Point<Number>>> OrthogonalPolyhedron<Number, Converter, Type>::preparePlot( unsigned _xDim,
																						 unsigned _yDim ) const {
	std::vector<std::vector<Point<Number>>> result;

	std::vector<Point<Number>> points = mGrid.allBlack();

	// add points
	for ( auto point : points ) {
		point.reduceToDimensions( std::vector<unsigned>( {_xDim, _yDim} ) );
		Point<Number> lowerRight = mGrid.iSuccessor( point, 0 );
		lowerRight.reduceToDimensions( std::vector<unsigned>( {_xDim, _yDim} ) );
		Point<Number> upperRight = mGrid.directSuccessor( point );
		upperRight.reduceToDimensions( std::vector<unsigned>( {_xDim, _yDim} ) );
		Point<Number> upperLeft = mGrid.iSuccessor( point, 1 );
		upperLeft.reduceToDimensions( std::vector<unsigned>( {_xDim, _yDim} ) );

		// std::cout << "point " << point << " corresponds to box " << point << " ,
		// " << lowerRight << " , " <<
		// upperRight << " , " << upperLeft << std::endl;
		result.emplace_back( std::vector<Point<Number>>( {point, lowerRight, upperRight, upperLeft} ) );
	}

	return result;
}

/***************************************************************************
 * Private methods
 ***************************************************************************/

/**
 * Updates the boundary box
 */
template <typename Number, typename Converter, ORTHO_TYPE Type>
void OrthogonalPolyhedron<Number, Converter, Type>::updateBoundaryBox() const {
	// If there are no vertices, the box is empty
	if ( mGrid.empty() ) {
		mBoundaryBox.clear();
	}

	std::vector<Vertex<Number>> vertices = mGrid.vertices();
	std::vector<Point<Number>> points;
	std::for_each(vertices.begin(), vertices.end(), [&](const Vertex<Number>& v){points.push_back(Point<Number>(v.rawCoordinates()));});

	mBoundaryBox = BoxT<Number,Converter>( points );

	mBoxUpToDate = true;
}
}
