
// ATLAS: Adaptable Tool Library for Advanced Simulation
//
// Copyright 2015 University of Central Florida
//
//
// This library provides many fundamental capabilities used in creating
// virtual environment simulations.  It includes elements such as vectors,
// matrices, quaternions, containers, communication schemes (UDP, TCP, DIS,
// HLA, Bluetooth), and XML processing.  It also includes some extensions
// to allow similar code to work in Linux and in Windows.  Note that support
// for iOS and Android development is also included.
//
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
#include "atXMLDocument.h++"


atXMLDocument::atXMLDocument(atXMLDocumentDocPtr doc)
{
   // Save a pointer to the XML document
   xml_doc = (xmlDocPtr ) doc;

   // Initialize the value for holding temp strings
   string_value = new atString();

   // Initialize the xml text pointer
   xml_text = NULL;
}


atXMLDocument::atXMLDocument(atXMLDocument * doc)
{
   // Save a pointer to the XML document of the document we are copying
   xml_doc = xmlCopyDoc(doc->xml_doc, 1);

   // Initialize the value for holding temp strings
   string_value = new atString();

   // Initialize the xml text pointer
   xml_text = NULL;
}


atXMLDocument::~atXMLDocument()
{
   // Free up the text buffer 
   if (xml_text != NULL)
      xmlFree(xml_text);

   // Free up the temp string
   if (string_value != NULL)
      delete string_value;

   // Free the XML document itself
   xmlFreeDoc(xml_doc);
}


atXMLDocumentDocPtr atXMLDocument::getDoc()
{
   // Return the base xml doc
   return xml_doc;
}


atXMLDocumentNodePtr atXMLDocument::getRootNode()
{
   // Get the root node pointer and return it
   return (atXMLDocumentNodePtr ) xmlDocGetRootElement(xml_doc);
}


atXMLDocumentNodePtr atXMLDocument::getNextSiblingNode(
   atXMLDocumentNodePtr node)
{
   xmlNodePtr   xmlNode;

   // Get the next sibling node of this node and return it
   xmlNode = (xmlNodePtr ) node;
   return (atXMLDocumentNodePtr ) node->next;
}


atXMLDocumentNodePtr atXMLDocument::getNextChildNode(atXMLDocumentNodePtr node)
{
   xmlNodePtr   xmlNode;
                                                                                
   // Get the child node of this node and return it
   xmlNode = (xmlNodePtr ) node;
   return (atXMLDocumentNodePtr ) node->xmlChildrenNode;
}


char * atXMLDocument::getNodeName(atXMLDocumentNodePtr node)
{
   return (char * ) ((xmlNodePtr ) node)->name;
}


char * atXMLDocument::getNodeAttribute(atXMLDocumentNodePtr node, 
                                       char *attrName)
{
   char *   prop;

   // Get the property
   prop = (char *) xmlGetProp(node, (const xmlChar *) attrName);

   // Copy the property value into our internal string
   if (prop != NULL)
      string_value->setString(prop);
   else
      string_value->setString("");

   // Free up the XML structure
   if (prop != NULL)
      xmlFree((xmlChar *) prop);

   // Return the string
   return string_value->getString();
}


char * atXMLDocument::getNodeText(atXMLDocumentNodePtr node)
{
   // This function is to support the older (deprecated) method so we just 
   // call the replacement function with a "0" (meaning don't translate the
   // data within the XML itself)
   return getNodeText(node, 0);
}


char * atXMLDocument::getNodeText(atXMLDocumentNodePtr node, bool val)
{
   char *   str;

   // Get the string from the XML node (using true for val causes the 
   // parser to decode any special characters (ex: &amp; would be &) while 
   // using true causes the parser to encode any special characters 
   // (ex: & would be &amp;))
   // Note: CDATA seems to be unaffected with val as true so this can be 
   // used to prevent CDATA from being changed (however, this seems odd 
   // from the libxml2 documentation so it is possible this behavior may
   // change; it may be that xmlNodeListGetRawString or xmlNodeGetContent
   // should be used for CDATA but this will require more research)
   if (val)
   {
      str = (char *) xmlNodeListGetString(xml_doc, node, 1);
   }
   else
   {
      str = (char *) xmlNodeListGetString(xml_doc, node, 0);
   }

   // Copy the string value (if there is one) into our internal string
   if (str != NULL)
      string_value->setString(str);
   else
      string_value->setString("");

   // Free up the XML structure
   if (str != NULL)
      xmlFree((xmlChar *) str);

   // Return the string
   return string_value->getString();
}


void atXMLDocument::retrieveXML(u_char ** buffer, u_long * bufferLen)
{
   int   len;

   // If we previously called this method (xml_text is not NULL), then
   // we need to free it first
   if (xml_text != NULL)
      xmlFree(xml_text);

   // Now get the XML tree as text and save it in the internal text buffer
   xmlDocDumpFormatMemory(xml_doc, &xml_text, &len, 1);

   // Then point the user's pointer to this buffer
   *buffer = (u_char *) xml_text;
   *bufferLen = (u_long ) len;
}


void atXMLDocument::print()
{
   u_char *   buffer;
   u_long     bufferLen;

   // Get the XML
   retrieveXML(&buffer, &bufferLen);

   // Then print it
   buffer[bufferLen] = '\0';
   printf("%s\n", buffer);
}

