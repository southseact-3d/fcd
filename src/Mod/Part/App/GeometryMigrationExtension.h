#ifndef PART_GEOMETRYMIGRATIONEXTENSION_H
#define PART_GEOMETRYMIGRATIONEXTENSION_H

#include <bitset>
#include "Geometry.h"

#include <Mod/Part/PartGlobal.h>

namespace Part
{

// This is a light-weight c++ only geometry extension to enable migration of information that was
// stored within the Part WB and should be migrated to another WB
//
// It is designed so that a single extension can migrate different types of data.
//
// To migrate data:
// 1. Add an enum bit to indicate the type of migration type
// 2. Add the data members to store the information and accessors
class PartExport GeometryMigrationExtension: public Part::GeometryExtension
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    // Indicates the type of migration to be performed, it is stored as a bitset, so several
    // migrations may take place in a single extension.
    // It is intended to support also LinkStage3 migration with a single framework (Id, Ref, ...)
    enum MigrationType
    {
        None = 0,
        Construction = 1,
        GeometryId = 2,
        ExternalReference = 3,
        NumMigrationType  // Must be the last
    };

    GeometryMigrationExtension() = default;
    ~GeometryMigrationExtension() override = default;

    std::unique_ptr<Part::GeometryExtension> copy() const override;

    PyObject* getPyObject() override;


    virtual bool getConstruction() const
    {
        return ConstructionState;
    }

    virtual void setConstruction(bool construction)
    {
        ConstructionState = construction;
        setMigrationType(Construction);
    }

    long getId() const
    {
        return Id;
    }

    void setId(long id)
    {
        Id = id;
        setMigrationType(GeometryId);
    }

    const std::string& getRef() const
    {
        return Ref;
    }

    int getRefIndex() const
    {
        return RefIndex;
    }

    unsigned long getFlags() const
    {
        return Flags;
    }

    void setReference(const char* ref, int index, unsigned long flags)
    {
        Ref = ref ? ref : "";
        RefIndex = index;
        Flags = flags;
        setMigrationType(ExternalReference);
    }

    virtual bool testMigrationType(int flag) const
    {
        return GeometryMigrationFlags.test((size_t)(flag));
    };
    virtual void setMigrationType(int flag, bool v = true)
    {
        GeometryMigrationFlags.set((size_t)(flag), v);
    };

protected:
    void copyAttributes(Part::GeometryExtension* cpy) const override;

private:
    GeometryMigrationExtension(const GeometryMigrationExtension&) = default;

private:
    using MigrationTypeFlagType = std::bitset<32>;
    MigrationTypeFlagType GeometryMigrationFlags;
    bool ConstructionState {false};
    long Id = 0;
    int RefIndex = -1;
    unsigned long Flags = 0;
    std::string Ref;
};


class PartExport GeometryMigrationPersistenceExtension: public Part::GeometryPersistenceExtension
{
    TYPESYSTEM_HEADER();

public:
    // Called to extend 'Geometry' XML tag with additional attributes (eg Id)
    virtual void preSave(Base::Writer& /*writer*/) const
    {}
    // Called to add additional tag after 'GeometryExtensions' XML elements (eg Construction flag)
    virtual void postSave(Base::Writer& /*writer*/) const
    {}
};

}  // namespace Part


#endif  // PART_GEOMETRYMIGRATIONEXTENSION_H
