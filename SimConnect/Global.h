#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>
#include <ShlObj.h>
#include <fstream>

#include "SimConnect.h"

enum DATADEF {
	AIPARKDATA, AIRSPACEDATA, STARTUP
};

enum REQUESTID {
	R1, R2, R3
};

struct AIData {
	double ETD; // "AI TRAFFIC ETD"
	double tgtDist; // "GPS TARGET DISTANCE"
	double feetAGL; // "PLANE ALT ABOVE GROUND"
	double feetMSL; // "PLANE ALTITUDE"
	int glideSlope1; // "NAV HAS GLIDE SLOPE:1"
	int glideSlope2; // "NAV HAS GLIDE SLOPE:1"
	char state[32]; // "AI TRAFFIC STATE"
};

struct AirspaceData {
	double feetAGL; // "PLANE ALT ABOVE GROUND"
	double feetMSL; // "PLANE ALTITUDE"
	double longitude; // "PLANE LONGITUDE"
	double latitude; // "PLANE LATITUDE"
};

struct ConfigurationData {
	DWORD radius = 20000;
	DWORD maxetd = 90000; 
	DWORD requestEveryXSec = 180;
};

void setupDataDefinition(HANDLE simConnect);
void loadConfig(ConfigurationData* config);
void saveConfig(ConfigurationData configValues);
