#include "VectorNodeImpl.h"
#include "CheckedFile.h"
#include "StringFunctions.h"

namespace e57
{
   VectorNodeImpl::VectorNodeImpl( ImageFileImplWeakPtr destImageFile, bool allowHeteroChildren ) :
      StructureNodeImpl( destImageFile ), allowHeteroChildren_( allowHeteroChildren )
   {
      // don't checkImageFileOpen, StructNodeImpl() will do it
   }

   bool VectorNodeImpl::isTypeEquivalent( NodeImplSharedPtr ni )
   {
      // don't checkImageFileOpen

      // Same node type?
      if ( ni->type() != TypeVector )
      {
         return ( false );
      }

      std::shared_ptr<VectorNodeImpl> ai( std::static_pointer_cast<VectorNodeImpl>( ni ) );

      // allowHeteroChildren must match
      if ( allowHeteroChildren_ != ai->allowHeteroChildren_ )
      {
         return ( false );
      }

      // Same number of children?
      if ( childCount() != ai->childCount() )
      {
         return ( false );
      }

      // Check each child, must be in same order
      for ( unsigned i = 0; i < childCount(); i++ )
      {
         if ( !children_.at( i )->isTypeEquivalent( ai->children_.at( i ) ) )
         {
            return ( false );
         }
      }

      // Types match
      return ( true );
   }

   bool VectorNodeImpl::allowHeteroChildren() const
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      return allowHeteroChildren_;
   }

   void VectorNodeImpl::set( int64_t index64, NodeImplSharedPtr ni )
   {
      checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
      if ( !allowHeteroChildren_ )
      {
         // New node type must match all existing children
         for ( auto &child : children_ )
         {
            if ( !child->isTypeEquivalent( ni ) )
            {
               throw E57_EXCEPTION2( ErrorHomogeneousViolation,
                                     "this->pathName=" + this->pathName() );
            }
         }
      }

      //??? for now, use base implementation
      StructureNodeImpl::set( index64, ni );
   }

   void VectorNodeImpl::writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
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

      cf << space( indent ) << "<" << fieldName << " type=\"Vector\" allowHeterogeneousChildren=\""
         << static_cast<int64_t>( allowHeteroChildren_ ) << "\">\n";
      for ( auto &child : children_ )
      {
         child->writeXml( imf, cf, indent + 2, "vectorChild" );
      }
      cf << space( indent ) << "</" << fieldName << ">\n";
   }

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
   void VectorNodeImpl::dump( int indent, std::ostream &os ) const
   {
      // don't checkImageFileOpen
      os << space( indent ) << "type:        Vector" << " (" << type() << ")" << std::endl;
      NodeImpl::dump( indent, os ); // NOLINT(bugprone-parent-virtual-call)
      os << space( indent ) << "allowHeteroChildren: " << allowHeteroChildren() << std::endl;
      for ( unsigned i = 0; i < children_.size(); i++ )
      {
         os << space( indent ) << "child[" << i << "]:" << std::endl;
         children_.at( i )->dump( indent + 2, os );
      }
   }
#endif
}
