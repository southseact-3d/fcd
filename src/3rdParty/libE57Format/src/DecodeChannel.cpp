#include "DecodeChannel.h"
#include "SourceDestBufferImpl.h"
#include "StringFunctions.h"

namespace e57
{
   DecodeChannel::DecodeChannel( SourceDestBuffer dbuf_arg, std::shared_ptr<Decoder> decoder_arg,
                                 unsigned bytestreamNumber_arg, uint64_t maxRecordCount_arg ) :
      dbuf( dbuf_arg ), decoder( decoder_arg ), bytestreamNumber( bytestreamNumber_arg )
   {
      maxRecordCount = maxRecordCount_arg;
      currentPacketLogicalOffset = 0;
      currentBytestreamBufferIndex = 0;
      currentBytestreamBufferLength = 0;
      inputFinished = false;
   }

   bool DecodeChannel::isOutputBlocked() const
   {
      // If we have completed the entire vector, we are done
      if ( decoder->totalRecordsCompleted() >= maxRecordCount )
      {
         return ( true );
      }

      // If we have filled the dest buffer, we are blocked
      return ( dbuf.impl()->nextIndex() == dbuf.impl()->capacity() );
   }

   bool DecodeChannel::isInputBlocked() const
   {
      // If have read until the section end, we are done
      if ( inputFinished )
      {
         return ( true );
      }

      // If have eaten all the input in the current packet, we are blocked.
      return ( currentBytestreamBufferIndex == currentBytestreamBufferLength );
   }

#ifdef E57_ENABLE_DIAGNOSTIC_OUTPUT
   void DecodeChannel::dump( int indent, std::ostream &os ) const
   {
      os << space( indent ) << "dbuf" << std::endl;
      dbuf.dump( indent + 4, os );

      os << space( indent ) << "decoder:" << std::endl;
      decoder->dump( indent + 4, os );

      os << space( indent ) << "bytestreamNumber:              " << bytestreamNumber << std::endl;
      os << space( indent ) << "maxRecordCount:                " << maxRecordCount << std::endl;
      os << space( indent ) << "currentPacketLogicalOffset:    " << currentPacketLogicalOffset
         << std::endl;
      os << space( indent ) << "currentBytestreamBufferIndex:  " << currentBytestreamBufferIndex
         << std::endl;
      os << space( indent ) << "currentBytestreamBufferLength: " << currentBytestreamBufferLength
         << std::endl;
      os << space( indent ) << "inputFinished:                 " << inputFinished << std::endl;
      os << space( indent ) << "isInputBlocked():              " << isInputBlocked() << std::endl;
      os << space( indent ) << "isOutputBlocked():             " << isOutputBlocked() << std::endl;
   }
#endif
}
