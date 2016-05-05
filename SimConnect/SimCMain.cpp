//------------------------------------------------------------------------------
//
//  Main Class to be replaced by DLL Stub
// TODO Exit
//------------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#include "SimConnect.h"
#include "Global.h"

#define FSAPI __stdcall
typedef void  (FSAPI *FS10ACONTAINDELETEAIAIRCRAFT)(unsigned long container_id, int arg2);

FS10ACONTAINDELETEAIAIRCRAFT FS10DeleteAIAircraft = NULL;


struct Statistics {
	unsigned long nrOfRequests = 0;
	unsigned long nrOfDeletions = 0;
	unsigned long long lastDeleted = 0;
};

Statistics stats;
ConfigurationData conf;
HANDLE  hSimConnect = NULL;
int     quit = 0;
DWORD reqCounter = 100;
DWORD fsecCnt = 1; // four seconds counter
boolean deletedSome = false;
const DWORD SELECTION = 7;
const DWORD GROUP = 8;
const DWORD MENU = 9;
char menu[512];
SIMCONNECT_TEXT_RESULT res = SIMCONNECT_TEXT_RESULT_DISPLAYED;
static const char empty[] = "";
void setup()
{
	// Flightsim calling convention

	// Get the handle of the ACONTAIN.DLL module
	HMODULE hAContain = GetModuleHandle("ACONTAIN.DLL");

	// E) DELETE AN AI AIRCRAFT FROM THE FLIGHT SIMULATOR
	// If we got the handle of the ACONTAIN.DLL module then
	if (hAContain != NULL)
	{
		// Get a pointer to the ACONTAIN.DLL exported function
		FS10DeleteAIAircraft = (FS10ACONTAINDELETEAIAIRCRAFT)GetProcAddress(hAContain, (char*)7);
	}

	// load settings
	loadConfig(&conf);

	// Request an event when the simulation starts
	SimConnect_SubscribeToSystemEvent(hSimConnect, STARTUP, "SimStart");
	SimConnect_SubscribeToSystemEvent(hSimConnect, R2, "4sec");
	setupDataDefinition(hSimConnect);

	// setup menu and selection events
	SimConnect_MenuAddItem(hSimConnect, "AI Deleter Settings", MENU, 54321);
	SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP, MENU);
	SimConnect_MapClientEventToSimEvent(hSimConnect, MENU);
	SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP, MENU);
	SimConnect_SetNotificationGroupPriority(hSimConnect, MENU, SIMCONNECT_GROUP_PRIORITY_DEFAULT);
}

void getMenu(char* displayStatsMenu, int size)
{
	if (size < 400)
	{
		return;
	}

	sprintf_s(displayStatsMenu, size, "AC deleted: %6d Requests made: %6d Last deleted: %6lld seconds ago\0", stats.nrOfDeletions, stats.nrOfRequests, stats.lastDeleted == 0 ? 0 : (GetTickCount64() - stats.lastDeleted) / 1000);
	strcpy_s(displayStatsMenu + 74, size-74, "AI Deleter Settings\0");
	sprintf_s(displayStatsMenu + 94, size - 94, "Slow down check rate by 30 seconds (currently every %5d seconds)\0", conf.requestEveryXSec);
	strcpy_s(displayStatsMenu + 161, size - 161, "Speed up check rate by 30 seconds\0");
	sprintf_s(displayStatsMenu + 195, size - 195, "Increase range by 5000m (currently within %6d meters)\0", conf.radius);
	strcpy_s(displayStatsMenu + 252, size - 252, "Reduce range by 5000m\0");
	sprintf_s(displayStatsMenu + 274, size - 274, "Reduce allowed ETD by 1 hour (currently at most %6.2f hours from now)\0", float(conf.maxetd) / 3600.0);
	strcpy_s(displayStatsMenu + 345, size - 345, "Increase allowed ETD by 1 hour\0");
	strcpy_s(displayStatsMenu + 376, size - 376, "Just close window\0");
}

void CALLBACK dispatchEvents(SIMCONNECT_RECV*  pData, DWORD  cbData, void *  pContext)
{
	HRESULT hr;

	switch (pData->dwID)
	{
	case SIMCONNECT_RECV_ID_EVENT:
	{
		SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;

		switch (evt->uEventID)
		{
		case STARTUP:
			stats.lastDeleted = 0;
			stats.nrOfDeletions = 0;
			stats.nrOfRequests = 0;
			// Now the sim is running, request information on the user aircraft
			hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, R1, AIPARKDATA, conf.radius, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
			++stats.nrOfRequests;
			break;


		case R2:
			fsecCnt++;

			if (fsecCnt >= (conf.requestEveryXSec/4))
			{
				fsecCnt = 0;
				if (SUCCEEDED(SimConnect_RequestDataOnSimObjectType(hSimConnect, R1, AIPARKDATA, conf.radius, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT)))
				{
					++stats.nrOfRequests;
				}
			}
			break;

		case MENU:
			getMenu(menu, sizeof(menu));
			SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
			break;

		case SELECTION:
			res = (SIMCONNECT_TEXT_RESULT)(evt->dwData);
			switch (res) {
			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_1:
				if (conf.requestEveryXSec < 3600)
				{
					conf.requestEveryXSec += 30;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;

			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_2:
				if (conf.requestEveryXSec > 45)
				{
					conf.requestEveryXSec -= 30;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;

			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_3:
				if (conf.radius < 190000)
				{
					conf.radius += 5000;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;

			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_4:
				if (conf.radius > 5000)
				{
					conf.radius -= 5000;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;

			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_5:
				if (conf.maxetd > 5400)
				{
					conf.maxetd -= 3600;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;


			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_6:
				if (conf.maxetd < 7*86400) // must be smaller than a week
				{
					conf.maxetd += 3600;
				}
				getMenu(menu, sizeof(menu));
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(menu), (void*)menu);
				break;

			case SIMCONNECT_TEXT_RESULT_MENU_SELECT_8:
				SimConnect_Text(hSimConnect, SIMCONNECT_TEXT_TYPE_MENU, 0, SELECTION, sizeof(empty), (void*)empty);
				break;
			default: //default cannot be close as displayed is also an event
				break;
			}
			saveConfig(conf);
			break;
		default:
			break;
		}
		break;
	}


	case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
	{
		SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

		switch (pObjData->dwRequestID)
		{
		case R1:
		{
			DWORD ObjectID = pObjData->dwObjectID;
			DWORD entry = pObjData->dwentrynumber;
			DWORD outof = pObjData->dwoutof;
			DWORD length = pObjData->dwDefineCount;
			AIData *pS = (AIData*)&pObjData->dwData;
			if (ObjectID != SIMCONNECT_OBJECT_ID_USER)
			{
				if (strcmp(pS->state, "sleep") == 0 && pS->ETD > conf.maxetd)
				{
					if (FS10DeleteAIAircraft != NULL)
					{
						FS10DeleteAIAircraft(ObjectID, 1);
						++stats.nrOfDeletions;
						deletedSome = true;
					}
				}
			}

			if (entry == outof)
			{
				if (deletedSome)
				{
					stats.lastDeleted = GetTickCount64();
				}
				deletedSome = false;
			}


		}

		default:
			break;
		}
		break;
	}

	case SIMCONNECT_RECV_ID_EXCEPTION:
	{
		SIMCONNECT_RECV_EXCEPTION *except = (SIMCONNECT_RECV_EXCEPTION*)pData;
		printf("\n\n***** EXCEPTION=%d  SendID=%d  Index=%d  cbData=%d\n", except->dwException, except->dwSendID, except->dwIndex, cbData);
		break;
	}

	case SIMCONNECT_RECV_ID_QUIT:
	{
		quit = 1;
		break;
	}

	default:
		printf("\nReceived:%d", pData->dwID);
		break;
	}
}


/*
int __cdecl _tmain(int argc, _TCHAR* argv[])
{
	HRESULT hr;

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "DKTest", NULL, 0, 0, 0)))
	{
		printf("\nConnected to Flight Simulator!");

		// setup
		setup();

		// deal
		while (quit == 0)
		{
			SimConnect_CallDispatch(hSimConnect, dispatchEvents, NULL);

		}


		hr = SimConnect_Close(hSimConnect);

		printf("\nDisconnected from Flight Simulator");
	}
	else
		printf("\nFailed to connect to Flight Simulator");

	return 0;
}
*/

int __stdcall  DLLStart(void)
{
	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "SimCMain", NULL, 0, NULL, 0)))
	{
		printf("\nConnected...");
		setup();
		SimConnect_CallDispatch(hSimConnect, dispatchEvents, NULL);
	}
	return 0;
}
void __stdcall  DLLStop(void)
{
	if (hSimConnect != NULL)
	{
		SimConnect_MenuDeleteItem(hSimConnect, MENU);
		SimConnect_Close(hSimConnect);
	}
}




