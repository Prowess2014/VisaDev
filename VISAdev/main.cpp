
#include "stdio.h"
#include "MyVisa.h"

int main()
{	
	CMyVisa ob;
	if(ob.Open("GPIB0::1::INSTR",1000) < VI_SUCCESS)
		printf("failed to open\n");
	char *pbuf = (char*)malloc(550);
	
	int i = 0;
	while(1)
	{
		i++;
		ob.Write(":CURVe?\n");
		
		if(NULL != ob.Read(550))
		{
			//printf(pbuf);
			itoa(i,pbuf,10);
			printf(pbuf);
			printf("\n");
		}
	}
	return 0;
}