#pragma once

/// @file
/// @brief E57 Simple API for writing E57.
/// @details This includes support for the
/// [E57_EXT_surface_normals](http://www.libe57.org/E57_EXT_surface_normals.txt) extension.

#include "E57SimpleData.h"

namespace e57
{
   /// Options to the Writer constructor
   struct E57_DLL WriterOptions
   {
      /// Optional file guid
      ustring guid;

      /// Information describing the Coordinate Reference System to be used for the file
      ustring coordinateMetadata;
   };

   /// @brief Used for writing an E57 file using the E57 Simple API.
   ///
   /// The Writer includes support for the
   /// [E57_EXT_surface_normals](http://www.libe57.org/E57_EXT_surface_normals.txt) extension.
   class E57_DLL Writer
   {
   public:
      /// @brief Writer constructor
      /// @param [in] filePath Path to E57 file
      /// @param [in] options Options to be used for the file
      Writer( const ustring &filePath, const WriterOptions &options );

      /// @brief Writer constructor (deprecated)
      /// @param [in] filePath Path to E57 file
      /// @param [in] coordinateMetadata Information describing the Coordinate Reference System to
      /// be used for the file
      /// @deprecated Will be removed in 4.0. Use Writer( const ustring &filePath, const
      /// WriterOptions &options ) instead.
      [[deprecated(
         "Will be removed in 4.0. Use Writer( ustring, WriterOptions )." )]] // TODO Remove in 4.0
      explicit Writer( const ustring &filePath, const ustring &coordinateMetadata = {} );

      /// @brief Returns true if the file is open
      bool IsOpen() const;

      /// @brief Closes the file
      bool Close();

      /// @name Image2D
      ///@{

      /// @brief This function writes the Image2D data to the file
      /// @note @p image2DHeader may be modified (adding a guid or adding missing, required fields).
      /// @param [in,out] image2DHeader header metadata
      /// @param [in] imageType identifies the image format
      /// @param [in] imageProjection identifies the projection
      /// @param [in] startPos position in the block to start writing
      /// @param [in] buffer pointer the data buffer
      /// @param [in] byteCount buffer size
      /// @return Returns the number of bytes written
      int64_t WriteImage2DData( Image2D &image2DHeader, Image2DType imageType,
                                Image2DProjection imageProjection, int64_t startPos, void *buffer,
                                int64_t byteCount );

      /// @brief Writes a new Image2D header
      /// @details The user needs to config a Image2D structure with all the camera information
      /// before making this call.
      /// @param [in,out] image2DHeader header metadata
      /// @return Returns the image2D index
      int64_t NewImage2D( Image2D &image2DHeader );

      /// @brief Writes the actual image data
      /// @param [in] imageIndex picture block index given by the NewImage2D
      /// @param [in] imageType identifies the image format desired.
      /// @param [in] imageProjection identifies the projection desired.
      /// @param [in] buffer pointer the buffer
      /// @param [in] start position in the block to start writing
      /// @param [in] count size of desired chunk or buffer size
      /// @return Returns the number of bytes written
      int64_t WriteImage2DData( int64_t imageIndex, Image2DType imageType,
                                Image2DProjection imageProjection, void *buffer, int64_t start,
                                int64_t count );

      ///@}

      /// @name Data3D
      ///@{

      /// @brief This function writes the Data3D data to the file
      /// @details The user needs to config a Data3D structure with all the scanning information
      /// before making this call.
      /// @note @p data3DHeader may be modified (adding a guid or adding missing, required fields).
      /// @param [in,out] data3DHeader metadata about what is included in the buffers
      /// @param [in] buffers pointers to user-provided buffers containing the actual data
      /// @return Returns the index of the new scan's data3D block.
      int64_t WriteData3DData( Data3D &data3DHeader, const Data3DPointsFloat &buffers );

      /// @overload
      int64_t WriteData3DData( Data3D &data3DHeader, const Data3DPointsDouble &buffers );

      /// @brief Writes a new Data3D header
      /// @details The user needs to config a Data3D structure with all the scanning information
      /// before making this call.
      /// @param [in,out] data3DHeader scan metadata
      /// @return Returns the index of the new scan's data3D block.
      int64_t NewData3D( Data3D &data3DHeader );

      /// @brief Sets up a writer to write the actual scan data
      /// @param [in] dataIndex index returned by NewData3D
      /// @param [in] pointCount Number of points to write (number of elements in each of the
      /// buffers)
      /// @param [in] buffers pointers to user-provided buffers
      /// @return returns a vector writer setup to write the selected scan data
      CompressedVectorWriter SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                    const Data3DPointsFloat &buffers );

      /// @overload
      CompressedVectorWriter SetUpData3DPointsData( int64_t dataIndex, size_t pointCount,
                                                    const Data3DPointsDouble &buffers );

      /// @brief Writes out the group data
      /// @param [in] dataIndex data block index given by the NewData3D
      /// @param [in] groupCount size of each of the buffers given
      /// @param [in] idElementValue buffer with idElementValue indices for this group
      /// @param [in] startPointIndex buffer with starting indices in to the "points" data vector
      /// for the groups
      /// @param [in] pointCount buffer with sizes of the groups given
      /// @return Return true if successful, false otherwise
      bool WriteData3DGroupsData( int64_t dataIndex, size_t groupCount, int64_t *idElementValue,
                                  int64_t *startPointIndex, int64_t *pointCount );

      ///@}

      /// @name File information
      ///@{

      /// @brief Returns the file raw E57Root Structure Node
      StructureNode GetRawE57Root();
      /// @brief Returns the raw Data3D Vector Node
      VectorNode GetRawData3D();
      /// @brief Returns the raw Image2D Vector Node
      VectorNode GetRawImages2D();
      /// @brief Returns the ram ImageFile Node which is need to add enhancements
      ImageFile GetRawIMF();

      ///@}

      /// @cond documentNonPublic The following isn't part of the API, and isn't documented.
   protected:
      friend class WriterImpl;

      E57_INTERNAL_ACCESS( Writer )

   protected:
      std::shared_ptr<WriterImpl> impl_;
      /// @endcond
   }; // end Writer class

} // end namespace e57
