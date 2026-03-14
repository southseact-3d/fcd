#pragma once

#include "E57SimpleData.h"
#include "E57SimpleWriter.h"

namespace e57
{
   class WriterImpl
   {
   public:
      WriterImpl( const ustring &filePath, const WriterOptions &options );
      ~WriterImpl();

      // disallow copying a WriterImpl
      WriterImpl( const WriterImpl & ) = delete;
      WriterImpl &operator=( WriterImpl const & ) = delete;
      WriterImpl( const WriterImpl && ) = delete;
      WriterImpl &operator=( const WriterImpl && ) = delete;

      bool IsOpen() const;

      bool Close();

      int64_t NewImage2D( Image2D &image2DHeader );

      size_t WriteImage2DData( int64_t imageIndex, Image2DType imageType,
                               Image2DProjection imageProjection, uint8_t *pBuffer, int64_t start,
                               size_t count );

      int64_t NewData3D( Data3D &data3DHeader );

      template <typename COORDTYPE>
      CompressedVectorWriter SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                    const Data3DPointsData_t<COORDTYPE> &buffers );

      bool WriteData3DGroupsData( int64_t dataIndex, size_t groupCount, int64_t *idElementValue,
                                  int64_t *startPointIndex, int64_t *pointCount );

      StructureNode GetRawE57Root();

      VectorNode GetRawData3D();

      VectorNode GetRawImages2D();

      ImageFile GetRawIMF();

   private:
      ImageFile imf_;
      StructureNode root_;

      VectorNode data3D_;

      VectorNode images2D_;
   }; // end Writer class
} // end namespace e57
