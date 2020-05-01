#include "ParaSet.h"

void NameShow(Gesture gesture, string HgName, string& gesture_name, string& result_name)
{
	switch (gesture)
	{
	case pause:gesture_name = "pause"; break;
	case on:gesture_name = "on"; break;
	case confirm:gesture_name = "confirm"; break;
	case up:gesture_name = "up"; break;
	case down:gesture_name = "down"; break;
	default:gesture_name = "illegal";
	}

	Result result = (HgName == gesture_name) ? True : False;

	result_name = result ? "True" : "False";
}
