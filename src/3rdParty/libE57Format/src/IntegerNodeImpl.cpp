#include "IntegerNodeImpl.h"
#include "CheckedFile.h"
#include "StringFunctions.h"

namespace e57
{
   IntegerNodeImpl::IntegerNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t value,
                                     int64_t minimum, int64_t maximum ) :
      NodeImpl( destImageFile ), value_( value ), minimum_( minimum ), maximum_( maximum )
   {
   }

   // Throw an exception if the value is not within bounds.
   void IntegerNodeImpl::validateValue() const
   {
      if ( value_ < minimum_ || value_ > maximum_ )
      {
         throw E57_EXCEPTION2( ErrorValueOutOfBounds, "this->pathName=" + this->pathName() +
                                                         " value=" + toString( value_ ) +
                                                         " minimum=" + toString( minimum_ ) +
                                                         " maximum=" + toString( maximum_ ) );
      }
   }

   bool IntegerNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
   {
      // don't checkImageFileOpen

      // Same node type?
      if ( ni->type() != TypeInteger )
      {
         return ( false );
      }

      // Downcast to shared_ptr<IntegerNodeImpl>
      std::shared_ptr<IntegerNodeImpl> ii( std::static_pointer_cast<IntegerNodeImpl>( ni ) );

      // minimum must match
      if ( minimum_ != ii->minimum_ )
      {
         return ( false );
      }

      // maximum must match
      if ( maximum_ != ii->maximum_ )
      {
         return ( false );
      }

      // ignore value_, doesn't have to match

      // Types match
      return ( true );
   }

   bool IntegerNodeImpl::isDefined( const ustring &pathName )
   {
      // don't checkImageFileOpen

      // We have no sub-structure, so if path not empty return false
      return pathName.empty();
   }

   int64_t IntegerNodeImpl::value()
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return ( value_ );
   }

   int64_t IntegerNodeImpl::minimum()
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return ( minimum_ );
   }

   int64_t IntegerNodeImpl::maximum()
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return ( maximum_ );
   }

   void IntegerNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
   {
      // don't checkImageFileOpen

      // We are a leaf node, so verify that we are listed in set.
      if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
      {
         throw E57_EXCEPTION2( ErrorNoBufferForElement, "this->pathName=" + this->pathName() );
      }
   }

   void IntegerNodeImpl::writeXml( ImageFileImplSharedPtr /*imf???*/, CheckedFile &cf, int indent,
                                   const char *forcedFieldName )
   {
      // don't checkImageFileOpen

      ustring fieldName;
      if ( forcedFieldName != nullptr )
      {
         fieldName = forcedFieldName;
      }
      else
      {
         fieldName = elementName_;
      }

      cf << space( indent ) << "<" << fieldName << " type=\"Integer\"";

      // Don't need to write if are default values
      if ( minimum_ != INT64_MIN )
      {
         cf << " minimum=\"" << minimum_ << "\"";
      }
      if ( maximum_ != INT64_MAX )
      {
         cf << " maximum=\"" << maximum_ << "\"";
      }

      // Write value as child text, unless it is the default value
      if ( value_ != 0 )
      {
         cf << ">" << value_ << "</" << fieldName << ">\n";
      }
      else
      {
         cf << "/>\n";
      }
   }

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
   void IntegerNodeImpl::dump( int indent, std::ostream &os ) const
   {
      // don't checkImageFileOpen
      os << space( indent ) << "type:        Integer" << " (" << type() << ")" << std::endl;
      NodeImpl::dump( indent, os );
      os << space( indent ) << "value:       " << value_ << std::endl;
      os << space( indent ) << "minimum:     " << minimum_ << std::endl;
      os << space( indent ) << "maximum:     " << maximum_ << std::endl;
   }
#endif
}
