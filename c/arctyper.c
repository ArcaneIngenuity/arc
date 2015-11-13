/// \copyright Copyright 2011-2015 N.P. Wiggill <nick.wiggill@gmail.com>. All rights reserved.
/// 
/// \license This project is released under the <a href="https://opensource.org/licenses/MIT">MIT License</a>.
/// 
/// Permission is hereby granted, free of charge, to any person obtaining
/// a copy of this software and associated documentation files (the
/// "Software"), to deal in the Software without restriction, including
/// without limitation the rights to use, copy, modify, merge, publish,
/// distribute, sublicense, and/or sell copies of the Software, and to
/// permit persons to whom the Software is furnished to do so, subject to
/// the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included
/// in all copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
/// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
/// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
/// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
/// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
/// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// \brief arc - a realtime applications framework for native & web

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "ezxml/ezxml.h"
#include "klib/khash.h"
#include "klib/kvec.h"
// setup khash for key/value types
// shorthand way to get the key from hashtable or defVal if not found
#define kh_get_val(kname, hash, key, defVal) ({k=kh_get(kname, hash, key);(k!=kh_end(hash)?kh_val(hash,k):defVal);})

// shorthand way to set value in hash with single line command.  Returns value
// returns 0=replaced existing item, 1=bucket empty (new key), 2-adding element previously deleted
#define kh_set(kname, hash, key, val) ({int ret; k = kh_put(kname, hash,key,&ret); kh_value(hash,k) = val; ret;})


#define STRLEN_MAX 64

static const int StrPtr = 36;
KHASH_DECLARE(StrPtr, kh_cstr_t, uintptr_t)
KHASH_DEFINE(StrPtr, kh_cstr_t, uintptr_t, kh_str_hash_func, kh_str_hash_equal, 1)

typedef struct ArcFunction
{
	char name[STRLEN_MAX];
} ArcFunction;

struct ArcType;

typedef struct ArcMember
{
	char name[STRLEN_MAX];
	const char * typename; //we store the typename because not every member has a type in the types table, e.g. primitives
	
} ArcMember;

typedef struct ArcType
{
	char name[STRLEN_MAX];
	//const basetypename[STRLEN_MAX];
	kvec_t(const char *) bases;
	char filename[STRLEN_MAX];
	char ** functionsAlphabetical;
	char ** membersAlphabetical;
	khash_t(StrPtr) * functions;
	khash_t(StrPtr) * members;
	bool useStructKeyword;
	bool isPointer;
	
} ArcType;

/*
#ifndef KH_DECL_STR_ARCFUNCTION
#define KH_DECL_STR_ARCFUNCTION
static const int Str_ArcFunction = 41;
KHASH_DECLARE(Str_ArcFunction, kh_cstr_t, ArcFunction)
#endif//KH_DECL_STR_ARCFUNCTION
*/
/*
#ifndef KH_DECL_STR_ARCTYPE
#define KH_DECL_STR_ARCTYPE
static const int Str_ArcType = 42;
KHASH_DECLARE(Str_ArcType, kh_cstr_t, ArcType)
#endif//KH_DECL_STR_ARCTYPE
*/





static char * srcPath;
khash_t(StrPtr) * types;
khash_t(StrPtr) * typesExternal;
char ** typesAlphabetical;
static khiter_t k;
static khiter_t j;
khash_t(StrPtr) * typesToSkip;

char* Text_load(char* filename) //as from orb 10/13/2015
{
	//should be portable - http://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
	//printf("filename???=%s\n", filename);
	char * str = NULL;

	FILE *file = fopen(filename, "rb"); //open for reading
	if (file)
	{
		fseek(file, 0, SEEK_END); //seek to end
		long fileSize = ftell(file); //get current position in stream
		fseek(file, 0, SEEK_SET); //seek to start
		//printf("%s\n", filename);
		
		str = malloc(fileSize + 1); //allocate enough room for file + null terminator (\0)

		if (str != NULL) //if allocation succeeded
		{
			
			//printf(" (fileSize...%ld)\n", fileSize);
			size_t freadResult;
			freadResult = fread(str, 1, fileSize, file); //read elements as one byte each, into string, from file. 
			//printf("freadResult...%d\n", freadResult);
			
			if (freadResult != fileSize)
			{
				fputs ("Reading error", stderr);
				//exit (3);
				
				str = NULL;
				return str;
			}
			
			fclose(file);

			str[fileSize] = 0; //'\0';
		}
	}
	else
		printf("File not found: %s\n", filename);
	
	return str;
}


//allows us to find anonymous structs within anonymous structs and gather all members for a tpo-level type from this tree
void seekBaseMembers(ArcType * basetype, khash_t(StrPtr) * memberNames)
{
	if (basetype)
	{
		for (int h = 0; h < kh_size(basetype->members); ++h)
		{

			
	char * keyMember = basetype->membersAlphabetical[h];
	ArcMember * member = kh_get_val(StrPtr, basetype->members, keyMember, NULL);
	kh_set(StrPtr, memberNames, member->name, "a"); //just use it as a set!

	printf("??? push member name=%s of type=%s\n", member->name, member->typename);
	
			//run through base names for this type, and for each base type, add its member names
			for (int g = 0; g < kv_size(basetype->bases); ++g)
			{
				const char * basetypename = kv_A(basetype->bases, g);
				//ArcMember * member = kh_get_val(StrPtr, basetype->bases, keyMember, NULL);
				ArcType * type = kh_get_val(StrPtr, types, basetypename, NULL);	

					//hack to disallow macros
					//if (strstr(member->typename, "(") == NULL) //no parens = safe
				seekBaseMembers(type, memberNames);
			}
	
		}
	}				

}

void implementTypesAndFunctions(FILE * hFile)
{
	fprintf(hFile, "// code generated by arctyper... //\n");
	fprintf(hFile, "\n");
	fprintf(hFile, "#include <stdio.h>\n");
	fprintf(hFile, "#include <stdlib.h>\n");
	fprintf(hFile, "#include <string.h>\n");
	fprintf(hFile, "#include \"arc.h\"\n");
	fprintf(hFile, "#include \"arctypes.h\"\n");
	fprintf(hFile, "\n");
	
	for (int i = 0; i < kh_size(types); ++i)
	{
		char * key = typesAlphabetical[i];
		//printf("key=%s$\n", key);
		ArcType * type = kh_get_val(StrPtr, types, key, NULL);
		fprintf(hFile, "#include \"%s.h\"\n", type->name);
	}
	fprintf(hFile, "\n");
	
	/*
	fprintf(hFile, "const char * TYPENAME[64];\n");
	fprintf(hFile, "size_t SIZEOF[64];\n");
	fprintf(hFile, "size_t sizeofCount = 0;\n");
	fprintf(hFile, "\n");
	*/
	
	//sizeofDynamic
	fprintf(hFile, "size_t sizeofDynamic(const char * name)\n");
	fprintf(hFile, "{\n");
	fprintf(hFile, "\tif (strlen(name) > 0)\n");
	fprintf(hFile, "\t{\n");
	
	for (int i = 0; i < kh_size(types); ++i)
	{
		char * key = typesAlphabetical[i];
		//printf("key=%s$\n", key);
		ArcType * type = kh_get_val(StrPtr, types, key, NULL);
		fprintf(hFile, "\t\tif (strcmp(name, \"%s\") == 0) return sizeof(%s);\n", type->name, type->name);
	}
	
	fprintf(hFile, "\t\n");
	for (k = kh_begin(typesExternal); k != kh_end(typesExternal); ++k)
	{
		if (kh_exist(typesExternal, k))
		{
			char * keyType = kh_key(typesExternal, k);
			fprintf(hFile, "\t\tif (strcmp(name, \"%s\") == 0) return sizeof(%s);\n", keyType, keyType);
		}
	}
	
	fprintf(hFile, "\t}\n");
	fprintf(hFile, "\tprintf(\"[ARC]    Class not found: %%s.\\n\", name);\n");
	fprintf(hFile, "\treturn NULL;\n");
	fprintf(hFile, "}\n");
	
	//addressofDynamic
	fprintf(hFile, "void * addressofDynamic(const char * name)\n");
	fprintf(hFile, "{\n");
	fprintf(hFile, "\tif (strlen(name) > 0)\n");
	fprintf(hFile, "\t{\n");

	for (int i = 0; i < kh_size(types); ++i)
	{
		char * keyType = typesAlphabetical[i];
		//printf("keyType=%s$\n", keyType);
		ArcType * type = kh_get_val(StrPtr, types, keyType, NULL);
		
		for (int h = 0; h < kh_size(type->functions); ++h)
		{
			char * keyFunction = type->functionsAlphabetical[h];
			//printf("keyFunction=%s$\n", keyFunction);
			ArcFunction * function = kh_get_val(StrPtr, type->functions, keyFunction, NULL);
			
			fprintf(hFile, "\t\tif (strcmp(name, \"%s\") == 0) return &%s;\n", function->name, function->name);
		}
	}

	fprintf(hFile, "\t}\n");
	
	fprintf(hFile, "\tprintf(\"[ARC]    Function not found: %%s.\\n\", name);\n");
	fprintf(hFile, "\treturn NULL;\n");
	
	fprintf(hFile, "}\n");
	fprintf(hFile, "\n");
	
	
	//offsetofDynamic
	fprintf(hFile, "size_t offsetofDynamic(const char * typename, const char * membername)\n");
	fprintf(hFile, "{\n");
	fprintf(hFile, "\tif (strlen(typename) > 0 && strlen(membername) > 0)\n");
	fprintf(hFile, "\t{\n");
	
	for (int i = 0; i < kh_size(types); ++i)
	{
		
		char * keyType = typesAlphabetical[i];
		//printf("keyType=%s$\n", keyType);
		ArcType * type = kh_get_val(StrPtr, types, keyType, NULL);
		
		fprintf(hFile, "\t\tif (strcmp(typename, \"%s\") == 0)\n", type->name);
		fprintf(hFile, "\t\t{\n");
		
		/*
		if (kh_size(type->members) > 0)
		{
			
			for (int h = 0; h < kh_size(type->members); ++h)
			{
				char * keyMember = type->membersAlphabetical[h];
				//printf("keyMember=%s$\n", keyMember);
				ArcMember * member = kh_get_val(StrPtr, type->members, keyMember, NULL);
				
				if (strstr(member->name, "[") != NULL) //if it's an array, comment it for now to prevent compile problems(?) with offsetof
					fprintf(hFile, "//");
				fprintf(hFile, "\t\t\tif (strcmp(membername, \"%s\") == 0) return offsetof(%s, %s);\n", member->name, type->name, member->name);
			}
			
		}
		*/
		khash_t(StrPtr) * memberNames = kh_init(StrPtr); //a list coontaining all the ty
		
		seekBaseMembers(type, memberNames);
		
		
		for (k = kh_begin(memberNames); k != kh_end(memberNames); ++k)
		{
			if (kh_exist(memberNames, k))
			{
				char * keyMemberName = kh_key(memberNames, k);
				if (strstr(keyMemberName, "[") != NULL) //if it's an array, comment it for now to prevent compile problems(?) with offsetof
					fprintf(hFile, "//");
				fprintf(hFile, "\t\t\tif (strcmp(membername, \"%s\") == 0) return offsetof(%s, %s);\n", keyMemberName, type->name, keyMemberName);	
			}
		}
		
		/*

		for (int j = 0; j < kv_size(type->bases); ++j)
		{
			const char * basetypename = kv_A(type->bases, j);
			
			ArcType * basetype = kh_get_val(StrPtr, types, basetypename, NULL);
			//printf("name=%s basetype=%p\n", basetypename, basetype);
			if (basetype) //sometimes it's not available in the map, e.g. arc types like Ctrl, View
			{
				for (int g = 0; g < kh_size(basetype->members); ++g)
				{
					char * keyMember = basetype->membersAlphabetical[g];
					ArcMember * member = kh_get_val(StrPtr, basetype->members, keyMember, NULL);
					
					fprintf(hFile, "\t\t\tif (strcmp(membername, \"%s\") == 0) return offsetof(%s, %s);\n", member->name, type->name, member->name);
					
				}
			}
		}
		*/

		fprintf(hFile, "\t\t}\n");
	}
	
	fprintf(hFile, "\t}\n");
	fprintf(hFile, "\n");
	
	fprintf(hFile, "\tprintf(\"[ARC]    Class / member not found: %%s :: %%s.\\n\", typename, membername);\n");
	fprintf(hFile, "\treturn NULL;\n");
	fprintf(hFile, "}\n");
	
	//typeofMember
	fprintf(hFile, "const char * typeofMemberDynamic(const char * typename, const char * membername)\n");
	fprintf(hFile, "{\n");
	fprintf(hFile, "\tif (strlen(typename) > 0 && strlen(membername) > 0)\n");
	fprintf(hFile, "\t{\n");
	
	
	
	for (int i = 0; i < kh_size(types); ++i)
	{
		char * keyType = typesAlphabetical[i];
		//printf("keyType=%s$\n", keyType);
		ArcType * type = kh_get_val(StrPtr, types, keyType, NULL);
		
		if (kh_size(type->members) > 0)
		{
			fprintf(hFile, "\t\tif (strcmp(typename, \"%s\") == 0)\n", type->name);
			fprintf(hFile, "\t\t{\n");
			for (int h = 0; h < kh_size(type->members); ++h)
			{
				char * keyMember = type->membersAlphabetical[h];
				//printf("keyMember=%s$\n", keyMember);
				ArcMember * member = kh_get_val(StrPtr, type->members, keyMember, NULL);
				
				if (strstr(member->name, "[") != NULL) //if it's an array, comment it for now to prevent compile problems(?) with offsetof
					fprintf(hFile, "//");
				fprintf(hFile, "\t\t\tif (strcmp(membername, \"%s\") == 0) return \"%s\";\n", member->name, member->typename);
			}
			
			for (int j = 0; j < kv_size(type->bases); ++j)
			{
				const char * basetypename = kv_A(type->bases, j);
			//if (strlen(type->basetypename) > 0)
				
				if (j == 0)
					fprintf(hFile, "\t\t\tchar * result =\t\t\ttypeofMemberDynamic(\"%s\", membername);\n", basetypename);
				else
					fprintf(hFile, "\t\t\tif (!result) result =\ttypeofMemberDynamic(\"%s\", membername);\n", basetypename);
			}
			if (kv_size(type->bases) > 0)
				fprintf(hFile, "\t\t\treturn result;\n");
			
			fprintf(hFile, "\t\t}\n");
		}
	}
	
	fprintf(hFile, "\t}\n");
	fprintf(hFile, "\n");
	
	fprintf(hFile, "\tprintf(\"[ARC]    Class / member not found: %%s :: %%s.\\n\", typename, membername);\n");
	fprintf(hFile, "\treturn NULL;\n");
	fprintf(hFile, "}\n");
	
	fprintf(hFile, "// ...code generated by arctyper //");
}


void stripBlockComments(char * buffer)
{
	char current;
	char last = 0;
	char * lastPtr = NULL;
	
	int c  = 0;
	
	bool inBlockComment = false;
	while (buffer[0] != '\0')
	{
		current = buffer[0];
		if 		(last == '/' && current == '*')
		{
			inBlockComment = true;
			lastPtr[0] = ' ';
		}
		else if (last == '*' && current == '/')
		{
			inBlockComment = false;
			buffer[0] = ' ';
			lastPtr[0] = ' ';
		}			
		
		if (inBlockComment)
		{
			buffer[0] = ' ';
		}
			
		last = current;
		lastPtr = buffer;
		++buffer;
	}
}

void stripLineComments(char * buffer)
{
	char current;
	char last = 0;
	char * lastPtr = NULL;
	
	int c  = 0;
	
	bool inElement = false;
	while (buffer[0] != '\0')
	{
		current = buffer[0];
		if 	(current == '/')
		{
			if (last == '/')
			{
				inElement = true;
				lastPtr[0] = ' ';
			}
		}
		else if (current == '\n')
		{
			inElement = false;
		}			
		
		if (inElement)
		{
			buffer[0] = ' ';
		}
			
		last = current;
		lastPtr = buffer;
		++buffer;
	}
}

void stripPreprocessorDirectives(char * buffer)
{
	//printf("buffer=%s\n", buffer);
	//char *strchr(const char *string, int c) -- Find first occurrence of character c in string. 
	int c  = 0;
	//char * str;
	
	//str = strchr(fileString, '#');
	
	bool inElement = false;//str[0] == '#';
	bool inMultiline = false;
	while (buffer[0] != '\0')
	{
		//++str;
		
		if (buffer[0] == '#')
		{
			inElement = true;
		}
		
		if (inElement)
		{
			if (buffer[0] == '\\')
				inMultiline = true;
			
			if (buffer[0] == '\n')
			{
				if (inMultiline)
				{
					inMultiline = false; //until such time as it may(!) be proven true again at end of this line
				}
				else //ordinary case - just end the preprocessor directive.
				{
					inElement = false;
					continue; //we don't want the newline included
				}
			}
		}
		
		if (inElement)
			buffer[0] = ' ';
		/*
		{
			buffer[c++] = fileString[0];
		}
		*/
		++buffer;
	}
}


void stripTypedefs(char * buffer)
{
	char current;
	char last = 0;
	
	int c  = 0;
	
	//use of 8 = length of "typedef" + space
	char* bufferLastFew;

	int braceCount = 0;
	
	bool inElement = false;
	while (buffer[0] != '\0')
	{
		current = buffer[0];
		if (c >= 8)
		{
			bufferLastFew = buffer - 8;
			//printf("???%s|", bufferLastFew);
			//if (bufferLastFew[0] == 't') exit(1);
			if (strncmp(bufferLastFew, "typedef ", 8) == 0)
			{
				inElement = true;
				for (int i = 0; i < 8; ++i)
				{
					bufferLastFew[i] = ' ';
				}
			}
			
			if (inElement)
			{
				if (buffer[0] == '{')
					++braceCount;
				if (buffer[0] == '}')
					--braceCount;
				
				if (braceCount == 0 && current == ';') //this isn't necessarily safe, but should work for vast majority of cases
				{
					inElement = false;
				}
				buffer[0] = ' ';
			}
		}
		
		last = current;
		
		++buffer;
		++c;
	}
}
void newlinesAndTabsToSpaces(char * buffer)
{
	while (buffer[0] != '\0')
	{
		if (buffer[0] == '\r' ||
			buffer[0] == '\n' ||
			buffer[0] == '\t')
		{
			buffer[0] = ' ';
		}

		++buffer;
	}	
}

int splitDeclarations(char * buffer, char ** results)
{
	int c = 0;
	int r = 0; //result index
	bool inElement = false;
	bool lastWasSemicolon = false;
	char* bufferLastFew;
	while (buffer[0] != '\0')
	{
		if (inElement)
		{
			if (buffer[0] == ';')
			{
				inElement = false;
				buffer[0] = '\0'; //replace semicolon with null - safer than placing beyond - to terminate each result string.
				/*
				if (--buffer[0] == ';') //remove stray semicolons at end
				{
					buffer[0] = '\0';
					++buffer;
				}
				*/
				++c;
			}
		}
		else //not in element
		{
			if (buffer[0] != ' ')
			{
				inElement = true;
				results[r++] = buffer;
			}
			if (buffer[0] == ';')
				buffer[0] =  ' ';
		}
		lastWasSemicolon = buffer[0] == ';';
		++buffer;
	}
	return c;
}

void getFunctionName(char * buffer, char ** namePtr)
{
	while (buffer[0] != '\0')
	{
		if (buffer[0] == '(') //opening bracket of function declaration
		{
			//seek backwards until space found
			--buffer;
			while (buffer[0] == ' ') //walk any empty space between function name and (
			{
				--buffer;
			}
			buffer[1] = '\0'; //terminate here at word end in last space
			while (buffer[0] != ' ') //walk any empty space between function name and (
			{
				--buffer;
			}
			//set pointer to word start
			*namePtr = buffer + 1;
			return;
		}

		++buffer;
	}	
}

ArcType * createType()
{
	ArcType * type = calloc(1, sizeof(ArcType));
	type->members = kh_init(StrPtr);
	type->functions = kh_init(StrPtr);
	kv_init(type->bases);
	return type;
}

void extractFunctionsFromHeaders(ArcType * type)
{	
	printf("extractFunctionsFromHeaders %s\n", type->filename);
	char filename[256];
	strcpy(filename, srcPath);
	strcat(filename, "/");
	strcat(filename, type->filename); // for the most recently added type
	strcat(filename, ".h");
	//printf("filename=%s\n", filename);
	char * fileString = Text_load(filename); //NB not freed till program ends - thus we keep valid strings till implementTypesAndFunctions()
	char * declarations[256];
	stripPreprocessorDirectives	(fileString);
	stripBlockComments			(fileString);
	stripLineComments			(fileString);
	stripTypedefs				(fileString);
	newlinesAndTabsToSpaces		(fileString);
	int c = splitDeclarations(fileString, declarations);
	char * functionNames[c];
	
	//printf("count of functions = %d", c);
	for (int i = 0; i < c; ++i)
	{
		getFunctionName(declarations[i], &functionNames[i]);
		ArcFunction * function = calloc(1, sizeof(ArcFunction));
		strcpy(function->name, functionNames[i]);
		
		k = kh_get(StrPtr, type->functions, function->name);
		if (k == kh_end(type->functions)) //if function not already present in map
		{
			kh_set(StrPtr, type->functions, function->name, function);
			printf("[ARC] Function %s added.\n", function->name);
		}
		else //if function is already present in map, get it
		{
			printf("[ARC] Function %s already exists, skipping.\n", function->name);
			//function = kh_get_val(StrPtr, types, subtype->name, NULL);
		}
	}
}

void readTypeFromXml(ezxml_t xml, ArcType * type)
{
	strncpy(type->name, ezxml_attr(xml, "class"), STRLEN_MAX);
	strncpy(type->filename,  ezxml_attr(xml, "path") ? ezxml_attr(xml, "path") : type->name, STRLEN_MAX);
	//printf("classname=%s\n", type->name);
	//printf("filename=%s\n",  type->filename);
}
void readMemberAndTypeFromString(char * string, ArcType * type, ArcMember * member)
{
	char * word = strtok(string, " "); //get leftmost token
	//printf("string=%s\n", string);
	if (strcmp(word, "struct") == 0)
	{
		type->useStructKeyword = true;
		word = strtok(NULL, " ");
		strncpy(type->name, word, STRLEN_MAX);
		strncpy(type->filename, word, STRLEN_MAX);
	}
	else
	{
		type->useStructKeyword = false;
		strncpy(type->name, word, STRLEN_MAX);
		strncpy(type->filename, word, STRLEN_MAX);
	}
	
	//check for * AND member name, and store them
	type->isPointer = false;
	word = strtok(NULL, " ");
	if (word != NULL)
	{
		if (strlen(word) > 0)
		{
			if (word[0] == '*')
			{
				type->isPointer = true;
				word = strtok(NULL, " ");
			}
			if (word != NULL)
			{
				char * ptr = strstr(word, "[");
				if (ptr != NULL)
					ptr[0] = '\0';
				strcpy(member->name, word);
				//printf("[0]classname=%s filename=%s useStructKeyword=%d isPointer=%d member->name=%s\n", type->name, type->filename, type->isPointer, type->useStructKeyword, member->name);
			}
			else
			{
				strcpy(member->name, string);
				//printf("[1]classname=%s filename=%s useStructKeyword=%d isPointer=%d member->name=%s\n", type->name, type->filename, type->isPointer, type->useStructKeyword, member->name);
			}
		}
	}
	member->typename = type->name;
}

void extractTypesAndFunctionsFromUpdaterComponentsXML(ezxml_t parentXml)
{
	//printf("extractTypesAndFunctionsFromUpdaterComponentsXML\n");
	for (ezxml_t elementXml = parentXml->child; elementXml; elementXml = elementXml->ordered) //run through distinct child element names
	{
		bool allowCustomElementsAsUpdaterComponents = false; //DEV -get from <hub> as an arg (or pass hub as arg)
		if (allowCustomElementsAsUpdaterComponents)
		{
			//TODO similar to below block, but using a custom element name as component class name
		}
		else //do not allow custom elements - fallback to seeking standard <component> elements
		{
			if (strcmp(ezxml_name(elementXml), "component") == 0) 
			{
				ArcType * subtype = createType();
				readTypeFromXml(elementXml, subtype);
				
				
				k = kh_get(StrPtr, types, subtype->name);
				if (k == kh_end(types)) //if type not already present in map
				{
					kh_set(StrPtr, types, subtype->name, subtype);
					printf("[ARC] Type %s added.\n", subtype->name);
					extractFunctionsFromHeaders(subtype);
				}
				else //if type is already present in map, get it
				{
					printf("[ARC] Type %s already exists, skipping.\n", subtype->name);
					//subtype = kh_get_val(StrPtr, types, subtype->name, NULL);
				}
			}
		}
	}
}

//assumes word "struct" will never be at very beginning of file (consider include guards etc.)
void stripToStructNamed(char ** bufferRef, ArcType * type)
{
	//if not inStruct
	//search till we find keyword "struct ", inStruct = true
	//read/store from there till first "{", ignoring any spaces
	//if struct name == type->name, go to closing brace without blanking
	//if struct name != type->name, keep reading and blanking out till we either find it or reach eof

	char * buffer = *bufferRef;
	
	char* bufferLastFew;
	int offsetToStruct = -1;

	int charCount  = 0;
	int charCountSinceStruct= 0;
	int braceCount = 0;
	char structStr[] = "struct";
	char typedefStr[] = "typedef";
	int structStrLen = strlen(structStr); //-1 for null terminator!
	int typedefStrLen = strlen(typedefStr); //-1 for null terminator!

	bool isSpace = true;
	bool inStruct = false;
	bool inStructNamed = false;

	while (buffer[0] != '\0')
	{
		isSpace = buffer[0] == ' ';

		if (inStruct)
			++charCountSinceStruct;
		
		if (inStructNamed) //braceCount could still be zero, however!
		{
			if (buffer[0] == '{')
			{
				++braceCount;
			}
			if (buffer[0] == '}')
			{
				if (--braceCount == 0) //if we've come back down to zero braces
				{
					inStructNamed = false;
					//printf("closed struct named...\n");
					(++buffer)[0] = '\0';
					(*bufferRef) += charCount - charCountSinceStruct;
					break;
				}
			}
			
		}
		else if (inStruct)
		{
			if (strncmp(buffer, type->name, strlen(type->name)) == 0)
			{
				inStructNamed = true;
				//offsetToStruct = charCount - charCountSinceStruct;
				//printf("found struct named... %d\n", charCount);
				
			}
			else
			{
				if (buffer[0] == '{')
					inStruct = false; //missed out chance, don't bother check for name further.
			}
		}
		else //not in anything yet, either candidate nor named struct
		{
			if (isSpace)
				if (charCount >= structStrLen) //space
				{
					bufferLastFew = buffer - structStrLen;

					//TODO check for space BEFORE struct - if not at start of file
					if (strncmp(bufferLastFew, structStr, structStrLen) == 0)
					{
						--bufferLastFew; //check if space before
						if (bufferLastFew[0] == ' ' || charCount == structStrLen) //only if there was a space before, or struct is at start of file
						{
							inStruct = true;
							charCountSinceStruct = structStrLen;
							//printf("found struct...%d\n", charCount);
							
							//look for typedef - optional - don't erase it if it's found
							int charCountTypedefToStruct = 0;
							
							do
							{
								++charCountTypedefToStruct;
								--bufferLastFew;
							}
							while (bufferLastFew[0] == ' '); //as soon as it becoms a letter, we can check
							
							//now we're on a non-space character, so check for typedef keyword
							bufferLastFew -= typedefStrLen - 1; //-1 because we're already on the last character of the word preceding struct
							if (strncmp(bufferLastFew, typedefStr, typedefStrLen) == 0)
							{
								//printf("found typedef...%d\n", charCountTypedefToStruct);
								charCountSinceStruct += charCountTypedefToStruct + typedefStrLen;
							}
						}
					}
				}
		}
		
		++buffer;
		++charCount;
	}
	
	if (offsetToStruct >= 0) //never found the one we were looking for
	{
		printf("Could not find class %s in associated file %s.h.\n", type->name, type->filename);
		exit(EXIT_FAILURE);
	}
}

void stripStructContainer(char ** bufferRef)
{
	char * buffer = *bufferRef;
	char * bufferLastFew;

	int charCount  = 0;
	int braceCount = 0;
	int charCountToStructOpen = 0;
	
	bool isSpace = true;
	bool inStruct = false;

	while (buffer[0] != '\0')
	{

		
		if (buffer[0] == '{')
		{
			buffer[0] = ' ';
			++braceCount;
		}
		else if (buffer[0] == '}')
		{
			buffer[0] = ' ';
			if ((--braceCount) == 0)
			{
				buffer[0] == '\0';
				(*bufferRef) += charCountToStructOpen;
				break;
			}
		}
		if (braceCount == 0)
		{
			buffer[0] = ' ';
			charCountToStructOpen++;
		}
		++buffer;
	}
}

void stripMultiSpaces(char ** bufferRef)
{
	char * buffer = *bufferRef;

	char bufferTemp[strlen(buffer)];
	
	int t = 0; //index into buffertemp
	int charCount  = 0;
	int spaceCount = 0;
	
	bool lastWasSpace = true;
	bool thisIsSpace = false;

	while (buffer[0] != '\0')
	{
		thisIsSpace = buffer[0] == ' ';
		
		if (!(lastWasSpace && thisIsSpace))
			bufferTemp[t++] = buffer[0];
		
		lastWasSpace = thisIsSpace;
		
		++buffer;
	}
	
	//we're at '\0' for buffer, so remove a final space if there was one.
	if (bufferTemp[t-1] == ' ')
		bufferTemp[t-1] = '\0';
	else
	{
		bufferTemp[t] = '\0';
	}
	//now copy this back into bufferRef's contents
	buffer = *bufferRef;
	strcpy(buffer, bufferTemp);
	
}

void extractMembersFromHeaders(ArcType * type)
{
	//printf("extractMembersFromHeaders header filename=%s.h\n", type->filename);

	char 	filename[256];
	strcpy(	filename, srcPath);
	strcat(	filename, "/");
	strcat(	filename, type->filename); // for the most recently added type
	strcat(	filename, ".h");
	//printf("filename=%s\n", filename);
	//get all data member declarations
	char * fileString = Text_load(filename); //NB not freed till program ends - thus we keep valid strings till implementTypesAndFunctions()
	char * declarations[256];
	stripPreprocessorDirectives	(fileString);
	stripBlockComments			(fileString);
	stripLineComments			(fileString);
	newlinesAndTabsToSpaces		(fileString); //needs to happen first so we can detect " struct "
	stripToStructNamed			(&fileString, type);
	stripStructContainer		(&fileString);
	stripMultiSpaces			(&fileString);
	int declarationCount = splitDeclarations(fileString, declarations); //countValidEntries(fileString);

	//if(fileString)
	//	printf("fileString=\n%s$$", fileString);
	//printf("declarationCount=%d\n", declarationCount);
	
	//for each declaration string, read type and add it only if does not already exist in types
	for (int i = 0; i < declarationCount; ++i)
	{
		//printf("declaration=%s\n", declarations[i]);
		stripMultiSpaces(&declarations[i]);
		
		ArcType 	* subtype = createType();
		ArcMember 	* member  = calloc(1, sizeof(ArcMember));
		
		readMemberAndTypeFromString(declarations[i], subtype, member);
		
		//if this is an inline struct
		if (strlen(member->name) == 0)//(strcmp("", member->name) == 0)
		{
			//strcpy(type->basetypename, member->typename);
			kv_push(const char *, type->bases, member->typename);
		}
		
		char 	subfilename[256];
		strcpy(	subfilename, srcPath);
		strcat(	subfilename, "/");
		strcat(	subfilename, subtype->filename); // for the most recently added type
		strcat(	subfilename, ".h");
					
		//add to the global types hash if not already present, and recurse if file therefor exists
		k = kh_get(StrPtr, types, subtype->name);
		if (k == kh_end(types)) //if type not already present in global map
		{
			char * 	subfileString = Text_load(subfilename);
			if (subfileString) //if type info is accesible in a header
			{
				//we only add types for which a header exists - this prevents primitives polluting the types table
				printf("[ARC] Type %s added.\n", subtype->name);
				kh_set(StrPtr, types, subtype->name, subtype);
			
				extractMembersFromHeaders(subtype);
			}
			else
			{
				//printf("[ARC] Type %s file %s.h could not be found!\n", subtype->name, subtype->filename);
				//exit(EXIT_FAILURE);
				
				kh_set(StrPtr, typesExternal, member->typename, NULL); //use this as a set - keys but no values
			}
		}
		else //if type is already present in map, get it
		{
			printf("[ARC] Type %s already exists, skipping.\n", subtype->name);
			subtype = kh_get_val(StrPtr, types, subtype->name, NULL);
		}
		
		//irrespective of presence of further files...
		//try to add members - doesn't matter if we overwrite
		if (strlen(member->name) > 0) 
			kh_set(StrPtr, type->members, member->name, member);
	}
	//printf("[ARC] Type %s added.\n", type->name);
}

void extractTypesAndFunctionsFromConfigXML(ezxml_t nodesXml)
{
	//printf("extractTypesAndFunctionsFromConfigXML\n");

	ArcType * type;
	for (ezxml_t nodeXml = ezxml_child(nodesXml, "node"); nodeXml; nodeXml = nodeXml->next)
	{	
		//model
		ezxml_t modelXml = ezxml_child(nodeXml, "model");
		if (modelXml)
		{
			type = createType();
			readTypeFromXml(modelXml, type);
			kh_set(StrPtr, types, type->name, type);
			printf("[ARC] Type %s added.\n", type->name);
			extractMembersFromHeaders(type);
		}
		
		//views
		ezxml_t viewXml = ezxml_child(nodeXml, "view");
		if (viewXml)
		{
			type = createType();
			readTypeFromXml(viewXml, type);
			kh_set(StrPtr, types, type->name, type);
			printf("[ARC] Type %s added.\n", type->name);
			extractFunctionsFromHeaders(type);
			extractMembersFromHeaders(type);
			ezxml_t nodeComponentsXml = ezxml_child(viewXml, "components");
			if (nodeComponentsXml)
				extractTypesAndFunctionsFromUpdaterComponentsXML(nodeComponentsXml);
		}
		//ctrls
		ezxml_t ctrlXml = ezxml_child(nodeXml, "ctrl");
		if (ctrlXml)
		{
			type = createType();
			readTypeFromXml(ctrlXml, type);
			kh_set(StrPtr, types, type->name, type);
			printf("[ARC] Type %s added.\n", type->name);
			extractFunctionsFromHeaders(type);
			extractMembersFromHeaders(type);
			ezxml_t nodeComponentsXml = ezxml_child(ctrlXml, "components");
			if (nodeComponentsXml)
				extractTypesAndFunctionsFromUpdaterComponentsXML(nodeComponentsXml);
		}
		
		//recurse
		ezxml_t nodeNodesXml = ezxml_child(nodeXml, "nodes");
		if (nodeNodesXml)
			extractTypesAndFunctionsFromConfigXML(nodeNodesXml);
	}
}

static int cmp(const void *p1, const void *p2){
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

//sorts global types alphabetically, and functions and members alphabetically within their parent type.
void sortAlphabetical()
{
	size_t typeCount = kh_size(types);
	
	typesAlphabetical = calloc(typeCount, sizeof(char *));
	
	int i = 0;
	for (k = kh_begin(types); k != kh_end(types); ++k)
		if (kh_exist(types, k)) 
		{
			ArcType * type = kh_value(types, k);
			typesAlphabetical[i++] = type->name;
			
			int h;
			
			//functions
			size_t functionCount = kh_size(type->functions);
			type->functionsAlphabetical =  calloc(functionCount, sizeof(char *));
			h = 0;
			for (j = kh_begin(type->functions); j != kh_end(type->functions); ++j)
				if (kh_exist(type->functions, j)) 
				{
					ArcFunction * function = kh_value(type->functions, j);
					type->functionsAlphabetical[h++] = function->name;
					
				}
			
			//data members
			size_t memberCount = kh_size(type->members);
			type->membersAlphabetical =  calloc(memberCount, sizeof(char *));
			h = 0;
			for (j = kh_begin(type->members); j != kh_end(type->members); ++j)
				if (kh_exist(type->members, j)) 
				{
					ArcFunction * member = kh_value(type->members, j);
					type->membersAlphabetical[h++] = member->name;
					
				}
		}
	
	//types 
	qsort(typesAlphabetical, typeCount, sizeof(char *), cmp);
}

void main(int argc, char *argv[])
{	
	if (argc < 2)
	{
		printf("Usage: arctyper <path to arc.config> <path to source input folder> <path to created output file>\n");
		printf("Output is <output>.c.\n");
		exit(EXIT_FAILURE);
	}
	
	ezxml_t xml = ezxml_parse_file(argv[1]);
	if (xml == NULL)
	{
		printf("Error opening config file %s for read.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	srcPath = argv[2];
	
	char * name = argv[3];
	char * suffix = ".c";
	char filename[strlen(name)+strlen(suffix)];
	strcpy(filename, name);
	strcat(filename, suffix);
	FILE * cFile = fopen(filename, "w");
	if (cFile == NULL)
	{
		printf("Error opening source file %s for write.\n", filename);
		exit(EXIT_FAILURE);
	}
	
	//we use StrPtr to avoid a lot of hassle no every kh_set(), as keys are contained in values (copied from xml)
	//this way, we just create it beforehand and put into the hash, instead of having to create it with a temp key (char[]) etc.
	types 			= kh_init(StrPtr); //types found in the project (data path resolution needed for these, possibly)
	typesExternal 	= kh_init(StrPtr); //types found externally  (no data path resolution needed for these)
	//ArcType * type = createType();
	extractTypesAndFunctionsFromConfigXML(xml);
	
	//write the file
	sortAlphabetical();
	implementTypesAndFunctions(cFile);
	
	ezxml_free(xml);
	if (cFile) fclose(cFile);
}