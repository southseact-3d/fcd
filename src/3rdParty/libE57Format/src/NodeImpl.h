#pragma once

#include "Common.h"

namespace e57
{
   class CheckedFile;

   class NodeImpl : public std::enable_shared_from_this<NodeImpl>
   {
   public:
      virtual NodeType type() const = 0;
      void checkImageFileOpen( const char *srcFileName, int srcLineNumber,
                               const char *srcFunctionName ) const;
      virtual bool isTypeEquivalent( NodeImplSharedPtr ni ) = 0;

      bool isRoot() const;
      NodeImplSharedPtr parent();
      ustring pathName() const;
      ustring relativePathName( const NodeImplSharedPtr &origin,
                                ustring childPathName = ustring() ) const;
      ustring elementName() const;
      ImageFileImplSharedPtr destImageFile();

      ustring imageFileName() const;
      virtual bool isDefined( const ustring &pathName ) = 0;
      bool isAttached() const;
      virtual void setAttachedRecursive();

      void setParent( NodeImplSharedPtr parent, const ustring &elementName );
      bool isTypeConstrained();

      virtual NodeImplSharedPtr get( const ustring &pathName );
      virtual void set( const ustring &pathName, NodeImplSharedPtr ni,
                        bool autoPathCreate = false );
      virtual void set( const StringList &fields, unsigned level, NodeImplSharedPtr ni,
                        bool autoPathCreate = false );

      virtual void checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin ) = 0;
      void checkBuffers( const std::vector<SourceDestBuffer> &sdbufs, bool allowMissing );
      bool findTerminalPosition( const NodeImplSharedPtr &target, uint64_t &countFromLeft );

      virtual void writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
                             const char *forcedFieldName = nullptr ) = 0;

      virtual ~NodeImpl() = default;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      virtual void dump( int indent = 0, std::ostream &os = std::cout ) const;
#endif

   private:
#ifdef VALIDATE_BASIC
      bool _verifyPathNameAbsolute( const ustring &inPathName );
#endif

      NodeImplSharedPtr _verifyAndGetRoot();

   protected:
      friend class StructureNodeImpl;
      friend class CompressedVectorWriterImpl;
      friend class Decoder;
      friend class Encoder;

      explicit NodeImpl( ImageFileImplWeakPtr destImageFile );

      virtual NodeImplSharedPtr lookup( const ustring & /*pathName*/ )
      {
         return {};
      }

      NodeImplSharedPtr getRoot();

      ImageFileImplWeakPtr destImageFile_;
      NodeImplWeakPtr parent_;
      ustring elementName_;
      bool isAttached_;
   };
}
