
#include "Boot.h"


void main()
{
	Button_Init();

	if( BOOTLOADER_MODE == IsButtonPressed())
	{
		BootLoaderMode_Handler();
	}
	else
	{
		NomalMode_Handler();
	}

	while (1);
}

