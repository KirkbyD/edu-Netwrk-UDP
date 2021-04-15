#include "cShaderManager.hpp"

#include "GLCommon.hpp"	// For all the OpenGL calls, etc.

#include <fstream>
#include <sstream>		// "string stream"
#include <vector>

#include <algorithm>		// for the std::copy
#include <iterator>			// for the back_inserter

bool cShaderManager::useShaderProgram(unsigned int ID) {
	// Use the number directy... 
	// TODO: Might do a lookup to see if we really have that ID...
	glUseProgram(ID);
	return true;
}

bool cShaderManager::useShaderProgram(std::string friendlyName) {
	std::map< std::string /*name*/, unsigned int /*ID*/ >::iterator itShad = this->m_name_to_ID.find(friendlyName);

	if (itShad == this->m_name_to_ID.end())
		return false; // Maybe set glUseProgram(0)....?
	glUseProgram(itShad->second);
	return true;
}

unsigned int cShaderManager::getIDFromFriendlyName(std::string friendlyName) {
	std::map< std::string /*name*/, unsigned int /*ID*/ >::iterator itShad = this->m_name_to_ID.find(friendlyName);
	if (itShad == this->m_name_to_ID.end())
		return 0;
	return itShad->second;
}

cShaderManager::cShaderProgram* cShaderManager::pGetShaderProgramFromFriendlyName(std::string friendlyName) {
	unsigned int shaderID = this->getIDFromFriendlyName(friendlyName);

	// Now get the actual shader
	std::map< unsigned int /*ID*/, cShaderProgram >::iterator itShad = this->m_ID_to_Shader.find(shaderID);

	if (itShad == this->m_ID_to_Shader.end())
		return nullptr;

	cShaderProgram* pShaderIFound = &(itShad->second);
	return pShaderIFound;
}


const unsigned int MAXLINELENGTH = 65536;		// 16x1024

void cShaderManager::setBasePath(std::string basepath) {
	this->m_basepath = basepath;
	return;
}


// Returns bool if didn't load
bool cShaderManager::m_loadSourceFromFile(cShader& shader) {
	std::string fullFileName = this->m_basepath + shader.fileName;

	std::ifstream theFile(fullFileName.c_str());
	if (!theFile.is_open())
		return false;

	shader.vecSource.clear();

	char pLineTemp[MAXLINELENGTH] = { 0 };
	while (theFile.getline(pLineTemp, MAXLINELENGTH)) {
		std::string tempString(pLineTemp);
		shader.vecSource.push_back(tempString);
	}

	theFile.close();
	return true;		// Return the string (from the sstream)
}

// Returns empty string if no error returns false if no error
bool cShaderManager::m_wasThereACompileError(unsigned int shaderID, std::string& errorText) {
	errorText = "";	// No error

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);


		delete[] pLogText;	// Oops

		return true;	// There WAS an error
	}
	return false; // There WASN'T an error
}

bool cShaderManager::m_wasThereALinkError(unsigned int programID, std::string& errorText) {
	errorText = "";	// No error

	GLint wasError = 0;
	glGetProgramiv(programID, GL_LINK_STATUS, &wasError);
	if (wasError == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

		char* pLogText = new char[maxLength];
		// Fill with zeros, maybe...?
		glGetProgramInfoLog(programID, maxLength, &maxLength, pLogText);
		// Copy char array to string
		errorText.append(pLogText);

		// Extra code that Michael forgot wasn't there... 
		this->m_lastError.append("\n");
		this->m_lastError.append(errorText);


		delete[] pLogText;

		// There WAS an error
		return true;
	}

	// There WASN'T an error
	return false;
}


std::string cShaderManager::getLastError(void) {
	std::string lastErrorTemp = this->m_lastError;
	this->m_lastError = "";
	return lastErrorTemp;
}

#include <iostream>
bool cShaderManager::m_compileShaderFromSource(cShaderManager::cShader& shader, std::string& error) {
	error = "";

	const unsigned int MAXLINESIZE = 8 * 1024;	// About 8K PER LINE, which seems excessive

	unsigned int numberOfLines = static_cast<unsigned int>(shader.vecSource.size());

	// This is an array of pointers to strings. aka the lines of source
	char** arraySource = new char* [numberOfLines];
	// Clear array to all zeros (i.e. '\0' or null terminator)
	memset(arraySource, 0, numberOfLines);

	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++) {
		unsigned int numCharacters = (unsigned int)shader.vecSource[indexLine].length();
		// Create an array of chars for each line
		arraySource[indexLine] = new char[numCharacters + 2];		// For the '\n' and '\0' at end
		memset(arraySource[indexLine], 0, numCharacters + 2);

		// Copy line of source into array
		for (unsigned int indexChar = 0; indexChar != shader.vecSource[indexLine].length(); indexChar++) {
			arraySource[indexLine][indexChar] = shader.vecSource[indexLine][indexChar];
		}//for ( unsigned int indexChar = 0...

		// At a '\0' at end (just in case)
		arraySource[indexLine][numCharacters + 0] = '\n';
		arraySource[indexLine][numCharacters + 1] = '\0';
	}//for ( unsigned int indexLine = 0...

	//******************************************************************************************
	//	// Print it out (for debug)
	//	std::cout << "Source (from char** array):" << std::endl;
	//	for ( unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++ )
	//	{
	//		std::cout << "(" << indexLine << "):\t" << arraySource[indexLine] << std::endl;
	//	}
	//	std::cout << "END" << std::endl;
	//******************************************************************************************

	glShaderSource(shader.ID, numberOfLines, arraySource, NULL);
	glCompileShader(shader.ID);

	// Get rid of the temp source "c" style array
	for (unsigned int indexLine = 0; indexLine != numberOfLines; indexLine++)
		delete[] arraySource[indexLine];
	// And delete the original char** array
	delete[] arraySource;

	// Did it work? 
	std::string errorText = "";
	if (this->m_wasThereACompileError(shader.ID, errorText)) {
		std::stringstream ssError;
		ssError << shader.getShaderTypeString();
		ssError << " compile error: ";
		ssError << errorText;
		error = ssError.str();
		return false;
	}
	return true;
}



bool cShaderManager::createProgramFromFile(std::string friendlyName, cShader& vertexShad, cShader& fragShader) {
	std::string errorText = "";

	// Shader loading happening before vertex buffer array
	vertexShad.ID = glCreateShader(GL_VERTEX_SHADER);
	vertexShad.shaderType = cShader::VERTEX_SHADER;
	// Load some text from a file...
	if (!this->m_loadSourceFromFile(vertexShad))
		return false;

	errorText = "";
	if (!this->m_compileShaderFromSource(vertexShad, errorText)) {
		this->m_lastError = errorText;
		return false;
	}//if ( this->m_compileShaderFromSource(...



	fragShader.ID = glCreateShader(GL_FRAGMENT_SHADER);
	fragShader.shaderType = cShader::FRAGMENT_SHADER;
	if (!this->m_loadSourceFromFile(fragShader))
		return false;

	if (!this->m_compileShaderFromSource(fragShader, errorText)) {
		this->m_lastError = errorText;
		return false;
	}//if ( this->m_compileShaderFromSource(...


	cShaderProgram curProgram;
	curProgram.ID = glCreateProgram();

	glAttachShader(curProgram.ID, vertexShad.ID);
	glAttachShader(curProgram.ID, fragShader.ID);
	glLinkProgram(curProgram.ID);

	// Was there a link error? 
	errorText = "";
	if (this->m_wasThereALinkError(curProgram.ID, errorText)) {
		std::stringstream ssError;
		ssError << "Shader program link error: ";
		ssError << errorText;
		this->m_lastError = ssError.str();
		return false;
	}

	// At this point, shaders are compiled and linked into a program
	curProgram.friendlyName = friendlyName;

	// Add the shader to the map
	this->m_ID_to_Shader[curProgram.ID] = curProgram;

	// Save to other map, too
	this->m_name_to_ID[curProgram.friendlyName] = curProgram.ID;

	return true;
}

bool cShaderManager::InitTextShader(unsigned &text_program, unsigned &uniform_tex, unsigned &attribute_coord, unsigned &uniform_color, unsigned &dp_vbo) {
	int params = -1;

	FILE* f;
	fopen_s(&f, "data/shaders/textvertexshader01.glsl", "rb");
	long filesize = ftell(f);
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* simple_text_vert = (char*)malloc(filesize + 1);
	fread(simple_text_vert, filesize, 1, f);
	fclose(f);
	simple_text_vert[filesize] = 0;

	fopen_s(&f, "data/shaders/textfragshader01.glsl", "rb");
	fseek(f, 0, SEEK_END);
	filesize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* simple_text_frag = (char*)malloc(filesize + 1);
	fread(simple_text_frag, filesize, 1, f);
	fclose(f);
	simple_text_frag[filesize] = 0;

	//text vertex shader
	unsigned int simple_text_vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(simple_text_vs, 1, &simple_text_vert, NULL);
	glCompileShader(simple_text_vs);
	glGetShaderiv(simple_text_vs, GL_COMPILE_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to compile simple text vertex shader");
		return 1;
	}
	unsigned int simple_text_fg = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(simple_text_fg, 1, &simple_text_frag, NULL);
	glCompileShader(simple_text_fg);
	glGetShaderiv(simple_text_fg, GL_COMPILE_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to compile simple text fragment shader");
		return 1;
	}

	text_program = glCreateProgram();
	glAttachShader(text_program, simple_text_vs);
	glAttachShader(text_program, simple_text_fg);
	glLinkProgram(text_program);

	glGetProgramiv(text_program, GL_LINK_STATUS, &params);
	if (!params) {
		fprintf(stderr, "Unable to link simple text shader program");
		return 1;
	}

	uniform_tex = glGetUniformLocation(text_program, "tex");
	attribute_coord = glGetAttribLocation(text_program, "coord");
	uniform_color = glGetUniformLocation(text_program, "color");
	glGenBuffers(1, &dp_vbo);

	return true;
}
