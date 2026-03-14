#pragma once

#include "Common.h"

namespace e57
{
   class ImageFileImpl;

   class SourceDestBufferImpl : public std::enable_shared_from_this<SourceDestBufferImpl>
   {
   public:
      SourceDestBufferImpl( ImageFileImplWeakPtr destImageFile, const ustring &pathName,
                            size_t capacity, bool doConversion, bool doScaling );

      template <typename T> void setTypeInfo( T *base, size_t stride = sizeof( T ) );

      SourceDestBufferImpl( ImageFileImplWeakPtr destImageFile, const ustring &pathName,
                            StringList *b );

      ImageFileImplWeakPtr destImageFile() const
      {
         return destImageFile_;
      }

      ustring pathName() const
      {
         return pathName_;
      }

      MemoryRepresentation memoryRepresentation() const
      {
         return memoryRepresentation_;
      }

      void *base() const
      {
         return base_;
      }

      StringList *ustrings() const
      {
         return ustrings_;
      }

      bool doConversion() const
      {
         return doConversion_;
      }

      bool doScaling() const
      {
         return doScaling_;
      }

      size_t stride() const
      {
         return stride_;
      }

      size_t capacity() const
      {
         return capacity_;
      }

      unsigned nextIndex() const
      {
         return nextIndex_;
      }

      void rewind()
      {
         nextIndex_ = 0;
      }

      int64_t getNextInt64();
      int64_t getNextInt64( double scale, double offset );
      float getNextFloat();
      double getNextDouble();
      ustring getNextString();
      void setNextInt64( int64_t value );
      void setNextInt64( int64_t value, double scale, double offset );
      void setNextFloat( float value );
      void setNextDouble( double value );
      void setNextString( const ustring &value );

      void checkCompatible( const std::shared_ptr<SourceDestBufferImpl> &newBuf ) const;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout );
#endif

   private:
      template <typename T> void _setNextReal( T inValue );

      /// Common routine to check that constructor arguments were ok, throws if not
      void checkState_() const;

      ImageFileImplWeakPtr destImageFile_;

      /// Pathname from CompressedVectorNode to source/dest  object, e.g. "Indices/0"
      ustring pathName_;

      /// Type of element (e.g. ::Int8, ::UIin64, ::Real64...)
      MemoryRepresentation memoryRepresentation_;

      /// Address of first element, for non-ustring buffers
      char *base_ = nullptr;

      /// Total number of elements in array
      size_t capacity_ = 0;

      /// Convert memory representation to/from disk representation
      bool doConversion_ = false;

      /// Apply scale factor for integer type
      bool doScaling_ = false;

      /// Distance between each element (different from size_ if elements not contiguous)
      size_t stride_ = 0;

      /// Number of elements that have been set (dest buffer) or read (source buffer) since
      /// rewind().
      unsigned nextIndex_ = 0;

      /// Optional array of ustrings (used if memoryRepresentation_ == ::UString)
      StringList *ustrings_ = nullptr;
   };
}
