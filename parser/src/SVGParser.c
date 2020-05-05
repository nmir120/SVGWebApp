/*
Numan Mir
1005381
nmir@uoguelph.ca
SVG App
*/

/**
References:
- Used Prof. Nikitenko's StructListDemo.c and libXmlExample.c as reference to form createSVGimage.
- Used code from http://knol2share.blogspot.com/2009/05/validate-xml-against-xsd-in-c.html as reference
to help in validating against an XSD file
- Used code from http://www.xmlsoft.org/examples/tree2.c to assist in creating XML tree to be saved in
svg file
**/

#define LIBXML_SCHEMAS_ENABLED
#include <libxml/xmlschemastypes.h>
#include "SVGParser.h"
#include <string.h>
#include "LinkedListAPI.h"
#include <math.h>
#include <ctype.h>


char* JSONattrsListOfCurrentPath(char* fileName, int index);
char* JSONattrsListOfCurrentRect(char* fileName, int index);
char* JSONfirstLayerGroups(char* fileName);
char* JSONfirstLayerPaths(char* fileName);
char* JSONfirstLayerCircs(char* fileName);
char* JSONfirstLayerRects(char* fileName);
char* getTitle(char* fileName);
char* getDescription(char* fileName);
char* jsonOfCreateValidSVGimage(char* fileName);
void createSVGFileFromJSON(char* svgFile, char* fileName);
bool setOtherAttr(ListIterator iterAttrs, Attribute* newAttribute);
bool validateAttributes(List* attrs);
bool validateToXSD(xmlDocPtr doc, char* schemaFile);
void rectToDoc(xmlNodePtr childOf, ListIterator iterRects);
void circToDoc(xmlNodePtr childOf, ListIterator iterCircs);
void pathToDoc(xmlNodePtr childOf, ListIterator iterPaths);
void groupToDoc(xmlNodePtr childOf, ListIterator iterGroups);
xmlDocPtr svgToDoc(SVGimage* img);
SVGimage* parseNodes(xmlNode* temp, SVGimage* img);
Rectangle* parseRectangles(xmlNode* temp, SVGimage* img);
Circle* parseCircles(xmlNode* temp, SVGimage* img);
Path* parsePaths(xmlNode* temp, SVGimage* img);
Group* parseGroups(xmlNode* temp, SVGimage* img);
void thingsFromNestedGroups(List* allThings, Group* groupElement, int num);
/** To avoid memory errors i.e.; deleting rect data twice **/
void deleteDummyRectangle(void* data); //group all into one?
void deleteDummyCircle(void* data);
void deleteDummyPath(void* data);
void deleteDummyGroup(void* data);
void deleteJSAttrList(void* attrStr);

/** Helper function to write rectangles to xml tree**/
void rectToDoc(xmlNodePtr childOf, ListIterator iterRects) {
    xmlNodePtr child = NULL;
    Rectangle* curRect = nextElement(&iterRects);
    char buffer[300];
    while(curRect != NULL) {
        child = xmlNewChild(childOf, NULL, (xmlChar*)"rect", NULL);
        //since this function assumes valid svg struct, does this mean all rects will have x, y and all components (and valid values)?
        sprintf(buffer, "%f", curRect->x);
        if(curRect->units != NULL) {
            if(curRect->units[0] != '\0') strcat(buffer, curRect->units);
        }
        xmlNewProp(child, (xmlChar*)"x", (xmlChar*)buffer); //store x value
        sprintf(buffer, "%f", curRect->y);
        if(curRect->units != NULL) {
            if(curRect->units[0] != '\0') strcat(buffer, curRect->units);
        }
        xmlNewProp(child, (xmlChar*)"y", (xmlChar*)buffer); //store y value
        sprintf(buffer, "%f", curRect->width);
        if(curRect->units != NULL) {
            if(curRect->units[0] != '\0') strcat(buffer, curRect->units);
        }
        xmlNewProp(child, (xmlChar*)"width", (xmlChar*)buffer); //store width value
        sprintf(buffer, "%f", curRect->height);
        if(curRect->units != NULL) {
            if(curRect->units[0] != '\0') strcat(buffer, curRect->units);
        }
        xmlNewProp(child, (xmlChar*)"height", (xmlChar*)buffer); //store height value
        //otherAttributes
        int numOfAttrs = 0;
        if(curRect->otherAttributes != NULL) {
            numOfAttrs = getLength(curRect->otherAttributes);
        }
        if(numOfAttrs > 0) {
            ListIterator iterAttrs = createIterator(curRect->otherAttributes);
            Attribute* curAttr = nextElement(&iterAttrs);
        
            while(curAttr != NULL) { //what if attributes' name or value is blank?
                xmlNewProp(child, (xmlChar*)curAttr->name, (xmlChar*)curAttr->value);
                curAttr = nextElement(&iterAttrs);
            }

        }
        curRect = nextElement(&iterRects);
    }
}

void circToDoc(xmlNodePtr childOf, ListIterator iterCircs) {
    xmlNodePtr child = NULL;
    Circle* curCirc = nextElement(&iterCircs);
    char buffer[300];
    while(curCirc != NULL) {
        child = xmlNewChild(childOf, NULL, (xmlChar*)"circle", NULL);
        //since this function assumes valid svg struct, does this mean all rects will have x, y and all components (and valid values)?
        sprintf(buffer, "%f", curCirc->cx);
        if(curCirc->units != NULL) {
            if(curCirc->units[0] != '\0') strcat(buffer, curCirc->units); 
        }
        xmlNewProp(child, (xmlChar*)"cx", (xmlChar*)buffer); //store cx value
        sprintf(buffer, "%f", curCirc->cy);
        if(curCirc->units != NULL) {
            if(curCirc->units[0] != '\0') strcat(buffer, curCirc->units); 
        }
        xmlNewProp(child, (xmlChar*)"cy", (xmlChar*)buffer); //store cy value
        sprintf(buffer, "%f", curCirc->r);
        if(curCirc->units != NULL) {
            if(curCirc->units[0] != '\0') strcat(buffer, curCirc->units); 
        }
        xmlNewProp(child, (xmlChar*)"r", (xmlChar*)buffer); //store r value
        //otherAttributes
        int numOfAttrs = 0;
        if(curCirc->otherAttributes != NULL) {
            numOfAttrs = getLength(curCirc->otherAttributes);
        }
        if(numOfAttrs > 0) {
            ListIterator iterAttrs = createIterator(curCirc->otherAttributes);
            Attribute* curAttr = nextElement(&iterAttrs);
        
            while(curAttr != NULL) { //what if attributes' name or value is blank?
                xmlNewProp(child, (xmlChar*)curAttr->name, (xmlChar*)curAttr->value);
                curAttr = nextElement(&iterAttrs);
            }

        }
        curCirc = nextElement(&iterCircs);
    }
}
void pathToDoc(xmlNodePtr childOf, ListIterator iterPaths) {
    xmlNodePtr child = NULL;
    Path* curPath = nextElement(&iterPaths);
    while(curPath != NULL) {
        child = xmlNewChild(childOf, NULL, (xmlChar*)"path", NULL);

        xmlNewProp(child, (xmlChar*)"d", (xmlChar*)curPath->data); //store data

        //otherAttributes
        int numOfAttrs = 0;
        if(curPath->otherAttributes != NULL) {
            numOfAttrs = getLength(curPath->otherAttributes);
        }
        if(numOfAttrs > 0) {
            ListIterator iterAttrs = createIterator(curPath->otherAttributes);
            Attribute* curAttr = nextElement(&iterAttrs);
        
            while(curAttr != NULL) { //what if attributes' name or value is blank?
                xmlNewProp(child, (xmlChar*)curAttr->name, (xmlChar*)curAttr->value);
                curAttr = nextElement(&iterAttrs);
            }

        }
        curPath = nextElement(&iterPaths);
    }
}
void groupToDoc(xmlNodePtr childOf, ListIterator iterGroups) {
    xmlNodePtr child = NULL;
    Group* curGroup = nextElement(&iterGroups);
    while(curGroup != NULL) {
        child = xmlNewChild(childOf, NULL, (xmlChar*)"g", NULL);
        //group's other attributes
        int numOfAttrs = 0;
        if(curGroup->otherAttributes != NULL) {
            numOfAttrs = getLength(curGroup->otherAttributes);
        }
        if(numOfAttrs > 0) {
            ListIterator iterAttrs = createIterator(curGroup->otherAttributes);
            Attribute* curAttr = nextElement(&iterAttrs);
            while(curAttr != NULL) { //what if attributes' name or value is blank?
                xmlNewProp(child, (xmlChar*)curAttr->name, (xmlChar*)curAttr->value);
                curAttr = nextElement(&iterAttrs);
            }

        }
        //group's rectangles list
        int numOfRects = 0;
        if(curGroup->rectangles != NULL) {
            numOfRects = getLength(curGroup->rectangles);
        }
        if(numOfRects > 0) {
            ListIterator iterRects = createIterator(curGroup->rectangles);
            rectToDoc(child, iterRects);
        }
        //group's circles list
        int numOfCircs = 0;
        if(curGroup->circles != NULL) {
            numOfCircs = getLength(curGroup->circles);
        }
        if(numOfCircs > 0) {
            ListIterator iterCircs = createIterator(curGroup->circles);
            circToDoc(child, iterCircs);
        }
        //group's paths list
        int numOfPaths = 0;
        if(curGroup->paths != NULL) {
            numOfPaths = getLength(curGroup->paths);
        }
        if(numOfPaths > 0) {
            ListIterator iterPaths = createIterator(curGroup->paths);
            pathToDoc(child, iterPaths);
        }
        //group's group list
        int numOfGroups = 0;
        if(curGroup->groups != NULL) {
           numOfGroups = getLength(curGroup->groups);
        }
        if(numOfGroups > 0) {
            ListIterator iterGroups = createIterator(curGroup->groups);
            groupToDoc(child, iterGroups); //uhh that was suspiciously easy
        }

        curGroup = nextElement(&iterGroups);
    }
}

/** Helper function to convert SVGimage struct into xmlDoc struct **/
xmlDocPtr svgToDoc(SVGimage* img) {
    //create xml tree
    xmlDocPtr doc = NULL;  /* document pointer */
    xmlNodePtr root_node = NULL;/* node pointers */

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    //storing root node's otherAttributes
    int numOfAttrs = 0;
    if(img->otherAttributes != NULL) {
        numOfAttrs = getLength(img->otherAttributes);
    }
    if(numOfAttrs > 0) {
        ListIterator iterAttrs = createIterator(img->otherAttributes);
        Attribute* curAttr = nextElement(&iterAttrs);
        
        while(curAttr != NULL) { //what if attributes' name or value is blank?
            xmlNewProp(root_node, (xmlChar*)curAttr->name, (xmlChar*)curAttr->value);
            curAttr = nextElement(&iterAttrs);
        }

    }
    //saving namespace into tree
    xmlNsPtr nameSpace = xmlNewNs(root_node, (xmlChar*)img->namespace, NULL);
    xmlSetNs(root_node, nameSpace);

    if(img->title != NULL && img->title[0] != '\0') { //is this the right way to check?
        xmlNewChild(root_node, NULL, (xmlChar*)"title", (xmlChar*)img->title);
    }
    if(img->description != NULL && img->description[0] != '\0') {
        xmlNewChild(root_node, NULL, (xmlChar*)"desc", (xmlChar*)img->description);
    }

    int numOfRect = 0;
    //saving first layer of rectangles
    if(img->rectangles != NULL) {
        numOfRect = getLength(img->rectangles);
    }
    if(numOfRect > 0) { //prob call another method
        ListIterator iterRects = createIterator(img->rectangles);
        rectToDoc(root_node, iterRects);
    }

    int numOfCirc = 0;
    //saving first layer of circles
    if(img->circles != NULL) {
        numOfCirc = getLength(img->circles);
    }
    if(numOfCirc > 0) {
        ListIterator iterCircs = createIterator(img->circles);
        circToDoc(root_node, iterCircs);
    }
    int numOfPaths = 0;
    //saving first layer of paths
    if(img->paths != NULL) {
        numOfPaths = getLength(img->paths);
    }
    if(numOfPaths > 0) {
        ListIterator iterPaths = createIterator(img->paths);
        pathToDoc(root_node, iterPaths);
    }
    //groups n stuff
    int numOfGroups = 0;
    //saving first layer of paths
    if(img->groups != NULL) {
        numOfGroups = getLength(img->groups);
    }
    if(numOfGroups > 0) {
        ListIterator iterGroups = createIterator(img->groups);
        groupToDoc(root_node, iterGroups);
    }
    
    return doc;

}
/**
    Helper function that will return true if fileName validates with XSD schemaFile
**/
bool validateToXSD(xmlDocPtr doc, char* schemaFile) {
    bool valid = false;
	xmlSchemaPtr schema = NULL;
	xmlSchemaParserCtxtPtr ctxt = NULL;

    xmlLineNumbersDefault(1);
    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

	if (doc != NULL){
		xmlSchemaValidCtxtPtr ctxt = NULL;
		int ret = 0;
		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, doc);
		if (ret == 0) {
            valid = true;
		}
		xmlSchemaFreeValidCtxt(ctxt);
		xmlFreeDoc(doc);
	}

	// free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    //xmlMemoryDump();
    return valid;
}
/** Function to create an SVG object based on the contents of an SVG file.
 * This function must validate the XML tree generated by libxml against a SVG schema file
 * before attempting to traverse the tree and create an SVGimage struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
       Schema file name is not NULL/empty, and represents a valid schema file
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, or SVG file was invalid, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createValidSVGimage(char* fileName, char* schemaFile) {
    SVGimage* img = NULL;
    bool xsdValid = false;
    xmlDocPtr doc = NULL;
    if(fileName != NULL && fileName[0] != '\0' && schemaFile != NULL && schemaFile[0] != '\0') {
        doc = xmlReadFile(fileName, NULL, 0);
        xsdValid = validateToXSD(doc, schemaFile); //if true is returned, doc was sucessfully validated against schemaFile
        if(xsdValid) {
            img = createSVGimage(fileName); //create svgstruct with validated doc
        }
    }
    return img;
}

/** Function to writing a SVGimage into a file in SVG format.
 *@pre
    SVGimage object exists, is valid, and is not NULL.
    fileName is not NULL, has the correct extension
 *@post SVGimage has not been modified in any way, and a file representing the
    SVGimage contents in SVG format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a SVGimage struct
 	fileName - the name of the output file
 **/
bool writeSVGimage(SVGimage* image, char* fileName) {
    bool success = false;
    //can assume image was already validated using validateSVGimage
    xmlDocPtr doc = NULL;
    //how to check if fileName has the .svg extension?
    if(image != NULL && fileName != NULL && fileName[0] != '\0') {
        doc = svgToDoc(image); 
        xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        success = true;
    }

    return success;
}

bool validateAttributes(List* attrs) {
    bool valid = true;
    if(getLength(attrs) > 0) {
        ListIterator iterAttrs = createIterator(attrs);
        Attribute* curAttr = nextElement(&iterAttrs);
        while(curAttr != NULL) {
            if(curAttr->name == NULL || curAttr->value == NULL) {
                return false;
            }
            curAttr = nextElement(&iterAttrs);
        }
    }
    return valid;

}

/** Function to validating an existing a SVGimage object against a SVG schema file
 *@pre 
    SVGimage object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post SVGimage has not been modified in any way
 *@return the boolean aud indicating whether the SVGimage is valid
 *@param obj - a pointer to a GPXSVGimagedoc struct
 *@param obj - the name iof a schema file
 **/
bool validateSVGimage(SVGimage* image, char* schemaFile) {
    bool valid = true; //start off true.
    xmlDocPtr imgDoc = NULL;
    if(image == NULL || schemaFile == NULL || schemaFile[0] == '\0') {
        valid = false;
    }

    if(valid) {
        imgDoc = svgToDoc(image);
        //validating svg content against schema
        valid = validateToXSD(imgDoc, schemaFile);
    }

    if(valid) {
        if(image->namespace == NULL) { //xsd already does this?
            return false;
        } 
        else if(strlen(image->namespace) < 1) { 
            return false;
        }
        //should i check if title/desc are null? they can be empty...
        if(image->title == NULL || image->description == NULL) {
            return false;
        }
        //lists must be initialized
        if(image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) {
            return false;
        }
        //validating image->otherAttributes
        valid = validateAttributes(image->otherAttributes);
        if(!valid) {
            return false;
        }
        //validating rectangles
        List* allRects = getRects(image);
        if(getLength(allRects) > 0) {
            ListIterator iterRects = createIterator(allRects);
            Rectangle* curRect = nextElement(&iterRects);
            while(curRect != NULL) {
                if(curRect->width < 0 || curRect->height < 0) { //width, height must be >= 0
                    freeList(allRects);
                    return false;
                }
                else if(curRect->otherAttributes == NULL) { //otherAttributes must not be NULL, can be empty
                    freeList(allRects);
                    return false;
                }
                else {
                    valid = validateAttributes(curRect->otherAttributes);
                    if(!valid) {
                        freeList(allRects);
                        return false;
                    }
                }
                curRect = nextElement(&iterRects);
            }
        }
        freeList(allRects);
        //validating circles
        List* allCircs = getCircles(image);
        if(getLength(allCircs) > 0) {
            ListIterator iterCircs = createIterator(allCircs);
            Circle* curCirc = nextElement(&iterCircs);
            while(curCirc != NULL) {
                if(curCirc->r < 0) {
                    freeList(allCircs);
                    return false;
                }
                else if(curCirc->otherAttributes == NULL) {
                    freeList(allCircs);
                    return false;
                }
                else {
                    valid = validateAttributes(curCirc->otherAttributes);
                    if(!valid) {
                        freeList(allCircs);
                        return false;
                    }
                }
                curCirc = nextElement(&iterCircs);
            }
        }
        freeList(allCircs);
        //validating paths
        List* allPaths = getPaths(image);
        if(getLength(allPaths) > 0) {
            ListIterator iterPaths = createIterator(allPaths);
            Path* curPath = nextElement(&iterPaths);
            while(curPath != NULL) {
                if(curPath->data == NULL) {
                    freeList(allPaths);
                    return false;
                }
                else if(curPath->otherAttributes == NULL) {
                    freeList(allPaths);
                    return false;
                }
                else {
                    valid = validateAttributes(curPath->otherAttributes);
                    if(!valid) {
                        freeList(allPaths);
                        return false;
                    }
                }
                curPath = nextElement(&iterPaths);
            }
        }
        freeList(allPaths);
        //validating groups
        List* allGroups = getGroups(image);
        if(getLength(allGroups) > 0) {
            ListIterator iterGroups = createIterator(allGroups);
            Group* curGroup = nextElement(&iterGroups);
            while(curGroup != NULL) {
                if(curGroup->rectangles == NULL || curGroup->circles == NULL || curGroup->paths == NULL || curGroup->groups == NULL || curGroup->otherAttributes == NULL) {
                    freeList(allGroups);
                    return false;
                }
                else { //if all lists are initialized, check individual attributes in otherAttributes
                    valid = validateAttributes(curGroup->otherAttributes);
                    if(!valid) {
                        freeList(allGroups);
                        return false;
                    }
                }
                curGroup = nextElement(&iterGroups);
            }
        }
        freeList(allGroups);

    }

    return valid;
}


/** Function to create an SVG object based on the contents of an SVG file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createSVGimage(char* fileName){
   SVGimage* image = NULL;
   if(fileName == NULL || fileName[0] == '\0'){
       return NULL;
   }
   xmlDoc *doc = NULL;
   xmlNode *root = NULL;
  
   //check to see if not an SVG extension?
   doc = xmlReadFile(fileName, NULL, 0);
   if(doc == NULL) {
       xmlFreeDoc(doc);
       return NULL;
   }
 
   image = (SVGimage*)malloc(sizeof(SVGimage));
   image->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
   image->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
   image->paths = initializeList(&pathToString, &deletePath, &comparePaths);
   image->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
 
   strcpy(image->title, "");
   strcpy(image->description, "");
 
   root = xmlDocGetRootElement(doc);
 
   if(root->type == XML_ELEMENT_NODE) {
       /**getting content from first node**/
       if(strlen((char*)root->ns->href) > 255) { //256th should be null
            snprintf(image->namespace, 256, "%s", (char*)root->ns->href);
       }
       else { 
           int size = snprintf(image->namespace, 0, "%s", (char*)root->ns->href);
           snprintf(image->namespace, size+1, "%s", (char*)root->ns->href);
       }

       // Iterate through every attribute of the current node
       List* listOfAttrs = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
       xmlAttr *attr;
       for (attr = root->properties; attr != NULL; attr = attr->next) {
           xmlNode *value = attr->children;
              
           Attribute* svgAttr = (Attribute*)malloc(sizeof(Attribute)); //should this be stored out of for loop?
           int nameLen = strlen((char*)attr->name) + 2;
           int contLen = strlen((char*)value->content) + 2;
           svgAttr->name = (char*)malloc(sizeof(char)*nameLen);
           svgAttr->value = (char*)malloc(sizeof(char)*contLen);
           sprintf(svgAttr->name, "%s", attr->name);
           sprintf(svgAttr->value, "%s", value->content);

           insertBack(listOfAttrs, svgAttr);
 
           image->otherAttributes = listOfAttrs;
 
       }
       /**passing children to recursive function**/
       //where should linked list for rects, circles, etc be defined/initialized?
       parseNodes(root->children, image);
   }
   else {
       xmlFreeDoc(doc);
       return NULL;
   }
 
   xmlFreeDoc(doc); //is this where the doc ptr should be freed?
  
   return image;
}

SVGimage* parseNodes(xmlNode* temp, SVGimage* img) {
    xmlNode *cur_node = NULL;
    for (cur_node = temp; cur_node != NULL; cur_node = cur_node->next) {

        if (strcmp((char *)cur_node->name, "title") == 0) { //if element name is title, copy it into svg image struct
            if (cur_node->children->content != NULL) {
                if(strlen((char*)cur_node->children->content) > 255) { 
                    snprintf(img->title, 256, "%s", (char*)cur_node->children->content);
                }
                else {
                    int size = snprintf(img->title, 0, "%s", (char*)cur_node->children->content);
                    snprintf(img->title, size+1, "%s", (char*)cur_node->children->content);
                }
            }
            else {
                strcpy(img->title, "");
            }
        }
        else if (strcmp((char *)cur_node->name, "desc") == 0) { //if element name is desc, copy it into svg image struct
            if (cur_node->children->content != NULL){
                if(strlen((char*)cur_node->children->content) > 256) { 
                    snprintf(img->description, 256, "%s", (char*)cur_node->children->content);
                }
                else {
                    int size = snprintf(img->description, 0, "%s", (char*)cur_node->children->content);
                    snprintf(img->description, size+1, "%s", (char*)cur_node->children->content);
                }
            }
            else {
                strcpy(img->description, "");
            }
        }
        else if (strcmp((char *)cur_node->name, "g") == 0) { 
            Group* g = parseGroups(cur_node, img);
            insertBack(img->groups, g);
        }
        else if (strcmp((char *)cur_node->name, "rect") == 0) { //if element name is rect, store it into svg image struct
            Rectangle* rect = parseRectangles(cur_node, img);
            insertBack(img->rectangles, rect);
        }
        else if (strcmp((char *)cur_node->name, "circle") == 0) { //if element name is circle, store it into svg image struct
            Circle* circ = parseCircles(cur_node, img);
            insertBack(img->circles, circ);  
        }
        else if (strcmp((char *)cur_node->name, "path") == 0) { //if element name is path, store it into svg image struct
            Path* p = parsePaths(cur_node, img);
            insertBack(img->paths, p);  
        }
    }
    return img;
}

Rectangle* parseRectangles(xmlNode* temp, SVGimage* img) {
    Rectangle* rect = (Rectangle*)malloc(sizeof(Rectangle));
    List* rectOtherAttrs = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    rect->otherAttributes = rectOtherAttrs;
    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    strcpy(rect->units, "");
    xmlAttr* attr = NULL;
    for(attr = temp->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        if(strcmp((char *)attr->name, "x") == 0) {
            rect->x = atof((char*)value->content);
        }
        else if(strcmp((char *)attr->name, "y") == 0) {
            rect->y = atof((char*)value->content);
        }
        else if(strcmp((char *)attr->name, "width") == 0) {
            int length = strlen((char*)value->content);
            if(length > 2) {
                char* temp = (char*)malloc(sizeof(char)*(length+3));
                strcpy(temp, (char*)value->content);
                if(temp[length-2] == 'c' && temp[length-1] == 'm') {
                    strcpy(rect->units, "cm");
                }
                else if(temp[length-2] == 'm' && temp[length-1] == 'm') {
                    strcpy(rect->units, "mm");
                }
                free(temp);
            }
            rect->width = atof((char*)value->content); 
        }
        else if(strcmp((char *)attr->name, "height") == 0) {
            rect->height = atof((char*)value->content); 
        }
        else {
            Attribute* other = (Attribute*)malloc(sizeof(Attribute));
            int nameLen = strlen((char*)attr->name) + 2;
            int valLen = strlen((char*)value->content) + 2;
            other->name = (char*)malloc(sizeof(char)*nameLen);
            other->value = (char*)malloc(sizeof(char)*valLen); 
            sprintf(other->name, "%s", attr->name);
            sprintf(other->value, "%s", value->content);
            insertBack(rectOtherAttrs, other);
           
        }

    }

    return rect;
}

Circle* parseCircles(xmlNode* temp, SVGimage* img) {
    Circle* circ = (Circle*)malloc(sizeof(Circle));
    List* circOtherAttrs = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    circ->otherAttributes = circOtherAttrs;
    circ->cx = 0;
    circ->cy = 0;
    circ->r = 0;
    strcpy(circ->units, "");
    xmlAttr* attr = NULL;
    for(attr = temp->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        
        if(strcmp((char *)attr->name, "cx") == 0) {
            circ->cx = atof((char*)value->content); 
        }
        else if(strcmp((char *)attr->name, "cy") == 0) {
            circ->cy = atof((char*)value->content);
        }
        else if(strcmp((char *)attr->name, "r") == 0) {
            int length = strlen((char*)value->content);
            if(length > 2) {
                char* temp = (char*)malloc(sizeof(char)*(length+3));
                strcpy(temp, (char*)value->content);
                if(temp[length-2] == 'c' && temp[length-1] == 'm') {
                    strcpy(circ->units, "cm");
                }
                else if(temp[length-2] == 'm' && temp[length-1] == 'm') {
                    strcpy(circ->units, "mm");
                }
                free(temp);
            }
            circ->r = atof((char*)value->content);
        }
        else {
            Attribute* other = (Attribute*)malloc(sizeof(Attribute));
            int nameLen = strlen((char*)attr->name) + 2;
            int valLen = strlen((char*)value->content) + 2;
            other->name = (char*)malloc(sizeof(char)*nameLen);
            other->value = (char*)malloc(sizeof(char)*valLen); 
            sprintf(other->name, "%s", attr->name);
            sprintf(other->value, "%s", value->content);
            insertBack(circOtherAttrs, other);
        }

    }
    return circ;
}

Path* parsePaths(xmlNode* temp, SVGimage* img) {
    Path* path = (Path*)malloc(sizeof(Path));
    List* pathOtherAttrs = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    path->otherAttributes = pathOtherAttrs;

    xmlAttr* attr = NULL;
    for(attr = temp->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;

        if(strcmp((char *)attr->name, "d") == 0) {
            int dataLen = strlen((char*)value->content)+2;
            path->data = (char*)malloc(sizeof(char)*dataLen); 
            if((char*)value->content == NULL) {
                strcpy(path->data, "");
            }
            else{
                sprintf(path->data, "%s", (char*)value->content);
            }
            
        }
        else {
            Attribute* other = (Attribute*)malloc(sizeof(Attribute));
            int nameLen = strlen((char*)attr->name) + 2;
            int valLen = strlen((char*)value->content) + 2;
            other->name = (char*)malloc(sizeof(char)*nameLen);
            other->value = (char*)malloc(sizeof(char)*valLen); 
            sprintf(other->name, "%s", attr->name);
            sprintf(other->value, "%s", value->content);
            insertBack(pathOtherAttrs, other);
        }

    }
    return path;
}

Group* parseGroups(xmlNode* temp, SVGimage* img) {
    Group* group = (Group*)malloc(sizeof(Group));
    List* groupOtherAttrs = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    group->otherAttributes = groupOtherAttrs;
    group->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    group->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    group->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    group->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);

    xmlAttr* attr = NULL;
    for(attr = temp->properties; attr != NULL; attr = attr->next) {

        xmlNode *value = attr->children;
        Attribute* other = (Attribute*)malloc(sizeof(Attribute));
        int nameLen = strlen((char*)attr->name) + 2;
        int valLen = strlen((char*)value->content) + 2;
        other->name = (char*)malloc(sizeof(char)*nameLen);
        other->value = (char*)malloc(sizeof(char)*valLen); 
        sprintf(other->name, "%s", attr->name);
        sprintf(other->value, "%s", value->content);
        insertBack(groupOtherAttrs, other);
    }

    xmlNode *cur_node = NULL;
    for (cur_node = temp->children; cur_node != NULL; cur_node = cur_node->next) {

        if (strcmp((char *)cur_node->name, "g") == 0) { 
            Group* temp = parseGroups(cur_node, img);
            insertBack(group->groups, temp);
        }
        else if (strcmp((char *)cur_node->name, "rect") == 0) { //if element name is rect, store it into svg image struct
            Rectangle* temp = parseRectangles(cur_node, img);
            insertBack(group->rectangles, temp);
        }
        else if (strcmp((char *)cur_node->name, "circle") == 0) { //if element name is circle, store it into svg image struct
            Circle* temp = parseCircles(cur_node, img);
            insertBack(group->circles, temp);
        }
        else if (strcmp((char *)cur_node->name, "path") == 0) { //if element name is path, store it into svg image struct
            Path* temp = parsePaths(cur_node, img);
            insertBack(group->paths, temp);
        }
        
    }
    return group;
    
}

/** Function to delete image content and free all the memory.
 *@pre SVGimgage  exists, is not null, and has not been freed
 *@post SVSVGimage had been freed
 *@return none
 *@param obj - a pointer to an SVG struct
**/
void deleteSVGimage(SVGimage* img) {

    if(img == NULL){
        return;
    }

    if(img->rectangles != NULL) {
        freeList(img->rectangles);
    }
    if(img->circles != NULL) {
        freeList(img->circles);
    }
    if(img->groups != NULL) {
        freeList(img->groups);
    }
    if(img->paths != NULL) {
        freeList(img->paths);      
    }
    if(img->otherAttributes != NULL) {
        freeList(img->otherAttributes);
    }
    free(img); //freeing struct
}

/** Function to create a string representation of an SVG object.
 *@pre SVGimgage exists, is not null, and is valid
 *@post SVGimgage has not been modified in any way, and a string representing the SVG contents has been created
 *@return a string contaning a humanly readable representation of an SVG object
 *@param obj - a pointer to an SVG struct
**/
char* SVGimageToString(SVGimage* img) {
    char* svgString = NULL; //NS, TITLE
    int len = 0;
    
    len += strlen(img->namespace); 
    len += strlen(img->title);
    len += strlen(img->description);

    char* pathsList = toString(img->paths);
    len += strlen(pathsList) + 2;
    
    svgString = (char*)malloc(sizeof(char)*len + 200);
    sprintf(svgString, "SVG namespace: %s, title: %s, description: %s\nPaths linked list: %s\n", img->namespace, img->title, img->description, pathsList);
    free(pathsList);
    return svgString;
}

int compareAttributes(const void *first, const void *second){  
    return 0;
}

char* attributeToString(void* data) {
    Attribute* this = data;
    char* attrStr = NULL;
    int nameLen = 0;
    int valueLen = 0;

    nameLen = strlen(this->name);
    valueLen = strlen(this->value);
    attrStr = (char*)malloc(sizeof(char)*(nameLen + valueLen + 25));

    sprintf(attrStr, "Name: %s, Value: %s\n", this->name, this->value);
    return attrStr;
}

void deleteAttribute(void* data) {
    if(data != NULL) {
        Attribute* this = data;
        free(this->name);
        free(this->value);
        free(this);
    }
    return;
}

void deleteGroup(void* data) {
    if(data != NULL) {
        Group* this = data;
        freeList(this->rectangles);
        freeList(this->circles);
        freeList(this->paths);
        freeList(this->groups);
        freeList(this->otherAttributes);
        free(this);
    }
    return;
}

void deleteDummyGroup(void* data) {
    return;
}

char* groupToString(void* data) {
    //Group* this = data;
    char* groupStr = NULL;
    groupStr = (char*)malloc(sizeof(char)*(6));
    strcpy(groupStr, "test");
    return groupStr;
}

int compareGroups(const void *first, const void *second) {

    return 0;
}

//RECTANGLES
void deleteRectangle(void* data) {
    if(data != NULL) {
        Rectangle* this = data;
        freeList(this->otherAttributes);
        free(this);
    }
    return;
}

void deleteDummyRectangle(void* data) {

    return;
}

char* rectangleToString(void* data) {
    //Rectangle* this = data;
    char* rectStr = NULL;
    rectStr = (char*)malloc(sizeof(char)*(6));
    strcpy(rectStr, "test");
    return rectStr;
}

int compareRectangles(const void *first, const void *second) {

    return 0;
}

void deleteCircle(void* data) {
    if(data != NULL) {
        Circle* this = data;
        freeList(this->otherAttributes);
        free(this);
    }
    return;
}

void deleteDummyCircle(void* data) {

    return;
}


char* circleToString(void* data) {
    Circle* this = data;
    char* circStr = NULL;
    
    char* otherAttrsStr = toString(this->otherAttributes); 
    int otherAttributesLength = strlen(otherAttrsStr) + 2;

    circStr = (char*)malloc(sizeof(char)*(otherAttributesLength + 100));
    sprintf(circStr, "Circle cx: %f, cy: %f\nOther attributes list: %s\n", this->cx, this->cy, otherAttrsStr);
    free(otherAttrsStr);

    return circStr;
}

int compareCircles(const void *first, const void *second) {

    return 0;
}

void deletePath(void* data) {
    if(data != NULL) {
        Path* this = data;
        free(this->data);
        freeList(this->otherAttributes);
        free(this);
    }
    return;
}

void deleteDummyPath(void* data) {
    return;
}

char* pathToString(void* data) {
    Path* this = data;
    char* pathStr = NULL;
    
    int valueLen = strlen(this->data) + 2;

    char* otherAttrsStr = toString(this->otherAttributes); 
    int otherAttributesLength = strlen(otherAttrsStr) + 2;

    pathStr = (char*)malloc(sizeof(char)*(valueLen + otherAttributesLength + 50));
    sprintf(pathStr, "Path data: %s, Other attributes: %s\n", this->data, otherAttrsStr);
    free(otherAttrsStr);

    return pathStr;
}

int comparePaths(const void *first, const void *second) {


    return 0;
}

void thingsFromNestedGroups(List* allThings, Group* groupElement, int num) {
    //make iterator for groupElement->groups
    //for each group within the group Element, check recursively if it has groups within
    ListIterator itrGroups = createIterator(groupElement->groups);
    Group* outerG = nextElement(&itrGroups);
    while(outerG != NULL) {
        //case 1: handle rectangles
        if(num == 1) {
            if(outerG->rectangles != NULL && getLength(outerG->rectangles) > 0){
                ListIterator itrRects = createIterator(outerG->rectangles);
                Rectangle* innerRect = nextElement(&itrRects);
                while(innerRect != NULL){
                    insertBack(allThings, innerRect);
                    innerRect = nextElement(&itrRects);
                }
            }
        }
        //case 2: handle circles
        if(num == 2) {
            if(outerG->circles != NULL && getLength(outerG->circles) > 0){
                ListIterator itrCircles = createIterator(outerG->circles);
                Circle* innerCircle = nextElement(&itrCircles);
                while(innerCircle != NULL){
                    insertBack(allThings, innerCircle);
                    innerCircle = nextElement(&itrCircles);
                }
            }
        }
        //case 3: handle paths
        if(num == 3) {
            if(outerG->paths != NULL && getLength(outerG->paths) > 0){
                ListIterator itrPaths = createIterator(outerG->paths);
                Path* innerPath = nextElement(&itrPaths);
                while(innerPath != NULL){
                    insertBack(allThings, innerPath);
                    innerPath = nextElement(&itrPaths);
                }
            }
        }
        //case 4: handle groups
        if(num == 4){
            insertBack(allThings, outerG);
        }
        
        if(outerG->groups != NULL && getLength(outerG->groups) > 0) {
            thingsFromNestedGroups(allThings, outerG, num);
        }
        
        outerG = nextElement(&itrGroups);
    }
}

// Function that returns a list of all rectangles in the image.  
List* getRects(SVGimage* img) {
    List* allRects = initializeList(&rectangleToString, &deleteDummyRectangle, &compareRectangles);

    if(img != NULL) {
        ListIterator itrRects = createIterator(img->rectangles);
        ListIterator itrGroups = createIterator(img->groups);

        //Collecting first layer rectangles
        Rectangle* outerRect = nextElement(&itrRects);
        while(outerRect != NULL) {
            insertBack(allRects, outerRect);
            outerRect = nextElement(&itrRects);
        }
        
        //Collecting rectangles within groups
        Group* groupElement = nextElement(&itrGroups);
        while(groupElement != NULL) {
            //if current group has rectangles, parse through list, add them to allRects
            if(groupElement->rectangles != NULL && getLength(groupElement->rectangles) > 0) {

                ListIterator itrGroupRects = createIterator(groupElement->rectangles);
                Rectangle* innerRect = nextElement(&itrGroupRects);

                while(innerRect != NULL) {
                    insertBack(allRects, innerRect);
                    innerRect = nextElement(&itrGroupRects);
                }
            } 
            if(groupElement->groups != NULL && getLength(groupElement->groups) > 0) {
                thingsFromNestedGroups(allRects, groupElement, 1);
            }

            //if current groups has groups, call function to check if that group has groups etc recursively...
            groupElement = nextElement(&itrGroups);
        }

    }

    return allRects;
}

// Function that returns a list of all circles in the image.  
List* getCircles(SVGimage* img) {
   
    List* allCircles = initializeList(&circleToString, &deleteDummyCircle, &compareCircles);
    
    if(img != NULL) {
        ListIterator itrCircles = createIterator(img->circles);
        ListIterator itrGroups = createIterator(img->groups);

        //Collecting first layer circles
        Circle* outerCircle = nextElement(&itrCircles);
        while(outerCircle != NULL) {
            insertBack(allCircles, outerCircle);
            outerCircle = nextElement(&itrCircles);
        }

        
        //Collecting rectangles within groups
        Group* groupElement = nextElement(&itrGroups);
        while(groupElement != NULL) {
            //if current group has rectangles, parse through list, add them to allRects
            if(groupElement->circles != NULL && getLength(groupElement->circles) > 0) {

                ListIterator itrGroupCircles = createIterator(groupElement->circles);
                Circle* innerCircle = nextElement(&itrGroupCircles);

                while(innerCircle != NULL) {
                    insertBack(allCircles, innerCircle);
                    innerCircle = nextElement(&itrGroupCircles);
                }
            } 
            if(groupElement->groups != NULL && getLength(groupElement->groups) > 0) {
                thingsFromNestedGroups(allCircles, groupElement, 2);
            }

            //if current groups has groups, call function to check if that group has groups etc recursively...
            groupElement = nextElement(&itrGroups);
        }

        
    }
    
    return allCircles;
}
// Function that returns a list of all groups in the image.  
List* getGroups(SVGimage* img) {
    List* allGroups = initializeList(&groupToString, &deleteDummyGroup, &compareGroups);
    
    if(img != NULL) {
        ListIterator itrFirstGroups = createIterator(img->groups);
        Group* outerGroup = nextElement(&itrFirstGroups);
        while(outerGroup != NULL) {
            insertBack(allGroups, outerGroup);
            if(outerGroup->groups != NULL && getLength(outerGroup->groups) > 0) {
                thingsFromNestedGroups(allGroups, outerGroup, 4);
            }
            outerGroup = nextElement(&itrFirstGroups);
        }

    }
    return allGroups;
}
// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img) {
    List* allPaths = initializeList(&pathToString, &deleteDummyPath, &comparePaths);
    
    if(img != NULL) {
        ListIterator itrPaths = createIterator(img->paths);
        ListIterator itrGroups = createIterator(img->groups);

        //Collecting first layer circles
        Circle* outerPath = nextElement(&itrPaths);
        while(outerPath != NULL) {
            insertBack(allPaths, outerPath);
            outerPath = nextElement(&itrPaths);
        }
        
        //Collecting rectangles within groups
        Group* groupElement = nextElement(&itrGroups);
        while(groupElement != NULL) {
            //if current group has rectangles, parse through list, add them to allRects
            if(groupElement->paths != NULL && getLength(groupElement->paths) > 0) {
                ListIterator itrGroupPaths = createIterator(groupElement->paths);
                Circle* innerPath = nextElement(&itrGroupPaths);

                while(innerPath != NULL) {
                    insertBack(allPaths, innerPath);
                    innerPath = nextElement(&itrGroupPaths);
                }
            } 
            if(groupElement->groups != NULL && getLength(groupElement->groups) > 0) {
                thingsFromNestedGroups(allPaths, groupElement, 3);
            }

            //if current groups has groups, call function to check if that group has groups etc recursively...
            groupElement = nextElement(&itrGroups);
        }
        
    }
    return allPaths;
}

// Function that returns the number of all rectangles with the specified area
int numRectsWithArea(SVGimage* img, float area) {
    int numRects = 0;
    int a = ceil(area);
    float w = 0;
    float h = 0;

    if(img != NULL && area > 0) {
        List* allRects = getRects(img);
        ListIterator rectsItr = createIterator(allRects);
        Rectangle* curRect = nextElement(&rectsItr);
        while(curRect != NULL) {
            if(curRect->width > 0 && curRect->height > 0) {
                w = curRect->width;
                h = curRect->height;
                int compare = ceil(w*h);
                if(a == compare) {
                    numRects++;
                }
            }
            curRect = nextElement(&rectsItr);
        }
        freeList(allRects);
    }
    
    return numRects;
}
// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
    int numCircles = 0;
    
    int a = ceil(area);
    float r = 0;

    if(img != NULL && area > 0) {
        List* allCircles = getCircles(img);
        ListIterator circlesItr = createIterator(allCircles);
        Circle* curCircle = nextElement(&circlesItr);
        while(curCircle != NULL) {
            if(curCircle->r > 0) {
                r = curCircle->r;
                int compare = ceil(3.14159265359*r*r);
                if(a == compare) {
                    numCircles++;
                }
            }
            curCircle = nextElement(&circlesItr);
        }
        freeList(allCircles);
    }

    return numCircles;
}
// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {

    int numPaths = 0;

    if(img != NULL && data != NULL) {
        List* allPaths = getPaths(img);
        ListIterator pathsItr = createIterator(allPaths);
        Path* curPath = nextElement(&pathsItr);
        while(curPath != NULL) {
            if(curPath->data != NULL) {
                if(strcmp(curPath->data, data) == 0) {
                    numPaths++;
                }
            }
            curPath = nextElement(&pathsItr);
        }
        freeList(allPaths);
    }

    return numPaths;
}
// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len) {
    int numGroups = 0;
    int totalLength = 0;
    if(img != NULL && len >= 0) {
        List* allGroups = getGroups(img);
        ListIterator groupsItr = createIterator(allGroups);
        Group* curGroup = nextElement(&groupsItr);
        while(curGroup != NULL) {
            totalLength += getLength(curGroup->rectangles);
            totalLength += getLength(curGroup->circles);
            totalLength += getLength(curGroup->paths);
            totalLength += getLength(curGroup->groups);
            if(len == totalLength) {
                numGroups++;
            }
            totalLength = 0;
            curGroup = nextElement(&groupsItr);
        }
        freeList(allGroups);
    } 
    return numGroups;
}

/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.  
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img) {
    int totalAttr = 0;
    //doc = xmlReadDoc(img) ... if doc == NULL return 0? clear doc
    //how to check if it hasnt been freed?
    if(img != NULL) {
        
        if(img->otherAttributes != NULL) {
            totalAttr += getLength(img->otherAttributes);
        }
        
        List* allRects = getRects(img);
        ListIterator rectsItr = createIterator(allRects);
        Rectangle* curRect = nextElement(&rectsItr);
        while(curRect != NULL) {
            if(curRect->otherAttributes != NULL) {
                totalAttr += getLength(curRect->otherAttributes);
            }
            curRect = nextElement(&rectsItr);
        }
        
        List* allCircles = getCircles(img);
        ListIterator circlesItr = createIterator(allCircles);
        Circle* curCircle = nextElement(&circlesItr);
        while(curCircle != NULL) {
            if(curCircle->otherAttributes != NULL){
                totalAttr += getLength(curCircle->otherAttributes);
            }
            curCircle = nextElement(&circlesItr);
        }

        List* allPaths = getPaths(img);
        ListIterator pathsItr = createIterator(allPaths);
        Path* curPath = nextElement(&pathsItr);
        while(curPath != NULL) {
            if(curPath->otherAttributes != NULL) {
                totalAttr += getLength(curPath->otherAttributes);
            }
            curPath = nextElement(&pathsItr);
        }
        
        List* allGroups = getGroups(img);
        ListIterator groupsItr = createIterator(allGroups);
        Group* curGroup = nextElement(&groupsItr);
        while(curGroup != NULL) {
            if(curGroup->otherAttributes != NULL) {
                totalAttr += getLength(curGroup->otherAttributes);
            }
            curGroup = nextElement(&groupsItr);
        }

        freeList(allRects);
        freeList(allCircles);
        freeList(allPaths);
        freeList(allGroups);
    }
    return totalAttr;
}

/** Helper function for setAttribute. Searches through structs'
    otherAttributes. If there is a match, it replaces the content of the
    attribute with newAttribute->value and returns true. If no match, it returns
    false, so the attribute can be appended to the list in the function that 
    called this function.
**/
bool setOtherAttr(ListIterator iterAttrs, Attribute* newAttribute) {
    Attribute* curAttr = nextElement(&iterAttrs);
    bool inserted = false;
    while(curAttr != NULL) {
        if(strcmp(curAttr->name, newAttribute->name) == 0) {
            free(curAttr->value); //if preexisting, free previous malloc, re malloc
            curAttr->value = (char*)malloc(sizeof(char)*strlen(newAttribute->value)+2); 
            strcpy(curAttr->value, newAttribute->value);
            inserted = true;
        }
        curAttr = nextElement(&iterAttrs);
    }
    return inserted;
}

/** Function to setting an attribute in an SVGimage or component
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newAttribute is not NULL
 *@post The appropriate attribute was set corectly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    elemIndex - index of thje lement to modify
    newAttribute - struct containing name and value of the updated attribute
 **/
void setAttribute(SVGimage* image, elementType elemType, int elemIndex, Attribute* newAttribute) {
    //error checking?
    if(newAttribute == NULL || image == NULL) return;

    if(newAttribute->name == NULL || newAttribute->value == NULL) return;

    if(elemType == SVG_IMAGE) {
        //ignore index
        if(strcmp(newAttribute->name, "title") == 0){ //can title/desc be null??
            if(strlen(newAttribute->value) > 255) { //does 256 include terminating char \0?
                //truncate then copy
                snprintf(image->title, 256, "%s", (char*)newAttribute->value);
            } else {
                int size = snprintf(image->title, 0, "%s", (char*)newAttribute->value);
                snprintf(image->title, size+1, "%s", (char*)newAttribute->value);
            }
        }
        else if(strcmp(newAttribute->name, "desc") == 0){ //case sensitive??
            if(strlen(newAttribute->value) > 255) { //does 256 include terminating char \0?
                //truncate then copy
                snprintf(image->description, 256, "%s", (char*)newAttribute->value);
            } else {
                int size = snprintf(image->description, 0, "%s", (char*)newAttribute->value);
                snprintf(image->description, size+1, "%s", (char*)newAttribute->value);
            }
        } 
        else {
            if(image->otherAttributes == NULL) return; //unsuccessful
            else if(getLength(image->otherAttributes) > 0) {
                ListIterator iterAttrs = createIterator(image->otherAttributes);
                bool inserted = setOtherAttr(iterAttrs, newAttribute);
                if(!inserted) { //no match found in pre-existing attributes, append newAttr
                    insertBack(image->otherAttributes, newAttribute);
                    return;
                }
                else { //match found in pre-existing attrs, free newAttr
                    deleteAttribute(newAttribute);
                    return;
                }
            }
            else { //no pre-existing attrs, add as first
                insertBack(image->otherAttributes, newAttribute);
                return;
            }
        }
        
    }
    else if(elemType == RECT) {
        //validate index
        //only free newAttribute for successful returns
        if(image->rectangles == NULL) return; //unsuccessful

        int numRects = 0;
        numRects = getLength(image->rectangles);
        //free attribute for these early returns?? no...
        if(numRects < 1 || elemIndex > numRects || elemIndex < 0) {
            return; //unsuccessful
        }
        
        int count = 0;
        ListIterator iterRects = createIterator(image->rectangles);
        Rectangle* curRect = nextElement(&iterRects);

        float val = 0;
        char* ptr;
        while(curRect != NULL) {
            if(elemIndex == count) {
                if(strcmp(newAttribute->name, "x") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    curRect->x = val;
                }
                else if(strcmp(newAttribute->name, "y") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    curRect->y = val;
                }
                else if(strcmp(newAttribute->name, "width") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    else if(val < 0) { //cannot be negative
                        return;
                    }
                    curRect->width = val;
                }
                else if(strcmp(newAttribute->name, "height") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    else if(val < 0) {  //cannot be negative
                        return;
                    }
                    curRect->height = val;
                } //can user change the units?? if name == "units" ... dont think so
                else {
                    //otherAttrs
                    //should I assume rect->otherAttributes is not NULL? if it is, should i initialize it to add a new attribute to it?
                    if(curRect->otherAttributes == NULL) return; //unsuccessful
                    else if(getLength(curRect->otherAttributes) > 0) {
                        ListIterator iterAttrs = createIterator(curRect->otherAttributes); 
                        bool inserted = setOtherAttr(iterAttrs, newAttribute); //check if newAttr's name matches pre-existing attrs
                        if(!inserted) { //no match found for pre-existing otherAttributes
                            insertBack(curRect->otherAttributes, newAttribute);
                            return;
                        }
                        else { //match found, free
                            deleteAttribute(newAttribute); 
                            return; //success
                        }
                    }
                    else { //no pre-existing otherAttributes to compare, just add as first in list
                        insertBack(curRect->otherAttributes, newAttribute);
                        return;
                    }
                }
            }
            curRect = nextElement(&iterRects);
            count++;
        }

    } 
    else if(elemType == CIRC) {
        //validate index
        //only free newAttribute for successful returns
        if(image->circles == NULL) return; //unsuccessful

        int numCircs = 0;
        numCircs = getLength(image->circles);
        if(numCircs < 1 || elemIndex > numCircs || elemIndex < 0) {
            return; //unsuccessful
        }
        
        int count = 0;
        ListIterator iterCircs = createIterator(image->circles);
        Circle* curCirc = nextElement(&iterCircs);

        float val = 0;
        char* ptr;
        while(curCirc != NULL) {
            if(elemIndex == count) {
                if(strcmp(newAttribute->name, "cx") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    curCirc->cx = val;
                }
                else if(strcmp(newAttribute->name, "cy") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    curCirc->cy = val;
                }
                else if(strcmp(newAttribute->name, "r") == 0){
                    val = strtof(newAttribute->value, &ptr);
                    if(val == 0 && strcmp(ptr, "") != 0 && strcmp(ptr, "cm") != 0 && strcmp(ptr, "mm") != 0) { //invalid, strtof could not convert to float
                        return; //unsuccessful
                    }
                    else if(val < 0) {
                        return;
                    }
                    curCirc->r = val;
                }//can user change the units?? if name == "units"
                else {
                    //otherAttrs
                    //should I assume rect->otherAttributes is not NULL? if it is, should i initialize it to add a new attribute to it?
                    if(curCirc->otherAttributes == NULL) return; //unsuccessful
                    else if(getLength(curCirc->otherAttributes) > 0) {
                        ListIterator iterAttrs = createIterator(curCirc->otherAttributes); 
                        bool inserted = setOtherAttr(iterAttrs, newAttribute); //check if newAttr's name matches pre-existing attrs
                        if(!inserted) { //no match found for pre-existing otherAttributes
                            insertBack(curCirc->otherAttributes, newAttribute);
                            return; //dont free
                        }
                        else { //matching attr found, dont need newAttr after copying its content
                            deleteAttribute(newAttribute); 
                            return; //success
                        }
                    }
                    else { //no pre-existing otherAttributes to compare, just add as first in list
                        insertBack(curCirc->otherAttributes, newAttribute);
                        return; //dont free
                    }
                }
            }
            curCirc = nextElement(&iterCircs);
            count++;
        }
    }
    else if(elemType == PATH) {
        //validate index
        //only free newAttribute for successful returns
        if(image->paths == NULL) return; //unsuccessful

        int numPaths = 0;
        numPaths = getLength(image->paths);
        if(numPaths < 1 || elemIndex > numPaths || elemIndex < 0) {
            return; //unsuccessful
        }
        
        int count = 0;
        ListIterator iterPaths = createIterator(image->paths);
        Path* curPath = nextElement(&iterPaths);
        while(curPath != NULL) {
            if(elemIndex == count) {
                if(strcmp(newAttribute->name, "d") == 0){ //should it be "d" or "data"
                    if(curPath->data != NULL) { //should i just return if its null??
                        free(curPath->data);
                    }
                    /*else if(strlen(newAttribute->value) > 64) { //dont truncate???
                        curPath->data = (char*)malloc(sizeof(char)*66);
                        strncpy(curPath->data, newAttribute->value, 64);
                    }*/
                    // else {
                    //should i return if data is NULL?
                    curPath->data = (char*)malloc(sizeof(char)*strlen(newAttribute->value)+2); 
                    strcpy(curPath->data, newAttribute->value);
                    // }
                }
                else {
                    //otherAttrs
                    //should I assume rect->otherAttributes is not NULL? if it is, should i initialize it to add a new attribute to it?
                    if(curPath->otherAttributes == NULL) return; //unsuccessful
                    else if(getLength(curPath->otherAttributes) > 0) {
                        ListIterator iterAttrs = createIterator(curPath->otherAttributes); 
                        bool inserted = setOtherAttr(iterAttrs, newAttribute); //check if newAttr's name matches pre-existing attrs
                        if(!inserted) { //no match found for pre-existing otherAttributes
                            insertBack(curPath->otherAttributes, newAttribute);
                            return; 
                        } 
                        else {
                            deleteAttribute(newAttribute); 
                            return; //success, match found, free newAttr
                        }
                    }
                    else { //no pre-existing otherAttributes to compare, just add as first in list
                        insertBack(curPath->otherAttributes, newAttribute);
                        return; //successful, don't free newAttr
                    }
                }
            }
            curPath = nextElement(&iterPaths);
            count++;
        }
    }
    else if(elemType == GROUP) {
        //validate index
        //only free newAttribute for successful returns
        if(image->groups == NULL) return; //unsuccessful

        int numGroups = 0;
        numGroups = getLength(image->groups);
        if(numGroups < 1 || elemIndex > numGroups || elemIndex < 0) {
            return; //unsuccessful
        }
        
        int count = 0;
        ListIterator iterGroups = createIterator(image->groups);
        Group* curGroup = nextElement(&iterGroups);
        
        while(curGroup != NULL) {
            if(elemIndex == count) {
                //otherAttrs
                //should I assume rect->otherAttributes is not NULL? if it is, should i initialize it to add a new attribute to it?
                if(curGroup->otherAttributes == NULL) return; //unsuccessful
                else if(getLength(curGroup->otherAttributes) > 0) {
                    ListIterator iterAttrs = createIterator(curGroup->otherAttributes); 
                    bool inserted = setOtherAttr(iterAttrs, newAttribute); //check if newAttr's name matches pre-existing attrs
                    if(!inserted) { //no match found for pre-existing otherAttributes
                        insertBack(curGroup->otherAttributes, newAttribute);
                        return; //successful, dont free newAttr
                    }
                    else {
                        deleteAttribute(newAttribute); //matching attr found, so free
                        return; //success
                    }
                }
                else { //no pre-existing otherAttributes to compare, just add as first in list
                    insertBack(curGroup->otherAttributes, newAttribute);
                    return; //successful, dont free newAttr
                }
            }
            curGroup = nextElement(&iterGroups);
            count++;
        }
    }

    deleteAttribute(newAttribute);

    return; //success
}

/** Function to adding an element - Circle, Rectngle, or Path - to an SVGimage
 *@pre
    SVGimage object exists, is valid, and and is not NULL.
    newElement is not NULL
 *@post The appropriate element was added correctly
 *@return N/A
 *@param
    image - a pointer to an SVGimage struct
    elemType - enum value indicating elemtn to modify
    newElement - pointer to the element sgtruct (Circle, Rectngle, or Path)
 **/
void addComponent(SVGimage* image, elementType type, void* newElement) {
    if(image == NULL || newElement == NULL) return;

    if(type == RECT) {
        if(image->rectangles == NULL) return; //? or should we initialize it
        //what if newElement does not match type?
        Rectangle* rect = newElement;
        //return if any of the values are invalid? what is x and y are unitialized?
        if(rect->width < 0 || rect->height < 0 || rect->otherAttributes == NULL) { //check otherAttrs initialization?
            return; 
        }
        insertBack(image->rectangles, rect);
    }
    else if(type == CIRC) {
        if(image->circles == NULL) return; //? or should we initialize it
        Circle* circ = newElement;
        //return if any of the values are invalid? what is x and y are unitialized?
        if(circ->r < 0 || circ->otherAttributes == NULL) { //check otherAttrs initialization?
            return; 
        }
        insertBack(image->circles, circ);
    }
    else if(type == PATH) {
        if(image->paths == NULL) return;
        Path* path = newElement;
        if(path->data == NULL || path->otherAttributes == NULL) {
            return; 
        }
        insertBack(image->paths, path);
    }

    return;
}

/** Function to converting an Attribute into a JSON string
*@pre Attribute is not NULL
*@post Attribute has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an Attribute struct
**/
char* attrToJSON(const Attribute *a) {
    char* toReturn = NULL;
    if(a == NULL || a->name == NULL || a->value == NULL) { //test each
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*(strlen(a->name)+strlen(a->value)+35));
        //{"name":"attrName","value":"attrVal"}
        int size = snprintf(toReturn, 0, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
        snprintf(toReturn, size+2, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
    }

    return toReturn;
}

/** Function to converting a Circle into a JSON string
*@pre Circle is not NULL
*@post Circle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Circle struct
**/
char* circleToJSON(const Circle *c) {
    char* toReturn = NULL;
    if(c == NULL || c->otherAttributes == NULL) { //should i check if otherAttributes is not null?
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*(strlen(c->units)+70)); //4 for each float/int
        int numAttr = getLength(c->otherAttributes);
        //{"cx":xVal,"cy":yVal,"r":rVal,"numAttr":attVal,"units":"unitStr"}
        //do one for with units and one without
        int size = 0;
        if(c->units[0] != '\0') { //add units
            size = snprintf(toReturn, 0, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, numAttr, c->units);
            snprintf(toReturn, size+2, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", c->cx, c->cy, c->r, numAttr, c->units);
        }
        else { //no units
            size = snprintf(toReturn, 0, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"\"}", c->cx, c->cy, c->r, numAttr);
            snprintf(toReturn, size+2, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"\"}", c->cx, c->cy, c->r, numAttr);
        }

    }
    return toReturn;
}

/** Function to converting a Rectangle into a JSON string
*@pre Rectangle is not NULL
*@post Rectangle has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Rectangle struct
**/
char* rectToJSON(const Rectangle *r) {
    char* toReturn = NULL;
    if(r == NULL || r->otherAttributes == NULL) { //should i check if otherAttributes is not null?
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*(strlen(r->units)+75)); 
        int numAttr = getLength(r->otherAttributes);
        //{"x":xVal,"y":yVal,"w":wVal,"h":hVal,"numAttr":attVal,"units":"unitStr"}
        //ex: {"x":1,"y":2,"w":19,"h":15,"numAttr":3,"units":"cm"}
        int size = 0;
        if(r->units[0] != '\0') { //add units
            size = snprintf(toReturn, 0, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, numAttr, r->units);
            snprintf(toReturn, size+2, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\"}", r->x, r->y, r->width, r->height, numAttr, r->units);
        }
        else { //no units
            size = snprintf(toReturn, 0, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"\"}", r->x, r->y, r->width, r->height, numAttr);
            snprintf(toReturn, size+2, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"\"}", r->x, r->y, r->width, r->height, numAttr);
        }

    }
    return toReturn;
}

/** Function to converting a Path into a JSON string
*@pre Path is not NULL
*@post Path has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Path struct
**/
char* pathToJSON(const Path *p) {
    char* toReturn = NULL;
    if(p == NULL || p->data == NULL || p->otherAttributes == NULL) { //should i check if otherAttributes is not null?
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*(strlen(p->data)+60)); //4 for each float/int
        int numAttr = getLength(p->otherAttributes);
        //{"d":"dVal","numAttr":attVal}
        //{"d":"m47 36c-15 0-15 0-29.9 0-2.1 0-2.1 4-.1 4 10.4 0 19.6 0 30 0 2 0","numAttr":1}

        int size = 0;
        char pData[65];
        if(strlen(p->data) > 64) { //truncate over 64 characters
            snprintf(pData, 65, "%s", p->data);
            size = snprintf(toReturn, 0, "{\"d\":\"%s\",\"numAttr\":%d}", pData, numAttr);
            snprintf(toReturn, size+2, "{\"d\":\"%s\",\"numAttr\":%d}", pData, numAttr);
        }
        else {
            size = snprintf(toReturn, 0, "{\"d\":\"%s\",\"numAttr\":%d}", p->data, numAttr);
            snprintf(toReturn, size+2, "{\"d\":\"%s\",\"numAttr\":%d}", p->data, numAttr);
        }

    }
    return toReturn;
}

/** Function to converting a Group into a JSON string
*@pre Group is not NULL
*@post Group has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a Group struct
**/
char* groupToJSON(const Group *g) {
    char* toReturn = NULL;
    if(g == NULL || g->rectangles == NULL || g->circles == NULL || g->paths == NULL || g->groups == NULL || g->otherAttributes == NULL) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*55);
        int numAttr = getLength(g->otherAttributes);
        int numChildren = getLength(g->rectangles);
        numChildren += getLength(g->circles);
        numChildren += getLength(g->paths);
        numChildren += getLength(g->groups);
        //ex: {"children":2,"numAttr":1}

        int size = 0;
        size = snprintf(toReturn, 0, "{\"children\":%d,\"numAttr\":%d}", numChildren, numAttr);
        snprintf(toReturn, size+2, "{\"children\":%d,\"numAttr\":%d}", numChildren, numAttr);

    }
    return toReturn;
}

/** Function to converting a list of Attribute structs into a JSON string
*@pre Attribute list is not NULL
*@post Attribute list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* attrListToJSON(const List *list) {
    char* toReturn = NULL;
    List* attrs = (List*)list;
    if(attrs == NULL || getLength(attrs) < 1) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "[]");
    }
    else {
        //toReturn = (char*)malloc(sizeof(char)*55); malloc all strlens
        //ex: [{"name":"fill","value":"none"},{"name":"stroke","value":"blue"},{"name":"stroke- width","value":"1"}]
        ListIterator iterAttrs = createIterator(attrs); 
        Attribute* curAttr = nextElement(&iterAttrs); //assume it will be attributes? error check?
        char start[3];
        char* temp;
        char* temp2;
        strcpy(start, "[");
        int toMalloc = 2;
        int size = 0;
        List* attrJS = initializeList(&attributeToString, &deleteJSAttrList, &compareAttributes); //dummies except delete...
        int length = getLength(attrs);
        int i = 1;
        while(curAttr != NULL) {
            temp = attrToJSON(curAttr);
            temp2 = (char*)malloc(sizeof(char)*(strlen(temp)+8));
            if(i == length) { //last element, leave out comma
                size = snprintf(temp2, 0, "%s]", temp);
                snprintf(temp2, size+1, "%s]", temp);
            } 
            else {
                size = snprintf(temp2, 0, "%s,", temp);
                snprintf(temp2, size+1, "%s,", temp);
            }
            free(temp);
            toMalloc += size+1;
            insertBack(attrJS, temp2);
            i++;
            curAttr = nextElement(&iterAttrs);
        }
        toReturn = (char*)malloc(sizeof(char)*(toMalloc+30));
        snprintf(toReturn, 3, start);
        ListIterator iterJSAttrs = createIterator(attrJS);
        char* curJSAttr = nextElement(&iterJSAttrs);
        while(curJSAttr != NULL) {
            strcat(toReturn, curJSAttr);
            curJSAttr = nextElement(&iterJSAttrs);
        }
        freeList(attrJS);
    }
    return toReturn;
}

void deleteJSAttrList(void* attrStr) {
    free(attrStr);
    return;
}

/** Function to converting a list of Circle structs into a JSON string
*@pre Circle list is not NULL
*@post Circle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* circListToJSON(const List *list) {
    char* toReturn = NULL;
    List* circs = (List*)list;
    if(circs == NULL || getLength(circs) < 1) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "[]");
    }
    else {
        //ex: [{"cx":32,"cy":32,"r":30,"numAttr":1,"units":""},{"cx":32,"cy":32,"r":30,"numAttr":0,"units":"cm"}]
        ListIterator iterCircs = createIterator(circs); 
        Circle* curCirc = nextElement(&iterCircs); //assume it will be attributes? error check?
        char start[3];
        char* temp;
        char* temp2;
        strcpy(start, "[");
        int toMalloc = 2;
        int size = 0;
        List* circJS = initializeList(&circleToString, &deleteJSAttrList, &compareAttributes); //dummies except delete...
        int length = getLength(circs);
        int i = 1;
        while(curCirc != NULL) {
            temp = circleToJSON(curCirc);
            temp2 = (char*)malloc(sizeof(char)*(strlen(temp)+8));
            if(i == length) { //last element, leave out comma
                size = snprintf(temp2, 0, "%s]", temp);
                snprintf(temp2, size+1, "%s]", temp);
            } 
            else {
                size = snprintf(temp2, 0, "%s,", temp);
                snprintf(temp2, size+1, "%s,", temp);
            }
            free(temp);
            toMalloc += size+1;
            insertBack(circJS, temp2);
            i++;
            curCirc = nextElement(&iterCircs);
        }
        //freeList(attrs);
        toReturn = (char*)malloc(sizeof(char)*(toMalloc+30));
        snprintf(toReturn, 3, start);
        ListIterator iterJSCircs = createIterator(circJS);
        char* curJSCirc = nextElement(&iterJSCircs);
        while(curJSCirc != NULL) {
            strcat(toReturn, curJSCirc);
            curJSCirc = nextElement(&iterJSCircs);
        }
        freeList(circJS);
    }
    return toReturn;
}

/** Function to converting a list of Rectangle structs into a JSON string
*@pre Rectangle list is not NULL
*@post Rectangle list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* rectListToJSON(const List *list) {
    char* toReturn = NULL;
    List* rects = (List*)list;
    if(rects == NULL || getLength(rects) < 1) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "[]");
    }
    else {
        //ex: [{"x":1,"y":2,"w":19,"h":15,"numAttr":3,"units":"cm"},{"x":0,"y":0,"w":2,"h":2,"numAttr":0,"units":""}]
        ListIterator iterRects = createIterator(rects); 
        Rectangle* curRect = nextElement(&iterRects); //assume it will be attributes? error check?
        char start[3];
        char* temp;
        char* temp2;
        strcpy(start, "[");
        int toMalloc = 2;
        int size = 0;
        List* rectJS = initializeList(&rectangleToString, &deleteJSAttrList, &compareAttributes); //dummies except delete...
        int length = getLength(rects);
        int i = 1;
        while(curRect != NULL) {
            temp = rectToJSON(curRect);
            temp2 = (char*)malloc(sizeof(char)*(strlen(temp)+8));
            if(i == length) { //last element, leave out comma
                size = snprintf(temp2, 0, "%s]", temp);
                snprintf(temp2, size+1, "%s]", temp);
            } 
            else {
                size = snprintf(temp2, 0, "%s,", temp);
                snprintf(temp2, size+1, "%s,", temp);
            }
            free(temp);
            toMalloc += size+1;
            insertBack(rectJS, temp2);
            i++;
            curRect = nextElement(&iterRects);
        }
        toReturn = (char*)malloc(sizeof(char)*(toMalloc+30));
        snprintf(toReturn, 3, start);
        ListIterator iterJSRects = createIterator(rectJS);
        char* curJSRect = nextElement(&iterJSRects);
        while(curJSRect != NULL) {
            strcat(toReturn, curJSRect);
            curJSRect = nextElement(&iterJSRects);
        }
        freeList(rectJS);
    }
    return toReturn;
}

/** Function to converting a list of Path structs into a JSON string
*@pre Path list is not NULL
*@post Path list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* pathListToJSON(const List *list) {
    char* toReturn = NULL;
    List* paths = (List*)list;
    if(paths == NULL || getLength(paths) < 1) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "[]");
    }
    else {
        /*ex: 
        [{"d":"m47 36c-15 0-15 0-29.9 0-2.1 0-2.1 4-.1 4","numAttr":0},{"d":"m47 36c-15 0-15 0-29.9 0-2.1 0-2.1 4-.1 4 10.4 0 19.6 0 30 0 2 0","numAttr":1}]
        */
        ListIterator iterPaths = createIterator(paths); 
        Path* curPath = nextElement(&iterPaths); //assume it will be attributes? error check?
        char start[3];
        char* temp;
        char* temp2;
        strcpy(start, "[");
        int toMalloc = 2;
        int size = 0;
        List* pathJS = initializeList(&pathToString, &deleteJSAttrList, &compareAttributes); //dummies except delete...
        int length = getLength(paths);
        int i = 1;
        while(curPath != NULL) {
            temp = pathToJSON(curPath);
            temp2 = (char*)malloc(sizeof(char)*(strlen(temp)+8));
            if(i == length) { //last element, leave out comma
                size = snprintf(temp2, 0, "%s]", temp);
                snprintf(temp2, size+1, "%s]", temp);
            } 
            else {
                size = snprintf(temp2, 0, "%s,", temp);
                snprintf(temp2, size+1, "%s,", temp);
            }
            free(temp);
            toMalloc += size+1;
            insertBack(pathJS, temp2);
            i++;
            curPath = nextElement(&iterPaths);
        }
        toReturn = (char*)malloc(sizeof(char)*(toMalloc+30));
        snprintf(toReturn, 3, start);
        ListIterator iterJSPaths = createIterator(pathJS);
        char* curJSPath = nextElement(&iterJSPaths);
        while(curJSPath != NULL) {
            strcat(toReturn, curJSPath);
            curJSPath = nextElement(&iterJSPaths);
        }
        freeList(pathJS);
    }
    return toReturn;
}

/** Function to converting a list of Group structs into a JSON string
*@pre Group list is not NULL
*@post Group list has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to a List struct
**/
char* groupListToJSON(const List *list) {
    char* toReturn = NULL;
    List* groups = (List*)list;
    if(groups == NULL || getLength(groups) < 1) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "[]");
    }
    else {
        /*ex: 
        [{"children":2,"numAttr":1},{"children":3,"numAttr":1},{"children":2,"numAttr":1}]
        */
        ListIterator iterGroups = createIterator(groups); 
        Group* curGroup = nextElement(&iterGroups); //assume it will be groups? error check?
        char start[3];
        char* temp;
        char* temp2;
        strcpy(start, "[");
        int toMalloc = 2;
        int size = 0;
        List* groupJS = initializeList(&groupToString, &deleteJSAttrList, &compareAttributes); //dummies except delete...
        int length = getLength(groups);
        int i = 1;
        while(curGroup != NULL) {
            temp = groupToJSON(curGroup);
            temp2 = (char*)malloc(sizeof(char)*(strlen(temp)+8));
            if(i == length) { //last element, leave out comma
                size = snprintf(temp2, 0, "%s]", temp);
                snprintf(temp2, size+1, "%s]", temp);
            } 
            else {
                size = snprintf(temp2, 0, "%s,", temp);
                snprintf(temp2, size+1, "%s,", temp);
            }
            free(temp);
            toMalloc += size+1;
            insertBack(groupJS, temp2);
            i++;
            curGroup = nextElement(&iterGroups);
        }
        toReturn = (char*)malloc(sizeof(char)*(toMalloc+30));
        snprintf(toReturn, 3, start);
        ListIterator iterJSGroups = createIterator(groupJS);
        char* curJSGroup = nextElement(&iterJSGroups);
        while(curJSGroup != NULL) {
            strcat(toReturn, curJSGroup);
            curJSGroup = nextElement(&iterJSGroups);
        }
        freeList(groupJS);
    }
    return toReturn;
}

/** Function to converting an SVGimage into a JSON string
*@pre SVGimage is not NULL
*@post SVGimage has not been modified in any way
*@return A string in JSON format
*@param event - a pointer to an SVGimage struct
**/
char* SVGtoJSON(const SVGimage* imge) {
    char* toReturn = NULL;
    SVGimage* image = (SVGimage*)imge;
    //or just validate the imge using the function?
    if(image == NULL || image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        toReturn = (char*)malloc(sizeof(char)*75);
        List* rects = getRects(image); //error checking?
        int numRects = getLength(rects);
        freeList(rects);
        List* circles = getCircles(image);
        int numCircles = getLength(circles);
        freeList(circles);
        List* paths = getPaths(image);
        int numPaths = getLength(paths);
        freeList(paths);
        List* groups = getGroups(image);
        int numGroups = getLength(groups);
        freeList(groups);

        //{"numRect":1,"numCirc":5,"numPaths":2,"numGroups":3}
        int size = 0;
        size = snprintf(toReturn, 0, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", numRects, numCircles, numPaths, numGroups);
        snprintf(toReturn, size+2, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", numRects, numCircles, numPaths, numGroups);

    }
    return toReturn;
}

/* ******************************* Bonus A2 functions - optional for A2 *************************** */

/** Function to converting a JSON string into an SVGimage struct
*@pre JSON string is not NULL
*@post String has not been modified in any way
*@return A newly allocated and initialized SVGimage struct
*@param str - a pointer to a string
**/
SVGimage* JSONtoSVG(const char* svgString) {
    return NULL;
}

/** Function to converting a JSON string into a Rectangle struct
*@pre JSON string is not NULL
*@post Rectangle has not been modified in any way
*@return A newly allocated and initialized Rectangle struct
*@param str - a pointer to a string
**/
Rectangle* JSONtoRect(const char* svgString) {
    return NULL;
}

/** Function to converting a JSON string into a Circle struct
*@pre JSON string is not NULL
*@post Circle has not been modified in any way
*@return A newly allocated and initialized Circle struct
*@param str - a pointer to a string
**/
Circle* JSONtoCircle(const char* svgString) {
    return NULL;
}

/**
A3 STUFF I ADDED
**/
//returns JSON representing valid or invalid svg file
char* jsonOfCreateValidSVGimage(char* fileName) {
    char* converted;
    bool valid = false;
    SVGimage* toConvert = createValidSVGimage(fileName, "svg.xsd");
    valid = validateSVGimage(toConvert, "svg.xsd");
    if(valid) {
        converted = SVGtoJSON(toConvert);
    } else {
        converted = (char*)malloc(sizeof(char)*6); //invalid
        sprintf(converted, "{}");
    }
    deleteSVGimage(toConvert);
    return converted; //where should this be freed?
}

void createSVGFileFromJSON(char* svgFile, char* fileName) {
    /*assuming svgFile is a JSON representing an SVG struct... which will be turned into
    an svg struct using JSONtoSVG
    */
    /*given a JSON string encoding a basic SVG file, and a string with a file name, will:
    - create a SVGimage struct
    - fill out its fields
    - validate it with validateSVG - save it to a file using writeSVG.
    */
    printf("createSVGFile function connected thru server\n");
    printf("Arguments: %s and %s\n", svgFile, fileName);
    //first try to establish dummy connection with this function...
    //print arguments... + pass dummy data to server, look at FFI and other examples
}

char* getTitle(char* fileName) {
    char* toReturn = NULL;
    SVGimage* image = createValidSVGimage(fileName, "svg.xsd");
    //or just validate the imge using the function?
    if(image == NULL || image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        int size = 0;
        size = snprintf(toReturn, 0, "%s", image->title);
        toReturn = (char*)malloc(sizeof(char)*(size+10));
        snprintf(toReturn, size+2, "%s", image->title);
    }
    deleteSVGimage(image);
    return toReturn;
}

char* getDescription(char* fileName) {
    char* toReturn = NULL;
    SVGimage* image = createValidSVGimage(fileName, "svg.xsd");
    //or just validate the imge using the function?
    if(image == NULL || image->rectangles == NULL || image->circles == NULL || image->paths == NULL || image->groups == NULL || image->otherAttributes == NULL) {
        toReturn = (char*)malloc(sizeof(char)*5);
        snprintf(toReturn, 4, "{}");
    }
    else {
        int size = 0;
        size = snprintf(toReturn, 0, "%s", image->description);
        toReturn = (char*)malloc(sizeof(char)*(size+10));
        snprintf(toReturn, size+2, "%s", image->description);
    }
    return toReturn;
}

char* JSONfirstLayerRects(char* fileName) {
    //return JSON array of first layer rectangles!
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    List* firstRectangles = temp->rectangles;
    JSONarray = rectListToJSON(firstRectangles);
    deleteSVGimage(temp);
    return JSONarray;
}

char* JSONfirstLayerCircs(char* fileName) {
    //return JSON array of first layer rectangles!
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    List* firstCircles = temp->circles;
    JSONarray = circListToJSON(firstCircles);
    deleteSVGimage(temp);
    return JSONarray;
}

char* JSONfirstLayerPaths(char* fileName) {
    //return JSON array of first layer rectangles!
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    List* firstPaths = temp->paths;
    JSONarray = pathListToJSON(firstPaths);
    deleteSVGimage(temp);
    return JSONarray;
}

char* JSONfirstLayerGroups(char* fileName) {
    //return JSON array of first layer rectangles!
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    List* firstGroups = temp->groups;
    JSONarray = groupListToJSON(firstGroups);
    deleteSVGimage(temp);
    return JSONarray;
}


char* JSONattrsListOfCurrentRect(char* fileName, int index) {
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    ListIterator iterRects = createIterator(temp->rectangles);

    Rectangle* curRect = nextElement(&iterRects);
    int i = 0;
    while(i < index) {
        curRect = nextElement(&iterRects);
        i++;
    }
    List* attrs = curRect->otherAttributes;
    JSONarray = attrListToJSON(attrs);
    deleteSVGimage(temp);
    return JSONarray;
}

//returns image->(component)[index] list of otherAttributes in JSON
char* JSONattrsListOfCurrentThing(char* fileName, int index, int scenario) {
    char* JSONarray = NULL;
    SVGimage* temp = createValidSVGimage(fileName, "svg.xsd");
    ListIterator iter;
    List* attrs = NULL;
    if(scenario == 1) {
        iter = createIterator(temp->rectangles);
        Rectangle* curRect = nextElement(&iter);
        int i = 0;
        while(i < index) {
            curRect = nextElement(&iter);
            i++;
        }
        attrs = curRect->otherAttributes;
    }
    else if(scenario == 2) {
        iter = createIterator(temp->circles);
        Circle* curCirc = nextElement(&iter);
        int i = 0;
        while(i < index) {
            curCirc = nextElement(&iter);
            i++;
        }
        attrs = curCirc->otherAttributes;
    }
    else if(scenario == 3) {
        iter = createIterator(temp->paths);
        Path* curPath = nextElement(&iter);
        int i = 0;
        while(i < index) {
            curPath = nextElement(&iter);
            i++;
        }
        attrs = curPath->otherAttributes;
    }
    else { //groups
        iter = createIterator(temp->groups);
        Group* curGroup = nextElement(&iter);
        int i = 0;
        while(i < index) {
            curGroup = nextElement(&iter);
            i++;
        }
        attrs = curGroup->otherAttributes;
    }
     
    JSONarray = attrListToJSON(attrs);
    deleteSVGimage(temp);
    return JSONarray;
}

