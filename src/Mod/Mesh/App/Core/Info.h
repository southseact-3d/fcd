#ifndef MESH_INFO_H
#define MESH_INFO_H

#include "MeshKernel.h"

namespace MeshCore
{

class MeshKernel;

/**
 * Determines information about the mesh data structure.
 */
class MeshExport MeshInfo
{
public:
    explicit MeshInfo(const MeshKernel& rclM);
    virtual ~MeshInfo() = default;
    /**
     * Writes general information about the mesh structure into the stream.
     */
    std::ostream& GeneralInformation(std::ostream& rclStream) const;
    /**
     * Writes detailed information about the mesh structure into the stream.
     */
    std::ostream& DetailedInformation(std::ostream& rclStream) const;
    /**
     * Writes internal information about the mesh structure into the stream.
     */
    std::ostream& InternalInformation(std::ostream& rclStream) const;
    /**
     * Writes topological information about the mesh structure into the stream.
     */
    std::ostream& TopologyInformation(std::ostream& rclStream) const;

protected:
    /**
     * Writes detailed point information.
     */
    std::ostream& DetailedPointInfo(std::ostream& rclStream) const;
    /**
     * Writes detailed edge information.
     */
    std::ostream& DetailedEdgeInfo(std::ostream& rclStream) const;
    /**
     * Writes detailed facet information.
     */
    std::ostream& DetailedFacetInfo(std::ostream& rclStream) const;
    /**
     * Writes internal point information.
     */
    std::ostream& InternalPointInfo(std::ostream& rclStream) const;
    /**
     * Writes internal facet information.
     */
    std::ostream& InternalFacetInfo(std::ostream& rclStream) const;


private:
    const MeshKernel& _rclMesh;  // const reference to mesh data structure

public:
    MeshInfo() = delete;  // not accessible default constructor
    MeshInfo(const MeshInfo&) = delete;
    MeshInfo(MeshInfo&&) = delete;
    MeshInfo& operator=(const MeshInfo&) = delete;
    MeshInfo& operator=(MeshInfo&&) = delete;
};


}  // namespace MeshCore

#endif
