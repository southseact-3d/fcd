#pragma once

#include <limits>
#include <stack>

#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>

#include "Common.h"

using namespace XERCES_CPP_NAMESPACE;

namespace XERCES_CPP_NAMESPACE
{
   class SAX2XMLReader;
}

namespace e57
{
   class CheckedFile;

   class E57XmlParser : public DefaultHandler
   {
   public:
      explicit E57XmlParser( ImageFileImplSharedPtr imf );
      ~E57XmlParser() override;

      void init();

      void parse( InputSource &inputSource );

   private:
      /// SAX interface
      void startElement( const XMLCh *uri, const XMLCh *localName, const XMLCh *qName,
                         const Attributes &attributes ) override;
      void endElement( const XMLCh *uri, const XMLCh *localName, const XMLCh *qName ) override;
      void characters( const XMLCh *chars, XMLSize_t length ) override;

      /// SAX error interface
      void warning( const SAXParseException &ex ) override;
      void error( const SAXParseException &ex ) override;
      void fatalError( const SAXParseException &ex ) override;

      ImageFileImplSharedPtr imf_; /// Image file we are reading

      struct ParseInfo
      {
         // All the fields need to remember while parsing the XML
         // Not all fields are used at same time, depends on node type
         // Needed because not all info is available at one time to create the
         // node.
         NodeType nodeType;               // used by all types
         int64_t minimum;                 // used in Integer, ScaledInteger
         int64_t maximum;                 // used in Integer, ScaledInteger
         double scale;                    // used in ScaledInteger
         double offset;                   // used in ScaledInteger
         FloatPrecision precision;        // used in Float
         double floatMinimum;             // used in Float
         double floatMaximum;             // used in Float
         int64_t fileOffset;              // used in Blob, CompressedVector
         int64_t length;                  // used in Blob
         bool allowHeterogeneousChildren; // used in Vector
         int64_t recordCount;             // used in CompressedVector
         ustring childText; // used by all types, accumulates all child text between tags

         // Holds node for Structure, Vector, and CompressedVector so can append
         // child elements
         NodeImplSharedPtr container_ni;

         ParseInfo(); // default ctor
         void dump( int indent = 0, std::ostream &os = std::cout ) const;
      };

      std::stack<ParseInfo> stack_; /// Stores the current path in tree we are reading

      SAX2XMLReader *xmlReader;
   };

   class E57XmlFileInputSource : public InputSource
   {
   public:
      E57XmlFileInputSource( CheckedFile *cf, uint64_t logicalStart, uint64_t logicalLength );
      ~E57XmlFileInputSource() override = default;

      E57XmlFileInputSource( const E57XmlFileInputSource & ) = delete;
      E57XmlFileInputSource &operator=( const E57XmlFileInputSource & ) = delete;

      BinInputStream *makeStream() const override;

   private:
      //??? lifetime of cf_ must be longer than this object!
      CheckedFile *cf_;
      uint64_t logicalStart_;
      uint64_t logicalLength_;
   };
}
