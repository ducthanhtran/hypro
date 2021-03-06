#pragma once

#ifndef INCL_FROM_GOHEADER
	static_assert(false, "This file may only be included indirectly by GeometricObject.h");
#endif

#include "../../flags.h"
#include "util.h"
#include "../Box/Box.h"
#include "../Ellipsoids/Ellipsoid.h"
#include "../OrthogonalPolyhedron/OrthogonalPolyhedron.h"
#include "../Polytope/Polytope.h"
#include "../Polytopes/HPolytope/HPolytope.h"
#include "../Polytopes/VPolytope/VPolytope.h"
#include "../SupportFunction/SupportFunction.h"
#include "../Zonotope/Zonotope.h"
#include "../../util/pca.h"

namespace hypro {

/**
 * @brief      enum for differentiating between exact conversion and over-/underapproximation (some functions support an alternative computation method).
 */
enum CONV_MODE {
	EXACT, OVER, UNDER, ALTERNATIVE
};

template<typename Number>
class Converter {
	public:
		using Box = BoxT<Number,Converter>;
		using Ellipsoid = EllipsoidT<Number,Converter>;
		using HPolytope = HPolytopeT<Number,Converter>;
		using OrthogonalPolyhedron = OrthogonalPolyhedronT<Number,Converter>;
		using VPolytope = VPolytopeT<Number,Converter>;
		#ifdef HYPRO_USE_PPL
		using Polytope = PolytopeT<Number,Converter>;
		#endif
		using SupportFunction = SupportFunctionT<Number,Converter>;
		using Zonotope = ZonotopeT<Number,Converter>;

		static Box toBox(const Box& source, const CONV_MODE = CONV_MODE::EXACT);
		static Box toBox(const Ellipsoid& source, const CONV_MODE = CONV_MODE::EXACT);
		static Box toBox(const HPolytope& source, const CONV_MODE = CONV_MODE::OVER);
		static Box toBox(const VPolytope& source, const CONV_MODE = CONV_MODE::OVER);
		#ifdef HYPRO_USE_PPL
		static Box toBox(const Polytope& source, const CONV_MODE = CONV_MODE::OVER);
		#endif
		static Box toBox(const SupportFunction& source, const CONV_MODE = CONV_MODE::OVER);
		static Box toBox(const Zonotope& source, const CONV_MODE = CONV_MODE::OVER);

		static HPolytope toHPolytope(const Box& source, const CONV_MODE = CONV_MODE::EXACT);
		static HPolytope toHPolytope(const Ellipsoid& source, const CONV_MODE = CONV_MODE::EXACT);
		static HPolytope toHPolytope(const HPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static HPolytope toHPolytope(const VPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static HPolytope toHPolytope(const SupportFunction& source, const std::vector<vector_t<Number>>& additionalDirections = std::vector<vector_t<Number>>(), const CONV_MODE = CONV_MODE::OVER, unsigned numberOfDirections = defaultTemplateDirectionCount );
		static HPolytope toHPolytope(const Zonotope& source, const CONV_MODE = CONV_MODE::EXACT);

		static VPolytope toVPolytope(const Box& source, const CONV_MODE = CONV_MODE::EXACT);
		static VPolytope toVPolytope(const Ellipsoid& source, const CONV_MODE = CONV_MODE::EXACT);
		static VPolytope toVPolytope(const HPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static VPolytope toVPolytope(const VPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static VPolytope toVPolytope(const SupportFunction& source, const CONV_MODE = CONV_MODE::UNDER, unsigned numberOfDirections = defaultTemplateDirectionCount );
		static VPolytope toVPolytope(const Zonotope& source, const CONV_MODE = CONV_MODE::EXACT);

		static SupportFunction toSupportFunction(const Box& source, const CONV_MODE = CONV_MODE::EXACT);
		static SupportFunction toSupportFunction(const Ellipsoid& source, const CONV_MODE = CONV_MODE::EXACT);
		static SupportFunction toSupportFunction(const HPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static SupportFunction toSupportFunction(const VPolytope& source, const CONV_MODE = CONV_MODE::EXACT);
		static SupportFunction toSupportFunction(const SupportFunction& source, const CONV_MODE = CONV_MODE::EXACT);
		static SupportFunction toSupportFunction(const Zonotope& source, const CONV_MODE = CONV_MODE::EXACT);

		static Zonotope toZonotope(const Box& source, const CONV_MODE = CONV_MODE::EXACT);
		static Zonotope toZonotope(const Ellipsoid& source, const CONV_MODE = CONV_MODE::EXACT);
		static Zonotope toZonotope(const HPolytope& source, const CONV_MODE = CONV_MODE::OVER);
		static Zonotope toZonotope(const VPolytope& source, const CONV_MODE = CONV_MODE::OVER);
		static Zonotope toZonotope(const SupportFunction& source, const CONV_MODE = CONV_MODE::OVER, unsigned numberOfDirections = defaultTemplateDirectionCount );
		static Zonotope toZonotope(const Zonotope& source, const CONV_MODE = CONV_MODE::EXACT);
};

template<typename Number>
using Box = typename Converter<Number>::Box;

template<typename Number>
using Ellipsoid = typename Converter<Number>::Ellipsoid;

/**
 * Typedef for HpolytopeT.
 */
template<typename Number>
using HPolytope = typename Converter<Number>::HPolytope;

template<typename Number>
using OrthogonalPolyhedron = typename Converter<Number>::OrthogonalPolyhedron;

/**
 * Typedef for VPolytopeT.
 */
template<typename Number>
using VPolytope = typename Converter<Number>::VPolytope;

#ifdef HYPRO_USE_PPL
template<typename Number>
using Polytope = typename Converter<Number>::Polytope;
#endif

/**
 * Typedef for SupportFunctionT.
 */
template<typename Number>
using SupportFunction = typename Converter<Number>::SupportFunction;

/**
 * Typedef for ZonotopeT.
 */
template<typename Number>
using Zonotope = typename Converter<Number>::Zonotope;

#include "converterToBox.tpp"
#include "converterToHPolytope.tpp"
#include "converterToVPolytope.tpp"
#include "converterToSupportFunction.tpp"
#include "converterToZonotope.tpp"

} // namespace hypro
