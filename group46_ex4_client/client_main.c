
#include "ClientSocket.h"
#include "BoardTools.h"


int main(int argc, char *argv[])
{	
	int res = 0;
	if (argc == 5)
	{
		// === The next few lines delete the existing data in the log file ===
		errno_t retval;
		FILE *p_stream;
		retval = fopen_s(&p_stream, argv[1], "w");
		if (0 != retval)
		{
			PrintToLogFile("Custom message: Failed to open the file.\n", argv[1]);
			return -1;
		}
		retval = fputs("\0", p_stream);
		retval = fclose(p_stream);
		if (0 != retval)
		{
			PrintToLogFile("Custom message: Failed to clear the file.\n", argv[1]);
			return -1;
		}
		// =====================================================================

		res = MainClient(argv[1], argv[2], argv[3], argv[4]);
		//printf("Return code %d", res);
		return res;
	}
	else
	{
		printf("Invalid arguments\n");
		return -1;
	}
	
}