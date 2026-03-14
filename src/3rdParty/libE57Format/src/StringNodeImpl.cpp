#include "StringNodeImpl.h"
#include "CheckedFile.h"
#include "StringFunctions.h"

namespace e57
{
   StringNodeImpl::StringNodeImpl( ImageFileImplWeakPtr destImageFile, const ustring &value ) :
      NodeImpl( destImageFile ), value_( value )
   {
      // don't checkImageFileOpen, NodeImpl() will do it
   }

   bool StringNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
   {
      // don't checkImageFileOpen

      // Same node type?
      if ( ni->type() != TypeString )
      {
         return ( false );
      }

      // ignore value_, doesn't have to match

      // Types match
      return ( true );
   }

   bool StringNodeImpl::isDefined( const ustring &pathName )
   {
      // don't checkImageFileOpen

      // We have no sub-structure, so if path not empty return false
      return pathName.empty();
   }

   ustring StringNodeImpl::value()
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return ( value_ );
   }

   void StringNodeImpl::checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin )
   {
      // don't checkImageFileOpen

      // We are a leaf node, so verify that we are listed in set.
      if ( pathNames.find( relativePathName( origin ) ) == pathNames.end() )
      {
         throw E57_EXCEPTION2( ErrorNoBufferForElement, "this->pathName=" + this->pathName() );
      }
   }

   void StringNodeImpl::writeXml( ImageFileImplSharedPtr /*imf*/, CheckedFile &cf, int indent,
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

      cf << space( indent ) << "<" << fieldName << " type=\"String\"";

      // Write value as child text, unless it is the default value
      if ( value_.empty() )
      {
         cf << "/>\n";
      }
      else
      {
         cf << "><![CDATA[";

         size_t currentPosition = 0;
         size_t len = value_.length();

         // Loop, searching for occurrences of "]]>", which will be split across two CDATA
         // directives
         while ( currentPosition < len )
         {
            size_t found = value_.find( "]]>", currentPosition );

            if ( found == std::string::npos )
            {
               // Didn't find any more "]]>", so can send the rest.
               cf << value_.substr( currentPosition );
               break;
            }

            // Must output in two pieces, first send up to end of "]]"  (don't send the following
            // ">").
            cf << value_.substr( currentPosition, found - currentPosition + 2 );

            // Then start a new CDATA
            cf << "]]><![CDATA[";

            // Keep looping to send the ">" plus the remaining part of the string
            currentPosition = found + 2;
         }
         cf << "]]></" << fieldName << ">\n";
      }
   }

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
   void StringNodeImpl::dump( int indent, std::ostream &os ) const
   {
      os << space( indent ) << "type:        String" << " (" << type() << ")" << std::endl;
      NodeImpl::dump( indent, os );
      os << space( indent ) << "value:       '" << value_ << "'" << std::endl;
   }
#endif
}
