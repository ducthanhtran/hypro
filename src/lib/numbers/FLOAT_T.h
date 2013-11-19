/**
 * General class for floating point numbers with different formats. Extend to
 * other types if necessary.
 * 
 * @file FLOAT_T.h
 * @author  Stefan Schupp
 * @since   2013-10-14
 * @version 2013-10-14
 */

#pragma once

#include <string>
#include <iostream>
#include <stdexcept>

namespace hypro
{
		// nearest, towards zero, towards infty, towards -infty, away from zero
    enum HYPRO_RND		// changed
    {
     HYPRO_RNDN=0,
     HYPRO_RNDZ=1,
     HYPRO_RNDU=2,
     HYPRO_RNDD=3,
     HYPRO_RNDA=4
    };

    typedef unsigned long precision;
    
    template<typename FloatType>
    class FLOAT_T;
    
    template<typename T1, typename T2>
    struct FloatConv
    {
        FLOAT_T<T1> operator() ( const FLOAT_T<T2>& _op2 ) const
        {
            return FLOAT_T<T1>(_op2.toDouble());
        }
    };
    
    template<typename FloatType>
    class FLOAT_T
    {
        private:
            FloatType mValue;
        
        public:
    	
            /**
             * Constructors & Destructors
             */

            FLOAT_T<FloatType>()
            {
                mValue = 0;
            }
            
            FLOAT_T<FloatType>(const double _double)
            {
                mValue = _double;
            }
            
            FLOAT_T<FloatType>(const float _float)
            {
                mValue = _float;
            }
            
            FLOAT_T<FloatType>(const int _int)
            {
                mValue = _int;
            }

            FLOAT_T<FloatType>(const double _double, precision _prec){}

//            template<>
//            FLOAT_T<double>(const double _double, HYPRO_RND _rnd, precision _prec)
//            {
//                // TODO: What if a lesser precision than the default double precision is chosen? -> truncate with respect to _rnd!
//                mValue = _double;
//            }
            
            ~FLOAT_T()
            {
            	// TODO: clear mValue if needed
            }

            
            /*******************
            * Getter & Setter *
            *******************/

           const FloatType& getValue() const
           {
               return mValue;
           }

           const precision getPrec() const
           {
               // TODO
               return 0;
           }

           FLOAT_T<FloatType>& setPrec( const precision& _prec, const HYPRO_RND _rnd=HYPRO_RND::HYPRO_RNDN )
           {
               // TODO
               return *this;
           }
            
            /*************
             * Operators *
             *************/
            
            FLOAT_T<FloatType>& operator = (const FLOAT_T<FloatType> & _rhs)
            {
                mValue = _rhs.mValue;
                return *this;
            }
            
            /**
             * Boolean operators 
             */
            
            // TODO
            
            /**
             * arithmetic operations
             */
            
            FLOAT_T& add_assign( const FLOAT_T<FloatType>& _op2, HYPRO_RND _rnd )
            {
                // TODO: Include rounding
                mValue = mValue + _op2.mValue;
                return *this;
            }
            
            void add( FLOAT_T& _result, const FLOAT_T& _op2, HYPRO_RND _rnd) const
            {
                // TODO: Include rounding
                _result.mValue = mValue + _op2.mValue;
            }

            FLOAT_T& sub_assign(const FLOAT_T& _op2, HYPRO_RND _rnd)
            {
                // TODO: Include rounding
                mValue = mValue - _op2.mValue;
                return *this;
            }
            
            void sub(FLOAT_T& _result, const FLOAT_T& _op2, HYPRO_RND _rnd) const
            {
                // TODO: Include rounding
                _result.mValue = mValue - _op2.mValue;
            }
            
            FLOAT_T& mul_assign(const FLOAT_T& _op2, HYPRO_RND _rnd)
            {
                // TODO: Include rounding
                mValue = mValue * _op2.mValue;
                // mValue.mul_assign(_op2.mValue, _rnd);
                return *this;
            }
            
            void mul(FLOAT_T& _result, const FLOAT_T& _op2, HYPRO_RND _rnd) const
            {
                // TODO: Include rounding
                _result.mValue = mValue * _op2.mValue;
            }
            
            FLOAT_T& div_assign(const FLOAT_T& _op2, HYPRO_RND _rnd) throw (std::invalid_argument)
            {
                if( _op2 == 0 ) throw ( std::invalid_argument( "Division by zero not allowed." ) );
                // TODO: Include rounding
                mValue = mValue / _op2.mValue;
                return *this;
            }
            
            void div(FLOAT_T& _result, const FLOAT_T& _op2, HYPRO_RND _rnd) const throw (std::invalid_argument) 
            {
                if( _op2 == 0 ) throw ( std::invalid_argument( "Division by zero not allowed." ) );
                // TODO: Include rounding
                _result.mValue = mValue / _op2.mValue;
            }
            
            /**
             * special operators
             */

            FLOAT_T& sqrt()
            {
                mValue = sqrt(mValue);
                return *this;
            }

            FLOAT_T& cbrt()
            {
                mValue = cbrt(mValue);
                return *this;
            }

            FLOAT_T& root(unsigned long int _k)
            {
                // TODO
                return *this;
            }

            FLOAT_T& pow(unsigned long int _exp)
            {
                mValue = pow(mValue, _exp);
                return *this;
            }

            FLOAT_T& abs( HYPRO_RND _rnd)
            {
                mValue = abs(mValue);
                return *this;
            }

            /**
             * conversion operators
             */
            double toDouble(HYPRO_RND _rnd=HYPRO_RND::HYPRO_RNDN) const
            {
                return (double) mValue;
            }
            

            friend std::ostream & operator<< (std::ostream& ostr, const FLOAT_T<FloatType>& p) {
                ostr << p.toString();
                return ostr;
            }
            
            friend bool operator== (const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _lhs.mValue == _rhs.mValue;
            }
            
            /**
             * Overloaded operators
             */
            
            friend FLOAT_T<FloatType> operator +(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return FLOAT_T<FloatType>(_lhs.mValue + _rhs.mValue);
            }
            
            friend FLOAT_T<FloatType> operator -(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _lhs - _rhs;
            }
            
            friend FLOAT_T<FloatType> operator *(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _lhs * _rhs;
            }
            
            friend FLOAT_T<FloatType> operator /(const FLOAT_T<FloatType>& _lhs, const FLOAT_T<FloatType>& _rhs)
            {
                return _lhs / _rhs;
            }
            
            /**
             * Auxiliary Functions
             */
            
            std::string toString() const
            {
                std::stringstream str;
                str << mValue;   
                return str.str();
            }
    };
    
    #include "float/mpfr_float.tpp"
}
