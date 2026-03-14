#pragma once

#include "Common.h"

namespace e57
{
   class Decoder
   {
   public:
      static std::shared_ptr<Decoder> DecoderFactory( unsigned bytestreamNumber,
                                                      const CompressedVectorNodeImpl *cVector,
                                                      std::vector<SourceDestBuffer> &dbufs,
                                                      const ustring &codecPath );
      Decoder() = delete;
      virtual ~Decoder() = default;

      virtual void destBufferSetNew( std::vector<SourceDestBuffer> &dbufs ) = 0;
      virtual uint64_t totalRecordsCompleted() = 0;
      virtual size_t inputProcess( const char *source, size_t count ) = 0;
      virtual void stateReset() = 0;

      unsigned bytestreamNumber() const
      {
         return bytestreamNumber_;
      }

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      virtual void dump( int indent = 0, std::ostream &os = std::cout ) = 0;
#endif

   protected:
      explicit Decoder( unsigned bytestreamNumber );

      unsigned int bytestreamNumber_;
   };

   class BitpackDecoder : public Decoder
   {
   public:
      void destBufferSetNew( std::vector<SourceDestBuffer> &dbufs ) override;

      uint64_t totalRecordsCompleted() override
      {
         return ( currentRecordIndex_ );
      }

      size_t inputProcess( const char *source, size_t availableByteCount ) override;
      virtual size_t inputProcessAligned( const char *inbuf, size_t firstBit, size_t endBit ) = 0;

      void stateReset() override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) override;
#endif

   protected:
      BitpackDecoder( unsigned bytestreamNumber, SourceDestBuffer &dbuf, unsigned alignmentSize,
                      uint64_t maxRecordCount );

      void inBufferShiftDown();

      uint64_t currentRecordIndex_ = 0;
      uint64_t maxRecordCount_ = 0;

      std::shared_ptr<SourceDestBufferImpl> destBuffer_;

      std::vector<char> inBuffer_;
      size_t inBufferFirstBit_ = 0;
      size_t inBufferEndByte_ = 0;
      unsigned int inBufferAlignmentSize_;
      unsigned int bitsPerWord_;
      unsigned int bytesPerWord_;
   };

   class BitpackFloatDecoder : public BitpackDecoder
   {
   public:
      BitpackFloatDecoder( unsigned bytestreamNumber, SourceDestBuffer &dbuf,
                           FloatPrecision precision, uint64_t maxRecordCount );

      size_t inputProcessAligned( const char *inbuf, size_t firstBit, size_t endBit ) override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) override;
#endif

   protected:
      FloatPrecision precision_ = PrecisionSingle;
   };

   class BitpackStringDecoder : public BitpackDecoder
   {
   public:
      BitpackStringDecoder( unsigned bytestreamNumber, SourceDestBuffer &dbuf,
                            uint64_t maxRecordCount );

      size_t inputProcessAligned( const char *inbuf, size_t firstBit, size_t endBit ) override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) override;
#endif

   protected:
      bool readingPrefix_ = true;
      int prefixLength_ = 1;
      uint8_t prefixBytes_[8] = {};
      int nBytesPrefixRead_ = 0;
      uint64_t stringLength_ = 0;
      ustring currentString_;
      uint64_t nBytesStringRead_ = 0;
   };

   template <typename RegisterT> class BitpackIntegerDecoder : public BitpackDecoder
   {
   public:
      BitpackIntegerDecoder( bool isScaledInteger, unsigned bytestreamNumber,
                             SourceDestBuffer &dbuf, int64_t minimum, int64_t maximum, double scale,
                             double offset, uint64_t maxRecordCount );

      size_t inputProcessAligned( const char *inbuf, size_t firstBit, size_t endBit ) override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) override;
#endif

   protected:
      bool isScaledInteger_;
      int64_t minimum_;
      int64_t maximum_;
      double scale_;
      double offset_;
      unsigned bitsPerRecord_;
      RegisterT destBitMask_;
      static constexpr size_t RegisterBits = sizeof( RegisterT ) * 8;
   };

   class ConstantIntegerDecoder : public Decoder
   {
   public:
      ConstantIntegerDecoder( bool isScaledInteger, unsigned bytestreamNumber,
                              SourceDestBuffer &dbuf, int64_t minimum, double scale, double offset,
                              uint64_t maxRecordCount );
      void destBufferSetNew( std::vector<SourceDestBuffer> &dbufs ) override;

      uint64_t totalRecordsCompleted() override
      {
         return currentRecordIndex_;
      }

      size_t inputProcess( const char *source, size_t availableByteCount ) override;
      void stateReset() override;

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
      void dump( int indent = 0, std::ostream &os = std::cout ) override;
#endif

   protected:
      uint64_t currentRecordIndex_ = 0;
      uint64_t maxRecordCount_;

      std::shared_ptr<SourceDestBufferImpl> destBuffer_;

      bool isScaledInteger_;
      int64_t minimum_;
      double scale_;
      double offset_;
   };
}
