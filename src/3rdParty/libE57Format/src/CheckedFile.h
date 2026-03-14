#pragma once

#include <algorithm>

#include "Common.h"

namespace e57
{
   // Tool class to read buffer efficiently without
   // multiplying copy operations.
   //
   // WARNING: pointer input is handled by user!
   class BufferView;

   class CheckedFile
   {
   public:
      // physical page size is 2 raised to this power
      static constexpr size_t physicalPageSizeLog2 = 10;
      static constexpr size_t physicalPageSize = 1 << physicalPageSizeLog2;
      static constexpr uint64_t physicalPageSizeMask = physicalPageSize - 1;
      static constexpr size_t logicalPageSize = physicalPageSize - 4;

   public:
      enum Mode
      {
         Read,
         Write,
      };

      enum OffsetMode
      {
         Logical,
         Physical
      };

      CheckedFile( const e57::ustring &fileName, Mode mode, ReadChecksumPolicy policy );
      CheckedFile( const char *input, uint64_t size, ReadChecksumPolicy policy );
      ~CheckedFile();

      void read( char *buf, size_t nRead, size_t bufSize = 0 );
      void write( const char *buf, size_t nWrite );
      CheckedFile &operator<<( const e57::ustring &s );
      CheckedFile &operator<<( int64_t i );
      CheckedFile &operator<<( uint64_t i );
      CheckedFile &operator<<( float f );
      CheckedFile &operator<<( double d );
      void seek( uint64_t offset, OffsetMode omode = Logical );
      uint64_t position( OffsetMode omode = Logical );
      uint64_t length( OffsetMode omode = Logical );
      void extend( uint64_t newLength, OffsetMode omode = Logical );

      e57::ustring fileName() const
      {
         return fileName_;
      }

      void close();
      void unlink();

      static inline uint64_t logicalToPhysical( uint64_t logicalOffset );
      static inline uint64_t physicalToLogical( uint64_t physicalOffset );

   private:
      void verifyChecksum( char *page_buffer, uint64_t page );

      template <class FTYPE> CheckedFile &writeFloatingPoint( FTYPE value, int precision );

      void getCurrentPageAndOffset( uint64_t &page, size_t &pageOffset,
                                    OffsetMode omode = Logical );
      void readPhysicalPage( char *page_buffer, uint64_t page );
      void writePhysicalPage( char *page_buffer, uint64_t page );
      int open64( const e57::ustring &fileName, int flags, int mode );
      uint64_t lseek64( int64_t offset, int whence );

      e57::ustring fileName_;
      uint64_t logicalLength_ = 0;
      uint64_t physicalLength_ = 0;

      ReadChecksumPolicy checkSumPolicy_ = ChecksumPolicy::ChecksumAll;

      int fd_ = -1;
      BufferView *bufView_ = nullptr;
      bool readOnly_ = false;
   };

   inline uint64_t CheckedFile::logicalToPhysical( uint64_t logicalOffset )
   {
      const uint64_t page = logicalOffset / logicalPageSize;
      const uint64_t remainder = logicalOffset - page * logicalPageSize;

      return page * physicalPageSize + remainder;
   }

   inline uint64_t CheckedFile::physicalToLogical( uint64_t physicalOffset )
   {
      const uint64_t page = physicalOffset >> physicalPageSizeLog2;
      const auto remainder = static_cast<size_t>( physicalOffset & physicalPageSizeMask );

      return page * logicalPageSize + std::min( remainder, logicalPageSize );
   }
}
