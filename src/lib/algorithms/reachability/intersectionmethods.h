/**
* This files contains the implementation of intersection related methods
*/

#define INTERSECTION_VERBOSE

          /*
          * This method implements testIntersection from algoInv.m
          */
          bool testIntersection(std::vector<double>* d, mapping minus_invariants_in_L, matrix_t<double> set)
          {
               #ifdef INTERSECTION_VERBOSE
                   std::cout << "testIntersection(...): " << "d:" << d << BL;
                   print(*d);
                   std::cout << BL;
                   std::cout << "mapping minus_invariants_in_L: d->size() = " << d->size() << BL;
                   printArray<unsigned int>(minus_invariants_in_L, d->size());
                   std::cout << BL << "set: " << BL << set << BL;
               #endif
                              
                  // loop over all L
                  for(unsigned int i=0; i<d->size(); i++)
                  {
                       if( -set( minus_invariants_in_L[i],1 ).toDouble() > d->at(i) )    // intersection condition
                       {
                           #ifdef INTERSECTION_VERBOSE
                                  std::cout << "testIntersection(...): " << "returns false" << BL;
                           #endif
                           return false;
                       }
                  }

               #ifdef INTERSECTION_VERBOSE
                   std::cout << "testIntersection(...): " << "returns true" << BL;
               #endif
               return true;
          }
          
          
          /*
          * Inplace operator to intersect a set with an invariant
          */
          // values(state.invariant_constraints_in_L) = min(values(state.invariant_constraints_in_L),state.d);
          void intersectWithInvariant(matrix_t<double> set, mapping constraints_in_L_A, std::vector<double>* inv_values)
          {
               int length = inv_values->size();
               
               for( int i=0; i<length; i++)
               {
                   set( constraints_in_L_A[i],1 ) = MIN(set( constraints_in_L_A[i],0 ), inv_values->at(i) ); 
               }
          }

