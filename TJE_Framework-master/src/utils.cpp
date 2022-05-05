#include "utils.h"

#ifdef WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#include "includes.h"

#include "game.h"
#include "camera.h"
#include "shader.h"
#include "mesh.h"

#include "extra/stb_easy_font.h"

#include <sstream>
#include <string>
#include <iostream>

long getTime()
{
	#ifdef WIN32
		return GetTickCount();
	#else
		struct timeval tv;
		gettimeofday(&tv,NULL);
		return (int)(tv.tv_sec*1000 + (tv.tv_usec / 1000));
	#endif
}


//this function is used to access OpenGL Extensions (special features not supported by all cards)
void* getGLProcAddress(const char* name)
{
	return SDL_GL_GetProcAddress(name);
}

//Retrieve the current path of the application
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#ifdef WIN32
	#include <direct.h>
	#define GetCurrentDir _getcwd
#else
	#include <unistd.h>
	#define GetCurrentDir getcwd
#endif

std::string getPath()
{
    std::string fullpath;
    // ----------------------------------------------------------------------------
    // This makes relative paths work in C++ in Xcode by changing directory to the Resources folder inside the .app bundle
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        // error!
    }
    CFRelease(resourcesURL);
    chdir(path);
    fullpath = path;
#else
	 char cCurrentPath[1024];
	 if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		 return "";

	cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
	fullpath = cCurrentPath;

#endif    
    return fullpath;
}

template <typename T>
std::string to_string(T value)
{
  //create an output string stream
  std::ostringstream os ;

  //throw the value into the string stream
  os << value ;

  //convert the string stream into a string and return
  return os.str() ;
}

bool readFile(const std::string& filename, std::string& content)
{
	content.clear();

	long count = 0;

	FILE *fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
	{
		std::cerr << "::readFile: file not found " << filename << std::endl;
		return false;
	}

	fseek(fp, 0, SEEK_END);
	count = ftell(fp);
	rewind(fp);

	content.resize(count);
	if (count > 0)
	{
		count = fread(&content[0], sizeof(char), count, fp);
	}
	fclose(fp);

	return true;
}

bool readFileBin(const std::string& filename, std::vector<unsigned char>& buffer)
{
	buffer.clear();
	FILE* fp = nullptr;
	fp = fopen(filename.c_str(), "rb");
	if (fp == nullptr)
		return false;
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	buffer.resize(size);
	fread(&buffer[0], sizeof(char), buffer.size(), fp);
	fclose(fp);
	return true;
}

void stdlog(std::string str)
{
	std::cout << str << std::endl;
}

bool checkGLErrors()
{
	#ifdef _DEBUG
	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL Error: " << errString << std::endl;
		return false;
	}
	#endif

	return true;
}

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string join(std::vector<std::string>& strings, const char* delim)
{
	std::string str;
	for (int i = 0; i < strings.size(); ++i)
		str += strings[i] + (i < strings.size() - 1 ? std::string(delim) : "");
	return str;
}

Vector2 getDesktopSize( int display_index )
{
  SDL_DisplayMode current;
  // Get current display mode of all displays.
  int should_be_zero = SDL_GetCurrentDisplayMode(display_index, &current);
  return Vector2( (float)current.w, (float)current.h );
}


bool drawText(float x, float y, std::string text, Vector3 c, float scale )
{
	static char buffer[99999]; // ~500 chars
	int num_quads;

	if (scale == 0)
		return true;

	x /= scale;
	y /= scale;

	if (Shader::current)
		Shader::current->disable();

	num_quads = stb_easy_font_print(x, y, (char*)(text.c_str()), NULL, buffer, sizeof(buffer));

	Matrix44 projection_matrix;
	projection_matrix.ortho(0, Game::instance->window_width / scale, Game::instance->window_height / scale, 0, -1, 1);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(Matrix44().m);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(projection_matrix.m);

	glColor3f(c.x, c.y, c.z);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 16, buffer);
	glDrawArrays(GL_QUADS, 0, num_quads * 4);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

std::vector<std::string> tokenize(const std::string& source, const char* delimiters, bool process_strings)
{
	std::vector<std::string> tokens;

	std::string str;
	size_t del_size = strlen(delimiters);
	const char* pos = source.c_str();
	char in_string = 0;
	unsigned int i = 0;
	while (*pos != 0)
	{
		bool split = false;

		if (!process_strings || (process_strings && in_string == 0))
		{
			for (i = 0; i < del_size && *pos != delimiters[i]; i++);
			if (i != del_size) split = true;
		}

		if (process_strings && (*pos == '\"' || *pos == '\''))
		{
			if (!str.empty() && in_string == 0) //some chars remaining
			{
				tokens.push_back(str);
				str.clear();
			}

			in_string = (in_string != 0 ? 0 : *pos);
			if (in_string == 0)
			{
				str += *pos;
				split = true;
			}
		}

		if (split)
		{
			if (!str.empty())
			{
				tokens.push_back(str);
				str.clear();
			}
		}
		else
			str += *pos;
		pos++;
	}
	if (!str.empty())
		tokens.push_back(str);
	return tokens;
}

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

std::string getGPUStats()
{
	GLint nTotalMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX, &nTotalMemoryInKB);
	GLint nCurAvailMemoryInKB = 0;
	glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX, &nCurAvailMemoryInKB);
	if (glGetError() != GL_NO_ERROR) //unsupported feature by driver
	{
		nTotalMemoryInKB = 0;
		nCurAvailMemoryInKB = 0;
	}

	std::string str = "FPS: " + to_string(Game::instance->fps) + " DCS: " + to_string(Mesh::num_meshes_rendered) + " Tris: " + to_string(long(Mesh::num_triangles_rendered * 0.001)) + "Ks  VRAM: " + to_string(int((nTotalMemoryInKB-nCurAvailMemoryInKB) * 0.001)) + "MBs / " + to_string(int(nTotalMemoryInKB * 0.001)) + "MBs";
	Mesh::num_meshes_rendered = 0;
	Mesh::num_triangles_rendered = 0;
	return str;
}

Mesh* grid = NULL;

void drawGrid()
{
	if (!grid)
	{
		grid = new Mesh();
		grid->createGrid(10);
	}

	glLineWidth(1);
	glEnable(GL_BLEND);
	glDepthMask(false);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Shader* grid_shader = Shader::getDefaultShader("grid");
	grid_shader->enable();
	Matrix44 m;
	m.translate(floor(Camera::current->eye.x / 100.0)*100.0f, 0.0f, floor(Camera::current->eye.z / 100.0f)*100.0f);
	grid_shader->setUniform("u_color", Vector4(0.7, 0.7, 0.7, 0.7));
	grid_shader->setUniform("u_model", m);
	grid_shader->setUniform("u_camera_position", Camera::current->eye);
	grid_shader->setUniform("u_viewprojection", Camera::current->viewprojection_matrix);
	grid->render(GL_LINES); //background grid
	glDisable(GL_BLEND);
	glDepthMask(true);
	grid_shader->disable();
}


char* fetchWord(char* data, char* word)
{
	int pos = 0;
	while (*data && *data != ',' && *data != '\n' && pos < 254) { word[pos++] = *data; data++; }
	word[pos] = 0;
	if (pos < 254)
		data++; //skip ',' or '\n'
	return data;
}

char* fetchFloat(char* data, float& v)
{
	char w[255];
	data = fetchWord(data,w);
	v = atof(w);
	return data;
}

char* fetchMatrix44(char* data, Matrix44& m)
{
	char word[255];
	for (int i = 0; i < 16; ++i)
	{
		data = fetchWord(data, word);
		m.m[i] = atof(word);
	}
	return data;
}

char* fetchEndLine(char* data)
{
	while (*data && *data != '\n') { data++; }
	if (*data == '\n')
		data++;
	return data;
}

char* fetchBufferFloat(char* data, std::vector<float>& vector, int num )
{
	int pos = 0;
	char word[255];
	if (num)
		vector.resize(num);
	else //read size with the first number
	{
		data = fetchWord(data, word);
		float v = atof(word);
		assert(v);
		vector.resize(v);
	}

	int index = 0;
	while (*data != 0) {
		if (*data == ',' || *data == '\n')
		{
			if (pos == 0)
			{
				data++;
				continue;
			}
			word[pos] = 0;
			float v = atof(word);
			vector[index++] = v;
			if (*data == '\n' || *data == 0)
			{
				if (*data == '\n')
					data++;
				return data;
			}
			data++;
			if (index >= vector.size())
				return data;
			pos = 0;
		}
		else
		{
			word[pos++] = *data;
			data++;
		}
	}

	return data;
}

char* fetchBufferVec3(char* data, std::vector<Vector3>& vector)
{
	//int pos = 0;
	std::vector<float> floats;
	data = fetchBufferFloat(data, floats);
	vector.resize(floats.size() / 3);
	memcpy(&vector[0], &floats[0], sizeof(float)*floats.size());
	return data;
}

char* fetchBufferVec2(char* data, std::vector<Vector2>& vector)
{
	//int pos = 0;
	std::vector<float> floats;
	data = fetchBufferFloat(data, floats);
	vector.resize(floats.size() / 2);
	memcpy(&vector[0], &floats[0], sizeof(float)*floats.size());
	return data;
}

char* fetchBufferVec3u(char* data, std::vector<Vector3u>& vector)
{
	//int pos = 0;
	std::vector<float> floats;
	data = fetchBufferFloat(data, floats);
	vector.resize(floats.size() / 3);
	for (int i = 0; i < floats.size(); i += 3)
		vector[i / 3].set(floats[i], floats[i + 1], floats[i + 2]);
	return data;
}

char* fetchBufferVec4ub(char* data, std::vector<Vector4ub>& vector)
{
	//int pos = 0;
	std::vector<float> floats;
	data = fetchBufferFloat(data, floats);
	vector.resize(floats.size() / 4);
	for (int i = 0; i < floats.size(); i += 4)
		vector[i / 4].set(floats[i], floats[i + 1], floats[i + 2], floats[i + 3]);
	return data;
}

char* fetchBufferVec4(char* data, std::vector<Vector4>& vector)
{
	//int pos = 0;
	std::vector<float> floats;
	data = fetchBufferFloat(data, floats);
	vector.resize(floats.size() / 4);
	memcpy(&vector[0], &floats[0], sizeof(float)*floats.size());
	return data;
}


