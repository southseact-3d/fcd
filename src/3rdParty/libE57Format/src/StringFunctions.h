#pragma once

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace e57
{
   /// @brief Create whitespace of given length, for indenting printouts in dump() functions
   inline std::string space( size_t n )
   {
      return std::string( n, ' ' );
   }

   /// @brief Convert number to decimal string
   template <class T> std::string toString( T x )
   {
      static_assert( std::is_integral<T>::value || std::is_enum<T>::value ||
                        std::is_floating_point<T>::value,
                     "Numeric type required." );

      std::ostringstream ss;
      ss << x;
      return ss.str();
   }

   /// @overload
   inline std::string hexString( uint8_t x )
   {
      std::ostringstream ss;
      ss << "0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast<unsigned>( x );
      return ss.str();
   }

   /// @overload
   inline std::string hexString( uint16_t x )
   {
      std::ostringstream ss;
      ss << "0x" << std::hex << std::setw( 4 ) << std::setfill( '0' ) << x;
      return ss.str();
   }

   /// @overload
   inline std::string hexString( uint32_t x )
   {
      std::ostringstream ss;
      ss << "0x" << std::hex << std::setw( 8 ) << std::setfill( '0' ) << x;
      return ss.str();
   }

   /// @overload
   inline std::string hexString( uint64_t x )
   {
      std::ostringstream ss;
      ss << "0x" << std::hex << std::setw( 16 ) << std::setfill( '0' ) << x;
      return ss.str();
   }

   /// @overload
   inline std::string hexString( int8_t x )
   {
      return hexString( static_cast<uint8_t>( x ) );
   }

   /// @brief Convert number to a hexadecimal strings
   /// @note Hex strings don't have leading zeros.
   inline std::string hexString( int16_t x )
   {
      return hexString( static_cast<uint16_t>( x ) );
   }

   /// @overload
   inline std::string hexString( int32_t x )
   {
      return hexString( static_cast<uint32_t>( x ) );
   }

   /// @overload
   inline std::string hexString( int64_t x )
   {
      return hexString( static_cast<uint64_t>( x ) );
   }

   /// @brief Convert number to a binary string
   inline std::string binaryString( uint8_t x )
   {
      std::ostringstream ss;
      for ( int i = 7; i >= 0; i-- )
      {
         ss << ( ( x & ( 1LL << i ) ) ? 1 : 0 );
         if ( i > 0 && i % 8 == 0 )
         {
            ss << " ";
         }
      }
      return ss.str();
   }

   /// @overload
   inline std::string binaryString( uint16_t x )
   {
      std::ostringstream ss;
      for ( int i = 15; i >= 0; i-- )
      {
         ss << ( ( x & ( 1LL << i ) ) ? 1 : 0 );
         if ( i > 0 && i % 8 == 0 )
         {
            ss << " ";
         }
      }
      return ss.str();
   }

   /// @overload
   inline std::string binaryString( uint32_t x )
   {
      std::ostringstream ss;
      for ( int i = 31; i >= 0; i-- )
      {
         ss << ( ( x & ( 1LL << i ) ) ? 1 : 0 );
         if ( i > 0 && i % 8 == 0 )
         {
            ss << " ";
         }
      }
      return ss.str();
   }

   /// @overload
   inline std::string binaryString( uint64_t x )
   {
      std::ostringstream ss;
      for ( int i = 63; i >= 0; i-- )
      {
         ss << ( ( x & ( 1LL << i ) ) ? 1 : 0 );
         if ( i > 0 && i % 8 == 0 )
         {
            ss << " ";
         }
      }
      return ss.str();
   }

   /// @overload
   inline std::string binaryString( int8_t x )
   {
      return binaryString( static_cast<uint8_t>( x ) );
   }

   /// @overload
   inline std::string binaryString( int16_t x )
   {
      return binaryString( static_cast<uint16_t>( x ) );
   }

   /// @overload
   inline std::string binaryString( int32_t x )
   {
      return binaryString( static_cast<uint32_t>( x ) );
   }

   /// @overload
   inline std::string binaryString( int64_t x )
   {
      return binaryString( static_cast<uint64_t>( x ) );
   }

   /// @brief Convert a floating point number to a string and do some clean up of the string.
   template <class FTYPE> std::string floatingPointToStr( FTYPE value, int precision );

   extern template std::string floatingPointToStr<float>( float value, int precision );
   extern template std::string floatingPointToStr<double>( double value, int precision );

   /// Parse a double according the the classic ("C") locale.
   /// @return The parsed double or 0.0 on error.
   double strToDouble( const std::string &inStr );
}
