//------------------------------------------------------------------------------
//
//  Global Data, like Data Definitions
//
//------------------------------------------------------------------------------

#include "Global.h"

void setupDataDefinition(HANDLE simConnect)
{
	HRESULT hr;
	// unit is seconds even if you put something else there
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "AI TRAFFIC ETD", "seconds");
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "GPS TARGET DISTANCE", "meters");
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "PLANE ALT ABOVE GROUND", "feet");
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "PLANE ALTITUDE", "feet");
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "NAV HAS GLIDE SLOPE:1", "Bool", SIMCONNECT_DATATYPE_INT32);
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "NAV HAS GLIDE SLOPE:2", "Bool", SIMCONNECT_DATATYPE_INT32);
	hr = SimConnect_AddToDataDefinition(simConnect, AIPARKDATA, "AI TRAFFIC STATE", NULL, SIMCONNECT_DATATYPE_STRING32);
}

const static size_t maxsize = 1024;
static char mypath[maxsize];

void loadFilePath()
{
	// just getting the value where to write
	PWSTR path;
	size_t path_length;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);
	wcstombs_s(&path_length, mypath, maxsize, path, maxsize);
	CoTaskMemFree(path);
	
	const char folder[] = "\\AIDEL";
	strcpy_s(mypath + path_length-1, maxsize-path_length, folder);
	// will fail with already exists if already exists
	CreateDirectory(mypath, NULL);
	const char file[] = "\\config.ini";
	strcpy_s(mypath + path_length + strnlen_s(folder, 100) - 1, maxsize - path_length-100, file);
}

bool FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void loadConfig(ConfigurationData* config)
{
	loadFilePath();
	// only load if its existing adn not a directory
	if (FileExists(mypath))
	{
		std::ifstream instream;
		instream.open(mypath);
		instream >> config->maxetd >> config->radius >> config->requestEveryXSec;
		instream.close();
	}
}

void saveConfig(ConfigurationData configValues) {
	// actually writing the file
	std::ofstream outstream;
	outstream.open(mypath);
	outstream << configValues.maxetd << std::endl;
	outstream << configValues.radius << std::endl;
	outstream << configValues.requestEveryXSec << std::endl;

	outstream.close();
}