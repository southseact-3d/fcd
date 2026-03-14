#include "E57SimpleReader.h"
#include "ReaderImpl.h"

namespace e57
{
   Reader::Reader( const ustring &filePath, const ReaderOptions &options ) :
      impl_( new ReaderImpl( filePath, options ) )
   {
   }

   // Note that this constructor is deprecated (see header).
   Reader::Reader( const ustring &filePath ) : Reader( filePath, {} )
   {
   }

   bool Reader::IsOpen() const
   {
      return impl_->IsOpen();
   }

   bool Reader::Close()
   {
      return impl_->Close();
   }

   bool Reader::GetE57Root( E57Root &fileHeader ) const
   {
      return impl_->GetE57Root( fileHeader );
   }

   int64_t Reader::GetImage2DCount() const
   {
      return impl_->GetImage2DCount();
   }

   bool Reader::ReadImage2D( int64_t imageIndex, Image2D &image2DHeader ) const
   {
      return impl_->ReadImage2D( imageIndex, image2DHeader );
   }

   bool Reader::GetImage2DSizes( int64_t imageIndex, Image2DProjection &imageProjection,
                                 Image2DType &imageType, int64_t &imageWidth, int64_t &imageHeight,
                                 int64_t &imageSize, Image2DType &imageMaskType,
                                 Image2DType &imageVisualType ) const
   {
      return impl_->GetImage2DSizes( imageIndex, imageProjection, imageType, imageWidth,
                                     imageHeight, imageSize, imageMaskType, imageVisualType );
   }

   int64_t Reader::ReadImage2DData( int64_t imageIndex, Image2DProjection imageProjection,
                                    Image2DType imageType, void *pBuffer, int64_t start,
                                    int64_t count ) const
   {
      auto *buffer = static_cast<uint8_t *>( pBuffer );
      const auto size = static_cast<size_t>( count );

      const size_t read =
         impl_->ReadImage2DData( imageIndex, imageProjection, imageType, buffer, start, size );

      return static_cast<int64_t>( read );
   }

   int64_t Reader::GetData3DCount() const
   {
      return impl_->GetData3DCount();
   }

   ImageFile Reader::GetRawIMF() const
   {
      return impl_->GetRawIMF();
   }

   StructureNode Reader::GetRawE57Root() const
   {
      return impl_->GetRawE57Root();
   }

   VectorNode Reader::GetRawData3D() const
   {
      return impl_->GetRawData3D();
   }

   VectorNode Reader::GetRawImages2D() const
   {
      return impl_->GetRawImages2D();
   }

   bool Reader::ReadData3D( int64_t dataIndex, Data3D &data3DHeader ) const
   {
      return impl_->ReadData3D( dataIndex, data3DHeader );
   }

   bool Reader::GetData3DSizes( int64_t dataIndex, int64_t &rowMax, int64_t &columnMax,
                                int64_t &pointsSize, int64_t &groupsSize, int64_t &countSize,
                                bool &bColumnIndex ) const
   {
      return impl_->GetData3DSizes( dataIndex, rowMax, columnMax, pointsSize, groupsSize, countSize,
                                    bColumnIndex );
   }

   bool Reader::ReadData3DGroupsData( int64_t dataIndex, size_t groupCount, int64_t *idElementValue,
                                      int64_t *startPointIndex, int64_t *pointCount ) const
   {
      return impl_->ReadData3DGroupsData( dataIndex, groupCount, idElementValue, startPointIndex,
                                          pointCount );
   }

   CompressedVectorReader Reader::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                         const Data3DPointsFloat &buffers ) const
   {
      return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
   }

   CompressedVectorReader Reader::SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                         const Data3DPointsDouble &buffers ) const
   {
      return impl_->SetUpData3DPointsData( dataIndex, pointCount, buffers );
   }
} // end namespace e57
