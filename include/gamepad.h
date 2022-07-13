#include <MPG.h>

class Gamepad : public MPG
{
public:
	Gamepad(int debounceMS = 5)
			: MPG(debounceMS) {}

	void setup();
	void read();

};