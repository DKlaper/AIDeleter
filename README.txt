# AI Deleter 

## Overview

This FSX add-on deletes 'sleeping' AI aircraft around the user. 
The purpose is to make space for landing aircraft instead of having unrealistically full airports with planes that won't move for another day or so.

So every X (180) seconds it scans the given radius (20000 meters) around the user plane for any AI planes in state sleep and whose planned departure is away more than Y (900000) seconds.
All these aircraft will be deleted, as they would never move unless you let FSX run for more than 25 hours in this spot.

## Requirements / Known Issues

* This add-on works only with Windows Vista and later (Not working on Windows XP!)
* This add-on has only been tested with Windows 10 and FSX not Prepar3d.
* This add-on will break if you'd happen to have a very long username, although I think Windows itself also does not like that.

## Parameters

It has three parameters: 
* The radius of around the user to consider deleting aircraft. By default it is 20'000 meters.
* How often the routine checks for planes to delete. By default every 180 seconds.
* The maximum estimated departure time that's safe from deletion in relation to now. By default it's 90'000 seconds = 25 hours.

## Installation

Place the dll in a convenient spot for you, e.g. in a new subfolder of your FSX installation
and add the following to your dll.xml file in <Home Drive>:\Users\<Username>\AppData\Roaming\Microsoft\FSX folder

<Launch.Addon>
    <Name>AIDEL</Name>
    <Disabled>False</Disabled>
    <Path>Path/to/dll/SimCMain.dll</Path>
</Launch.Addon>

When loading FSX afterwards for the first time you need to allow running this DLL by clicking on RUN.

If you want to build the project you need to have the FSX SDK installed and adapt the paths in the include directories and linker to point to your SimConnect installation.

## Settings and Statistics

When the DLL is loaded correctly you can load a flight.
Then in the top bar menu under Add-Ons you will find AI-Deleter Settings.
Clicking on it will display the settings screen which also shows some statistics in the top bar.
With the buttons you can change the settings, which will be saved immediately for future use.
There are hard coded limits for each parameter.

Note that settings are persisted in your AppData Roaming folder under AIDEL. 
Editing of the file manually is not recommended.

## Uninstallation

Simply Delete the dll, remove the entry from dll.xml and in your AppData Roaming folder delete the AIDEL folder.

## Contact and Support

In case you have questions or need help I can be reached at dekla.dev@outlook.com

Have fun,

David
