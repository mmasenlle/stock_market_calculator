#include <stdio.h> 
#include "utils.h" 

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		printf("strtot(%s): %06d\n", argv[1], utils::strtot(argv[1]));
	}
}

