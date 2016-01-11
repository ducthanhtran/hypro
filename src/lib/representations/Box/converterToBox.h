/**
 * Specialization for a converter to a box.
 * @file converterToBox.h
 * @author Stefan Schupp <stefan.schupp@cs.rwth-aachen.de>
 * @author Simon Froitzheim
 *
 * @since	2015-05-12
 * @version	2016-01-11
 */

#pragma once

namespace hypro {
   
    /**
     * @brief Converts a box into another box.
     * @detail Simply overwrites _target with _source.
     *   
     * @param _source Box that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @param mode The requested conversion mode (exact conversion or overapproximation).
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::Box<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    /**
     * @brief Converts a support function into a box. (overapproximation in the general case)
     * @detail Creates a normal matrix with 2*d rows (for 2*d faces) and d columns that contains the basic 2 directions for every dimension.
     * The source object then gets evaluated in these 2*d basic directions which yields the interval end points of the new box.
     * Checks for exact conversion by validating that every vertex of the newly created box was already inside the source object.
     *   
     * @param _source Support Function that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::SupportFunction<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    /**
     * @brief Converts a polytope in V-representation into a box. (overapproximation in the general case)
     * @detail Computes the minimal and maximal point component for every dimension and sets up intervals ranging from the corresponding minimal value to the corresponding maximal value for every dimension.
     * Checks for exact conversion by validating that every vertex of the newly created box was already a vertex of the source object.
     *   
     * @param _source V-polytope that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @param mode The requested conversion mode (exact conversion or overapproximation).
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::VPolytope<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    /**
     * @brief Converts a polytope in H-representation into a box.
     * @detail Computes the minimal and maximal point component for every dimension and sets up intervals ranging from the corresponding minimal value to the corresponding maximal value for every dimension.
     * This is done by converting the H-Polytope into a V-Polytope at first.
     * Checks for exact conversion by validating that every vertex of the newly created box was already a vertex of the source object.
     *   
     * @param _source H-polytope that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @param mode The requested conversion mode (exact conversion or overapproximation).
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::HPolytope<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    /**
     * @brief Converts a zonotope into a box.
     * @detail Computes the minimal and maximal point component for every dimension and sets up intervals ranging from the corresponding minimal value to the corresponding maximal value for every dimension.
     * This is done by computing the vertices of the zonotope first.
     * Checks for exact conversion by validating that every vertex of the newly created box was already a vertex of the source object.
     *   
     * @param _source Zonotope that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @param mode The requested conversion mode (exact conversion or overapproximation).
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::Zonotope<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    /**
     * @brief Converts a polytope (different data structure) into a box.
     * @detail Computes the minimal and maximal point component for every dimension and sets up intervals ranging from the corresponding minimal value to the corresponding maximal value for every dimension.
     * Checks for exact conversion by validating that every vertex of the newly created box was already a vertex of the source object.
     *   
     * @param _source Polytope that needs to be converted.
     * @param _target An arbitrary box that gets overwritten with the converted object.
     * @param mode The requested conversion mode (exact conversion or overapproximation).
     * @return Return value is true if the requested conversion was successful.
     */  
    template <typename Number>
    static bool convert( const hypro::Polytope<Number>& _source, hypro::Box<Number>& _target, const CONV_MODE mode = CONV_MODE::OVER );
    
    

}  // namespace

#include "converterToBox.tpp"