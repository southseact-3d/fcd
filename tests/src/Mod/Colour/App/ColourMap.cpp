// SPDX-License-Identifier: LGPL-2.1-or-later

#include <gtest/gtest.h>

#include <App/Document.h>
#include <Base/FileInfo.h>
#include <fstream>

#include <Mod/Colour/App/ColourMap.h>
#include <Mod/Colour/App/ColourMapObject.h>
#include <Mod/Colour/App/Exporter.h>
#include <Mod/Colour/App/TextureCompositor.h>
#include <Mod/Colour/App/UVAtlas.h>
#include <src/App/InitApplication.h>

class ColourModuleTest: public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        tests::initApplication();
    }
};

TEST_F(ColourModuleTest, ColourMapRoundTrip)
{
    Colour::ColourMap map;
    map.setFaceColour(3, Colour::RGBA {10, 20, 30, 255});

    Colour::Layer layer;
    layer.name = "fill";
    layer.zOrder = 0;
    layer.colour = Colour::RGBA {100, 110, 120, 200};
    layer.imageRef = "img&1";
    map.addLayer(3, layer);
    map.registerImage("img&1", "Textures/face<1>.png");

    const std::string xml = map.serializeXml();

    Colour::ColourMap loaded;
    ASSERT_TRUE(loaded.deserializeXml(xml));

    auto c = loaded.getFaceColour(3);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(c.g, 20);
    EXPECT_EQ(c.b, 30);

    const auto& layers = loaded.getLayers(3);
    ASSERT_EQ(layers.size(), 1U);
    EXPECT_EQ(layers[0].name, "fill");
    EXPECT_EQ(layers[0].colour.r, 100);
    EXPECT_EQ(layers[0].imageRef, "img&1");
    EXPECT_EQ(loaded.getImageStoragePath("img&1"), "Textures/face<1>.png");
}

TEST_F(ColourModuleTest, ColourMapRemapFaceIndices)
{
    Colour::ColourMap map;
    map.setFaceColour(1, Colour::RGBA {10, 20, 30, 255});
    map.setFacePbr(1, Colour::PBR {0.4F, 0.5F, 0.6F});
    map.addLayer(1, Colour::Layer {});

    map.remapFaceIndices({{1, 42}});

    const auto c = map.getFaceColour(42);
    EXPECT_EQ(c.r, 10);
    EXPECT_EQ(map.getLayers(42).size(), 1U);
    EXPECT_EQ(map.getFacePbr(42).metallic, 0.4F);
}

TEST_F(ColourModuleTest, UVAtlasGeneratesIslands)
{
    std::vector<Colour::Vertex> vertices(3);
    vertices[0].position = {0.0F, 0.0F, 0.0F};
    vertices[1].position = {1.0F, 0.0F, 0.0F};
    vertices[2].position = {0.0F, 1.0F, 0.0F};
    vertices[0].normal = {0.0F, 0.0F, 1.0F};
    vertices[1].normal = {0.0F, 0.0F, 1.0F};
    vertices[2].normal = {0.0F, 0.0F, 1.0F};

    Colour::Triangle tri;
    tri.indices[0] = 0;
    tri.indices[1] = 1;
    tri.indices[2] = 2;
    tri.faceIndex = 1;

    Colour::UVAtlas atlas;
    ASSERT_TRUE(atlas.generateFacePerIsland(vertices, {tri}, 256, 2));
    EXPECT_EQ(atlas.triangleUvs().size(), 1U);
    EXPECT_EQ(atlas.islandsByFace().size(), 1U);
}

TEST_F(ColourModuleTest, PngAnd3mfExportProduceValidSignatures)
{
    std::vector<Colour::Vertex> vertices(3);
    vertices[0].position = {0.0F, 0.0F, 0.0F};
    vertices[1].position = {1.0F, 0.0F, 0.0F};
    vertices[2].position = {0.0F, 1.0F, 0.0F};
    vertices[0].normal = {0.0F, 0.0F, 1.0F};
    vertices[1].normal = {0.0F, 0.0F, 1.0F};
    vertices[2].normal = {0.0F, 0.0F, 1.0F};

    Colour::Triangle tri;
    tri.indices[0] = 0;
    tri.indices[1] = 1;
    tri.indices[2] = 2;
    tri.faceIndex = 9;

    Colour::UVAtlas atlas;
    ASSERT_TRUE(atlas.generateFacePerIsland(vertices, {tri}, 128, 2));

    Colour::ColourMap colourMap;
    colourMap.setFaceColour(9, {255, 0, 0, 255});

    Colour::TextureCompositor compositor;
    const Colour::AtlasImage base = compositor.compose(colourMap, atlas);

    std::vector<std::uint8_t> png;
    ASSERT_TRUE(compositor.encodePng(base, png));
    ASSERT_GE(png.size(), 8U);
    EXPECT_EQ(png[0], 137U);
    EXPECT_EQ(png[1], 80U);
    EXPECT_EQ(png[2], 78U);
    EXPECT_EQ(png[3], 71U);

    const std::string out3mf = Base::FileInfo::getTempFileName("colour_test") + ".3mf";
    Colour::Exporter exporter;
    ASSERT_TRUE(exporter.export3mf(out3mf, vertices, {tri}, atlas, base));

    std::ifstream f(out3mf, std::ios::binary);
    ASSERT_TRUE(f.is_open());
    unsigned char sig[4] = {0, 0, 0, 0};
    f.read(reinterpret_cast<char*>(sig), 4);
    EXPECT_EQ(sig[0], 0x50U); // PK
    EXPECT_EQ(sig[1], 0x4BU);
}

TEST_F(ColourModuleTest, ColourMapObjectDocumentIntegration)
{
    // Test that ColourMapObject integrates with FreeCAD document system
    App::Document* doc = App::GetApplication().newDocument("TestColour");
    ASSERT_NE(doc, nullptr);

    // Create a ColourMapObject
    Colour::ColourMapObject* colourMapObj = new Colour::ColourMapObject();
    colourMapObj->setColourMap(Colour::ColourMap());

    // Add to document
    doc->addObject(colourMapObj, "ColourMap");

    // Verify object was added
    auto objects = doc->getObjects();
    ASSERT_EQ(objects.size(), 1);
    EXPECT_TRUE(objects[0]->isDerivedFrom(Colour::ColourMapObject::getClassTypeId()));

    // Set some face colours
    Colour::ColourMap map;
    map.setFaceColour(0, {255, 0, 0, 255});
    map.setFaceColour(1, {0, 255, 0, 255});
    map.setFacePbr(0, {0.5F, 0.3F, 1.0F});

    colourMapObj->setColourMap(map);

    // Verify the colour data was set
    const Colour::ColourMap& retrievedMap = colourMapObj->getColourMap();
    auto colour0 = retrievedMap.getFaceColour(0);
    EXPECT_EQ(colour0.r, 255);
    EXPECT_EQ(colour0.g, 0);
    EXPECT_EQ(colour0.b, 0);

    auto pbr0 = retrievedMap.getFacePbr(0);
    EXPECT_FLOAT_EQ(pbr0.metallic, 0.5F);

    // Cleanup
    App::GetApplication().closeDocument(doc->getName());
}

TEST_F(ColourModuleTest, ColourMapObjectSerialization)
{
    // Test XML serialization round-trip
    Colour::ColourMapObject colourMapObj;

    Colour::ColourMap map;
    map.setFaceColour(0, {128, 64, 32, 255});
    map.setFacePbr(0, {0.8F, 0.4F, 1.0F});
    map.registerImage("test_img", "/path/to/image.png");

    Colour::Layer layer;
    layer.name = "Base Layer";
    layer.zOrder = 0;
    layer.colour = {200, 100, 50, 255};
    layer.blendMode = Colour::BlendMode::Normal;
    map.addLayer(0, layer);

    colourMapObj.setColourMap(map);

    // Serialize
    const std::string xml = map.serializeXml();
    EXPECT_FALSE(xml.empty());
    EXPECT_NE(xml.find("<ColourMap>"), std::string::npos);
    EXPECT_NE(xml.find("</ColourMap>"), std::string::npos);

    // Deserialize
    Colour::ColourMap loadedMap;
    ASSERT_TRUE(loadedMap.deserializeXml(xml));

    // Verify
    auto colour = loadedMap.getFaceColour(0);
    EXPECT_EQ(colour.r, 128);
    EXPECT_EQ(colour.g, 64);
    EXPECT_EQ(colour.b, 32);
    EXPECT_EQ(colour.a, 255);

    auto pbr = loadedMap.getFacePbr(0);
    EXPECT_FLOAT_EQ(pbr.metallic, 0.8F);
    EXPECT_FLOAT_EQ(pbr.roughness, 0.4F);

    EXPECT_EQ(loadedMap.getImageStoragePath("test_img"), "/path/to/image.png");

    const auto& layers = loadedMap.getLayers(0);
    ASSERT_EQ(layers.size(), 1U);
    EXPECT_EQ(layers[0].name, "Base Layer");
}
