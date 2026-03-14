#ifndef MESH_SEGMENT_H
#define MESH_SEGMENT_H

#include <string>
#include <vector>

#include "Core/Iterator.h"

#include "Facet.h"
#include "Types.h"


namespace Mesh
{

class MeshObject;

class MeshExport Segment
{
public:
    Segment(const MeshObject*, bool mod);
    Segment(const MeshObject*, const std::vector<FacetIndex>& inds, bool mod);
    ~Segment() = default;
    void addIndices(const std::vector<FacetIndex>& inds);
    void removeIndices(const std::vector<FacetIndex>& inds);
    const std::vector<FacetIndex>& getIndices() const;
    bool isEmpty() const
    {
        return _indices.empty();
    }

    Segment(const Segment&);
    Segment(Segment&&);
    Segment& operator=(const Segment&);
    Segment& operator=(Segment&&);
    bool operator==(const Segment&) const;

    void setName(const std::string& n)
    {
        _name = n;
    }
    const std::string& getName() const
    {
        return _name;
    }

    void setColor(const std::string& c)
    {
        _color = c;
    }
    const std::string& getColor() const
    {
        return _color;
    }

    void save(bool on)
    {
        _save = on;
    }
    bool isSaved() const
    {
        return _save;
    }

    // friends
    friend class MeshObject;

private:
    const MeshObject* _mesh;
    std::vector<FacetIndex> _indices;
    std::string _name;
    std::string _color;
    bool _save;
    bool _modifykernel;

public:
    class MeshExport const_facet_iterator
    {
    public:
        const_facet_iterator(const Segment*, std::vector<FacetIndex>::const_iterator);
        const_facet_iterator(const const_facet_iterator& fi);
        const_facet_iterator(const_facet_iterator&& fi);
        ~const_facet_iterator();

        const_facet_iterator& operator=(const const_facet_iterator& fi);
        const_facet_iterator& operator=(const_facet_iterator&& fi);
        const Facet& operator*() const;
        const Facet* operator->() const;
        bool operator==(const const_facet_iterator& fi) const;
        bool operator!=(const const_facet_iterator& fi) const;
        const_facet_iterator& operator++();
        const_facet_iterator& operator--();

    private:
        void dereference() const;
        const Segment* _segment;
        mutable Facet _facet;
        mutable MeshCore::MeshFacetIterator _f_it;
        std::vector<FacetIndex>::const_iterator _it;
    };

    const_facet_iterator facets_begin() const
    {
        return {this, _indices.begin()};
    }
    const_facet_iterator facets_end() const
    {
        return {this, _indices.end()};
    }
};

}  // namespace Mesh


#endif  // MESH_SEGMENT_H
