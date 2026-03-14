#pragma once

#include <memory>

#include "Common.h"

namespace e57
{
   class CheckedFile;

   struct E57FileHeader;
   struct NameSpace;

   class ImageFileImpl : public std::enable_shared_from_this<ImageFileImpl>
   {
   public:
      explicit ImageFileImpl( ReadChecksumPolicy policy );

      void construct2( const ustring &fileName, const ustring &mode );
      void construct2( const char *input, uint64_t size );

      std::shared_ptr<StructureNodeImpl> root();

      void close();
      void cancel();
      bool isOpen() const;
      bool isWriter() const;
      int writerCount() const;
      int readerCount() const;
      ~ImageFileImpl();

      uint64_t allocateSpace( uint64_t byteCount, bool doExtendNow );
      CheckedFile *file() const;
      ustring fileName() const;

      /// Manipulate registered extensions in the file
      void extensionsAdd( const ustring &prefix, const ustring &uri );
      bool extensionsLookupPrefix( const ustring &prefix, ustring &uri ) const;
      bool extensionsLookupUri( const ustring &uri, ustring &prefix ) const;
      size_t extensionsCount() const;
      ustring extensionsPrefix( size_t index ) const;
      ustring extensionsUri( size_t index ) const;

      /// Utility functions:
      bool isElementNameExtended( const ustring &elementName );
      bool isElementNameLegal( const ustring &elementName, bool allowNumber = true );
      bool isPathNameLegal( const ustring &pathName );
      void checkElementNameLegal( const ustring &elementName, bool allowNumber = true );

      void pathNameCheckWellFormed( const ustring &pathName );
      void pathNameParse( const ustring &pathName, bool &isRelative, StringList &fields );

      void incrWriterCount();
      void decrWriterCount();
      void incrReaderCount();
      void decrReaderCount();

      static void elementNameParse( const ustring &elementName, ustring &prefix, ustring &localPart,
                                    bool allowNumber = true );

      static ustring pathNameUnparse( bool isRelative, const StringList &fields );

      static unsigned bitsNeeded( int64_t minimum, int64_t maximum );

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) const;
#endif

   private:
      friend class E57XmlParser;
      friend class BlobNodeImpl;
      friend class CompressedVectorWriterImpl;
      friend class CompressedVectorReaderImpl;

      static void readFileHeader( CheckedFile *file, E57FileHeader &header );

      void checkImageFileOpen( const char *srcFileName, int srcLineNumber,
                               const char *srcFunctionName ) const;

      ustring fileName_;
      bool isWriter_;
      int writerCount_;
      int readerCount_;

      ReadChecksumPolicy checksumPolicy;

      CheckedFile *file_;

      // Read file attributes
      uint64_t xmlLogicalOffset_;
      uint64_t xmlLogicalLength_;

      // Write file attributes
      uint64_t unusedLogicalStart_;

      /// Bidirectional map from namespace prefix to uri
      std::vector<NameSpace> nameSpaces_;

      /// Smart pointer to metadata tree
      std::shared_ptr<StructureNodeImpl> root_;
   };
}
