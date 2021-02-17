#include "maincontoller.h"

/**
 * @brief main точка входа в приложение
 * @details запускает главный цикл приложения
 */
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



