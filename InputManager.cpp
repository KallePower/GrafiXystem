#include "InputManager.h"

InputManager* instance;

glm::vec4 Viewport;
Vector3 ViewPortNormalizedMouseCoordinates;

InputManager* InputManager::getInstance() 
{
	if(!instance) 
	{
		instance = new InputManager();
	}

	return instance;
}

InputManager::InputManager(void)
{
	this->keyListener = std::vector<IObserver*>();
	this->specialListener = std::vector<IObserver*>();
	this->mouseMoveListener = std::vector<IObserver*>();
	this->mouseClickListener = std::vector<IObserver*>();
	this->mouseWheelListener = std::vector<IObserver*>();

	Mouse.X=Mouse.Y=0;
	Mouse.Position.x=Mouse.Position.y=0.0f;
	LEFTnewState=MIDDLEnewState=RIGHTnewState=false;
	Mouse.LEFT.CLICK=Mouse.RIGHT.CLICK=Mouse.MIDDLE.CLICK=false;
	Mouse.LEFT.HOLD=Mouse.RIGHT.HOLD=Mouse.MIDDLE.HOLD=false;
	Mouse.Movement = glm::vec2(0,0);

	Viewport=glm::vec4(0,0,1,1);
	ViewPortNormalizedMouseCoordinates=Vector3(0,0,0);
}

InputManager::~InputManager(void)
{

}

void InputManager::attachMouseMove(IObserver* obs) 
{
	if(obs->checkForObservability(OBSERVATE_MOUSE))
		this->mouseMoveListener.push_back(obs);
}

void InputManager::attachKey(IObserver* obs) 
{
	if(obs->checkForObservability(OBSERVATE_KEYBOARD))
		this->keyListener.push_back(obs);
}

void InputManager::attachSpecial(IObserver* obs) 
{
	if(obs->checkForObservability(OBSERVATE_KEYBOARD))
	this->specialListener.push_back(obs);
}

void InputManager::attachMouseClick(IObserver* obs)
{
		if(obs->checkForObservability(OBSERVATE_MOUSE)||obs->checkForObservability(OBSERVATE_CLICKS))
			this->mouseClickListener.push_back(obs);
}

void InputManager::attachMouseWheel(IObserver* obs)
{
	if(obs->checkForObservability(OBSERVATE_WHEELS))
		this->mouseWheelListener.push_back(obs);
}





void
InputManager::notifySpecialKey(int key) 
{
	for(auto it = this->specialListener.begin(); it != this->specialListener.end(); ++it) 
	{
		((IInteractive*)(*it))->specialKeyPressed(key);
	}
}

void
InputManager::notifyKey(char key) 
{
	for(auto it = this->keyListener.begin(); it != this->keyListener.end(); ++it) 
	{
		((IInteractive*)(*it))->keyPress(key);
	}
}

void
InputManager::notifyMouse(int x, int y) 
{
	_setMousePosition(x,y);
	for(auto it = this->mouseMoveListener.begin(); it != this->mouseMoveListener.end(); ++it) 
	{
		((IInteractive*)(*it))->mouseMotion(x,y);
	}
}

void
InputManager::_notifyWheel(int wheel)
{
	for(auto it=mouseWheelListener.begin();it!=mouseWheelListener.end();it++)
	{
		if(!(*it)->checkForObservability(OBSERVATE_MOUSE|OBSERVATE_KEYBOARD))
		{
			if(wheel==0)
				((IWheelee*)(*it))->WheelVRoll(Mouse.WheelV);
			else
				((IWheelee*)(*it))->WheelHRoll(Mouse.WheelH);
		}
		else
			((IInteractive*)(*it))->mouseWheel(wheel,wheel==0?Mouse.WheelV:Mouse.WheelH);
	}
}

void
InputManager::_notifyQlicks(void)
{
	bool click = false;
	if(Mouse.LEFT.CLICK)
	{click=true;
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->LeftClick(Mouse.Position);
			else 
				((IInteractive*)(*it))->mouseQlicks(0,click,Mouse.Position);
		}
	}
	else if(Mouse.LEFT.RELEASE)
	{
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->LeftRelease(Mouse.Position);
			else 
				((IInteractive*)(*it))->mouseQlicks(0,click,Mouse.Position);
		}
	}

	if(click=Mouse.RIGHT.CLICK)
	{click=true;
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->RightClick(Mouse.Position);
			else
				((IInteractive*)(*it))->mouseQlicks(1,click,Mouse.Position);
		}
	}
	else if(Mouse.RIGHT.RELEASE)
	{
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->LeftRelease(Mouse.Position);
			else 
				((IInteractive*)(*it))->mouseQlicks(1,false,Mouse.Position);
		}
	}

	if(click=Mouse.MIDDLE.CLICK)
	{click=true;
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->MiddleClick(Mouse.Position);
			else
				((IInteractive*)(*it))->mouseQlicks(2,click,Mouse.Position);
		}
	}
	else if(Mouse.MIDDLE.RELEASE)
	{
		for(auto it = this->mouseClickListener.begin(); it != this->mouseClickListener.end(); ++it) 
		{
			if((*it)->checkForObservability(OBSERVATE_CLICKS))
				((IQlickable*)(*it))->MiddleRelease(Mouse.Position);
			else 
				((IInteractive*)(*it))->mouseQlicks(2,false,Mouse.Position);
		}
	}
}

void
InputManager::UpdateMouseButtons(int button,int state,int x,int y)
{
		switch (button)
		{
		case GLUT_LEFT_BUTTON:
			LEFTnewState = !state;
			break;
		case GLUT_MIDDLE_BUTTON:
			MIDDLEnewState =  !state;
			break;
		case GLUT_RIGHT_BUTTON:
			RIGHTnewState = !state;
			break;
		}
		_setMousePosition(x,y);

		for(auto it=mouseClickListener.begin();it!=mouseClickListener.end();it++)
		{
			if((*it)->checkForObservability(OBSERVATE_KEYBOARD))
				((IInteractive*)(*it))->mouseQlicks(button,state==GLUT_DOWN,Mouse.Position);
		}
}

void
InputManager::UpdateMouseWheel(int wheel,int state,int x,int y)
{
	Mouse.WheelV = Mouse.WheelH = WHEEL::NONE;

		if(wheel == 0)  // the first mouswheel -> veretical scroll
			Mouse.WheelV = (WHEEL)state;
		else if(wheel == 1)   // the second mousewheel (if your Mouse has one...)
			Mouse.WheelH = (WHEEL)state;

	// calls the invoktionList
	_notifyWheel(wheel);

#ifdef MOUSE_TEST_OUTPUT
	if(instance->Mouse.WheelV == WHEEL::UP)
		std::cout<<"WheelUP\n";
	if(instance->Mouse.WheelV == WHEEL::DOWN)
		std::cout<<"WheelDOWN\n";
#endif
}

glm::vec4 
InputManager::GetViewportRectangle(void)
{
	return Viewport;
}

void 
InputManager::SaveViewportRectangle(int x,int y,int w,int h)
{
	Viewport.x=x;
	Viewport.y=y;
	Viewport.w=w;
	Viewport.z=h;
}

void
InputManager::_setMousePosition(int x,int y)
{
	Mouse.Movement.x = x-Mouse.Position.x;
	Mouse.Movement.y = y-Mouse.Position.y;
	Mouse.Position.x = Mouse.X = x;
	Mouse.Position.y = Mouse.Y = y;

	ViewPortNormalizedMouseCoordinates.x = 2*(Mouse.Position.x/GetViewportRectangle().w)-1;
	ViewPortNormalizedMouseCoordinates.y = 2*(1-Mouse.Position.y/GetViewportRectangle().z)-1;

	glm::vec4 ray_clip(ViewPortNormalizedMouseCoordinates.x,ViewPortNormalizedMouseCoordinates.y,-1,1);

/*
	
	glm::vec4 ray_eye = inverse (projection_matrix) * ray_clip;
	ray_eye = vec4 (ray_eye.xy, -1.0, 0.0);
	vec3 ray_wor = (inverse (view_matrix) * ray_eye).xyz;
// don't forget to normalise the vector at some point
	ray_wor = normalise (ray_wor);
	*/
}

void
InputManager::_setMouseButtons()
{
	Mouse.LEFT.RELEASE = Mouse.LEFT.HOLD? !(Mouse.LEFT.HOLD = LEFTnewState):false;
	Mouse.LEFT.CLICK = Mouse.LEFT.CLICK? !(Mouse.LEFT.HOLD = LEFTnewState): !Mouse.LEFT.HOLD? LEFTnewState:false;

	Mouse.RIGHT.RELEASE = Mouse.RIGHT.HOLD? !(Mouse.RIGHT.HOLD = RIGHTnewState):false;
	Mouse.RIGHT.CLICK = Mouse.RIGHT.CLICK? !(Mouse.RIGHT.HOLD = RIGHTnewState): !Mouse.RIGHT.HOLD? RIGHTnewState:false;

	Mouse.MIDDLE.RELEASE = Mouse.MIDDLE.HOLD? !(Mouse.MIDDLE.HOLD = MIDDLEnewState):false;
	Mouse.MIDDLE.CLICK = Mouse.MIDDLE.CLICK? !(Mouse.MIDDLE.HOLD = MIDDLEnewState): !Mouse.MIDDLE.HOLD? MIDDLEnewState:false;

	Mouse.WheelV = Mouse.WheelH = WHEEL::NONE;

	_notifyQlicks();
}

void
InputManager::PerFrameReset(void)
{
	_setMouseButtons();

#ifdef MOUSE_TEST_OUTPUT
		 if(instance->Mouse.LEFT.CLICK)
		std::cout<<"CLICK\n";
	else if(instance->Mouse.LEFT.RELEASE)
		std::cout<<"\n...RELEASE\n\n";
	else if(instance->Mouse.LEFT.HOLD)
		std::cout<<"HOLD..";
	if(instance->Mouse.RIGHT.CLICK)
		std::cout<<"RIGHT-CLICK\n";
	else if(instance->Mouse.RIGHT.RELEASE)
		std::cout<<"\n...RIGHT-RELEASE\n\n";
	else if(instance->Mouse.RIGHT.HOLD)
		std::cout<<"RIGHT-HOLD..";
	if(instance->Mouse.MIDDLE.CLICK)
		std::cout<<"MIDDLE-CLICK\n";
	else if(instance->Mouse.MIDDLE.RELEASE)
		std::cout<<"\n...MIDDLE-RELEASE\n\n";
	else if(instance->Mouse.MIDDLE.HOLD)
		std::cout<<"MIDDLE-HOLD..";
#endif
}



bool
IObserver::checkForObservability(int flag)
{
	int obst=observedEvents();
	bool mouse = (obst/OBSERVATE_MOUSE) >= 1;
	obst %= OBSERVATE_MOUSE;
	bool keys = (obst/OBSERVATE_KEYBOARD) >=1;
	obst %= OBSERVATE_KEYBOARD;
	bool wheels = (obst/OBSERVATE_WHEELS) >=1;
	obst %= OBSERVATE_WHEELS;
	bool clicks = (obst/OBSERVATE_CLICKS) >=1;

	switch(flag)
	{
	case OBSERVATE_MOUSE:
		return mouse;
	case OBSERVATE_KEYBOARD:
		return keys;
	case OBSERVATE_WHEELS:
		return wheels;
	case OBSERVATE_CLICKS:
		return clicks;
	}

	return false;
}

IInteractive::IInteractive()
{
	
}
int
IInteractive::observedEvents()
{
	return OBSERVATE_MOUSE|OBSERVATE_KEYBOARD;
}

IQlickable::IQlickable()
{

}
int
IQlickable::observedEvents()
{
	return OBSERVATE_CLICKS|OBSERVATE_MOUSE;
}
IWheelee::IWheelee()
{

}
int 
IWheelee::observedEvents()
{
	return OBSERVATE_MOUSE|OBSERVATE_WHEELS;
}