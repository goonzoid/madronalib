
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLDefaultFileLocations.h"

File getDefaultFileLocation(eFileTypes whichFiles)
{
	File result = File::nonexistent;
	String dest;	

	// get start directory for search
	File::SpecialLocationType startDirType;
	switch(whichFiles)
	{
		case kFactoryPresetFiles:
        case kScaleFiles:
			startDirType = File::commonApplicationDataDirectory;
		break;
		case kUserPresetFiles:
		case kSampleFiles:
			startDirType = File::userApplicationDataDirectory;
		break;
		default:
		break;
	}
	File startDir = File::getSpecialLocation (startDirType);	
	

	if (startDir.exists())
	{

#if JUCE_WINDOWS

		// debug() << "Start dir: " << startDir.getFullPathName() << " \n";

		if (whichFiles == kScaleFiles)
		{
			dest = String(MLProjectInfo::makerName) + "/Scales";
		}
		else if (whichFiles == kSampleFiles)
		{
			dest = String(MLProjectInfo::makerName) + "/" + MLProjectInfo::projectName + "/Samples";
		}
		else if (whichFiles == kFactoryPresetFiles)
		{
			dest = String(MLProjectInfo::makerName) + "/" + MLProjectInfo::projectName + "/Presets";
		}
		else if (whichFiles == kUserPresetFiles)
		{
			dest = String(MLProjectInfo::makerName) + "/" + MLProjectInfo::projectName + "/Presets";
		}

#elif JUCE_LINUX


#elif JUCE_IPHONE


#elif JUCE_MAC
		if (whichFiles == kScaleFiles)
		{
			dest = String("Audio/Presets/") + MLProjectInfo::makerName + "/Scales";
		}
		else if (whichFiles == kSampleFiles)
		{
			dest = String("Audio/Presets/") + MLProjectInfo::makerName + "/Samples";
		}
		else // either presets type
		{
			dest = String("Audio/Presets/") + MLProjectInfo::makerName + "/" + MLProjectInfo::projectName;
		}
#endif
		result = startDir.getChildFile(dest);	
	
		if (result.exists())
		{
			debug() << "found path: " << dest << "\n";
			debug() << "full path:" << result.getFullPathName() << "\n";
		}
        else
        {
            result = File::nonexistent;
        }
	}
	return result;
}
