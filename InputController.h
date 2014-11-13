#ifndef INPUTCONTROLLER_H_
#define INPUTCONTROLLER_H_

#include <Windows.h>

#define NUMBER_OF_KEYS 256					//The number of entries in the keys arrays

class InputController
{
private:
	HWND m_windowHandle;

	bool m_CurrentKeys[NUMBER_OF_KEYS];		//An array of key values for the current frame
	bool m_PrevKeys[NUMBER_OF_KEYS];		//An array of key values for the previous frame


public:
	InputController(HWND windowHandle);		//Constructor

	void SetKeyDown(UINT keyCode);			//These set methods are used to update the correct values in
	void SetKeyUp(UINT keyCode);			//the current keys/mouse button arrays

	bool GetKeyDown(UINT keyCode);			//Returns true if the selected key is down.
	bool GetKeyUp(UINT keyCode);			//Returns true if the selected key was down last frame and up this frame

	void BeginUpdate();						//Begins updating the input, this reads the current value of the mouse cursor
	void EndUpdate();						//Ends updating the input, this copys the values of current key/mouse into previous keys/mouse
};

#endif