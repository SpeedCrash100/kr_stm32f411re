#include "maincontoller.h"

int main()
{
	if(!MainContoller_Init())
	{
		while(TRUE)
		{;
		}
	}

	MainContoller_Loop();
}



