#include <algorithm>
#include <limits>

#include "E57SimpleWriter.h"
#include "WriterImpl.h"

namespace
{
   /// Fill in missing min/max data in the Data3D header for the following:
   ///   - cartesian points
   ///   - spherical points
   ///   - time stamps
   template <typename COORDTYPE>
   void _fillMinMaxData( e57::Data3D &ioData3DHeader,
                         const e57::Data3DPointsData_t<COORDTYPE> &inBuffers )
   {
      static_assert( std::is_floating_point<COORDTYPE>::value, "Floating point type required." );

      auto &pointFields = ioData3DHeader.pointFields;

      constexpr COORDTYPE cMin = std::numeric_limits<COORDTYPE>::lowest();
      constexpr COORDTYPE cMax = std::numeric_limits<COORDTYPE>::max();

      // IF we are using scaled ints for cartesian points
      // AND we haven't set either min or max
      // THEN calculate them from the points
      auto pointRangeMinimum = cMax;
      auto pointRangeMaximum = cMin;

      const bool writePointRange =
         ( pointFields.pointRangeNodeType == e57::NumericalNodeType::ScaledInteger ) &&
         ( pointFields.pointRangeMinimum == cMin ) && ( pointFields.pointRangeMaximum == cMax );

      // IF we are using scaled ints for spherical angles
      // AND we haven't set either min or max
      // THEN calculate them from the points
      auto angleMinimum = cMax;
      auto angleMaximum = cMin;

      const bool writeAngle =
         ( pointFields.angleNodeType == e57::NumericalNodeType::ScaledInteger ) &&
         ( pointFields.angleMinimum == cMin ) && ( pointFields.angleMaximum == cMax );

      // IF we are using scaled ints for timestamps
      // AND we haven't set either min or max
      // THEN calculate them from the points
      double timeMinimum = std::numeric_limits<double>::max();
      double timeMaximum = std::numeric_limits<double>::lowest();

      const bool writeTimeStamp =
         pointFields.timeStampField &&
         ( pointFields.timeNodeType == e57::NumericalNodeType::ScaledInteger ) &&
         ( pointFields.timeMinimum == cMin ) && ( pointFields.timeMaximum == cMax );

      // Now run through the points and set the things we need to
      for ( size_t i = 0; i < ioData3DHeader.pointCount; ++i )
      {
         if ( writePointRange && pointFields.cartesianXField )
         {
            pointRangeMinimum = std::min( inBuffers.cartesianX[i], pointRangeMinimum );
            pointRangeMinimum = std::min( inBuffers.cartesianY[i], pointRangeMinimum );
            pointRangeMinimum = std::min( inBuffers.cartesianZ[i], pointRangeMinimum );

            pointRangeMaximum = std::max( inBuffers.cartesianX[i], pointRangeMaximum );
            pointRangeMaximum = std::max( inBuffers.cartesianY[i], pointRangeMaximum );
            pointRangeMaximum = std::max( inBuffers.cartesianZ[i], pointRangeMaximum );
         }

         if ( writePointRange && pointFields.sphericalRangeField )
         {
            // Note that the writer code uses pointRangeMinimum/pointRangeMaximum
            // (see WriterImpl::NewData3D()) instead of using the sphericalBounds which has
            // rangeMinimum and rangeMaximum.
            pointRangeMinimum = std::min( inBuffers.sphericalRange[i], pointRangeMinimum );
            pointRangeMaximum = std::max( inBuffers.sphericalRange[i], pointRangeMaximum );
         }

         if ( writeAngle )
         {
            angleMinimum = std::min( inBuffers.sphericalAzimuth[i], angleMinimum );
            angleMinimum = std::min( inBuffers.sphericalElevation[i], angleMinimum );

            angleMaximum = std::max( inBuffers.sphericalAzimuth[i], angleMaximum );
            angleMaximum = std::max( inBuffers.sphericalElevation[i], angleMaximum );
         }

         if ( writeTimeStamp )
         {
            timeMinimum = std::min( inBuffers.timeStamp[i], timeMinimum );
            timeMaximum = std::max( inBuffers.timeStamp[i], timeMaximum );
         }
      }

      if ( writePointRange )
      {
         pointFields.pointRangeMinimum = pointRangeMinimum;
         pointFields.pointRangeMaximum = pointRangeMaximum;
      }

      if ( writeAngle )
      {
         pointFields.angleMinimum = angleMinimum;
         pointFields.angleMaximum = angleMaximum;
      }

      if ( writeTimeStamp )
      {
         pointFields.timeMinimum = timeMinimum;
         pointFields.timeMaximum = timeMaximum;
      }
   }
   template void _fillMinMaxData( e57::Data3D &ioData3DHeader,
                                  const e57::Data3DPointsFloat &inBuffers );
   template void _fillMinMaxData( e57::Data3D &ioData3DHeader,
                                  const e57::Data3DPointsDouble &inBuffers );
}

namespace e57
{
   Writer::Writer( const ustring &filePath, const WriterOptions &options ) :
      impl_( new WriterImpl( filePath, options ) )
   {
   }

   // Note that this constructor is deprecated (see header).
   Writer::Writer( const ustring &filePath, const ustring &coordinateMetadata ) :
      Writer( filePath, WriterOptions{ {}, coordinateMetadata } )
   {
   }

   bool Writer::IsOpen() const
   {
      return impl_->IsOpen();
   }

   bool Writer::Close()
   {
      return impl_->Close();
   }

   int64_t Writer::WriteImage2DData( Image2D &image2DHeader, Image2DType imageType,
                                     Image2DProjection imageProjection, int64_t startPos,
                                     void *pBuffer, int64_t byteCount )
   {
      auto *buffer = static_cast<uint8_t *>( pBuffer );
      const auto sizeInBytes = static_cast<size_t>( byteCount );

      const int64_t imageIndex = impl_->NewImage2D( image2DHeader );

      const size_t written = impl_->WriteImage2DData( imageIndex, imageType, imageProjection,
                                                      buffer, startPos, sizeInBytes );

      return static_cast<int64_t>( written );
   }

   int64_t Writer::NewImage2D( Image2D &image2DHeader )
   {
      return impl_->NewImage2D( image2DHeader );
   }

   int64_t Writer::WriteImage2DData( int64_t imageIndex, Image2DType imageType,
                                     Image2DProjection imageProjection, void *pBuffer,
                                     int64_t start, int64_t count )
   {
      auto *buffer = static_cast<uint8_t *>( pBuffer );
      const auto size = static_cast<size_t>( count );

      const size_t written =
         impl_->WriteImage2DData( imageIndex, imageType, imageProjection, buffer, start, size );

      return static_cast<int64_t>( written );
   }

   int64_t Writer::WriteData3DData( Data3D &data3DHeader, const Data3DPointsFloat &buffers )
   {
      _fillMinMaxData( data3DHeader, buffers );

      const int64_t scanIndex = impl_->NewData3D( data3DHeader );

      e57::CompressedVectorWriter dataWriter =
         impl_->SetUpData3DPointsData( scanIndex, data3DHeader.pointCount, buffers );

      dataWriter.write( data3DHeader.pointCount );
      dataWriter.close();

      return scanIndex;
   }

   int64_t Writer::WriteData3DData( Data3D &data3DHeader, const Data3DPointsDouble &buffers )
   {
      _fillMinMaxData( data3DHeader, buffers );

      const int64_t scanIndex = impl_->NewData3D( data3DHeader );

      e57::CompressedVectorWriter dataWriter =
         impl_->SetUpData3DPointsData( scanIndex, data3DHeader.pointCount, buffers );

      dataWriter.write( data3DHeader.pointCount );
      dataWriter.close();

      return scanIndex;
   }

   int64_t Writer::NewData3D( Data3D &data3DHeader )
   {
      return impl_->NewData3D( data3DHeader );
   }

   CompressedVectorWriter Writer::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                         const Data3DPointsFloat &buffers )
   {
      return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
   }

   CompressedVectorWriter Writer::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                         const Data3DPointsDouble &buffers )
   {
      return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
   }

   bool Writer::WriteData3DGroupsData( int64_t dataIndex, size_t groupCount,
                                       int64_t *idElementValue, int64_t *startPointIndex,
                                       int64_t *pointCount )
   {
      return impl_->WriteData3DGroupsData( dataIndex, groupCount, idElementValue, startPointIndex,
                                           pointCount );
   }

   ImageFile Writer::GetRawIMF()
   {
      return impl_->GetRawIMF();
   }

   StructureNode Writer::GetRawE57Root()
   {
      return impl_->GetRawE57Root();
   }

   VectorNode Writer::GetRawData3D()
   {
      return impl_->GetRawData3D();
   }

   VectorNode Writer::GetRawImages2D()
   {
      return impl_->GetRawImages2D();
   }
} // end namespace e57
