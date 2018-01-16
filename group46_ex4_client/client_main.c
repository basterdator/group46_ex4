
#include "ClientSocket.h"


int main(int argc, char *argv[])
{	
	char a[] = "1";
	int ia = a[0] - '0';
	printf("%d", ia);
	//char string1[] = "NEW_USER_REQUEST:";
	//char string2[] = "anton\n";
	//char *string3 = NULL;
	//char *MSG_type = NULL;
	//char *param1 = NULL;
	//char *param2 = NULL;
	//char *param3 = NULL;
	//cnctnt(string1, string2, &string3);
	//ParseMessage(string3, &MSG_type, &param1, &param2, &param3);

	//printf("%s\n", MSG_type);
	//printf("%s\n", param1);
	//int res = 0;
	//if (argc == 5)
	//{
	//	// === The next few lines delete the existing data in the log file ===
	//	errno_t retval;
	//	FILE *p_stream;
	//	retval = fopen_s(&p_stream, argv[1], "w");
	//	if (0 != retval)
	//	{
	//		printf("Failed to open file.\n");
	//		return -1;
	//	}
	//	retval = fputs("\0", p_stream);
	//	retval = fclose(p_stream);
	//	if (0 != retval)
	//	{
	//		printf("Failed to close file.\n");
	//		return -1;
	//	}
	//	// ====================================================================

	//	res = MainClient(argv[1], argv[2], argv[3], argv[4]);
	//	printf("Return code %d", res);
	//	return res;


	//}
	//else
	//{
	//	printf("Invalid arguments\n");
	//	return -1;
	//}
	//
}