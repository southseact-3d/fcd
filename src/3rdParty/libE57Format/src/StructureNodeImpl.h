#pragma once

#include "NodeImpl.h"

namespace e57
{

   class StructureNodeImpl : public NodeImpl
   {
   public:
      explicit StructureNodeImpl( ImageFileImplWeakPtr destImageFile );
      ~StructureNodeImpl() override = default;

      NodeType type() const override;
      bool isTypeEquivalent( NodeImplSharedPtr ni ) override;
      bool isDefined( const ustring &pathName ) override;
      void setAttachedRecursive() override;

      virtual int64_t childCount() const;

      virtual NodeImplSharedPtr get( int64_t index );
      NodeImplSharedPtr get( const ustring &pathName ) override;

      virtual void set( int64_t index, NodeImplSharedPtr ni );
      void set( const ustring &pathName, NodeImplSharedPtr ni,
                bool autoPathCreate = false ) override;
      void set( const StringList &fields, unsigned level, NodeImplSharedPtr ni,
                bool autoPathCreate = false ) override;
      virtual void append( NodeImplSharedPtr ni );

      void checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin ) override;

      void writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
                     const char *forcedFieldName = nullptr ) override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) const override;
#endif

   protected:
      friend class CompressedVectorReaderImpl;

      NodeImplSharedPtr lookup( const ustring &pathName ) override;

      std::vector<NodeImplSharedPtr> children_;
   };
}
