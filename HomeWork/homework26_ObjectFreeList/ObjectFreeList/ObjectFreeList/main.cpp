#include "CObjFreeList.h"
#include <iostream>


using namespace std;

class INTCLASS
{
public:
	INTCLASS();
	virtual ~INTCLASS();

	int num;
};

INTCLASS::INTCLASS()
{
	printf("INTCLASS() 持失切\n");
}

INTCLASS::~INTCLASS()
{
	printf("INTCLASS() 社瑚切\n");
}


void main()
{
	CObjFreeList<INTCLASS> freelist(0, true);

	CObjFreeList<INTCLASS>::st_BLOCK_NODE *ptr = freelist._pFreeNode;
	while (ptr != NULL)
	{
		printf("%x\n", ptr);
		ptr = ptr->stpNextBlock;
	}

	printf("\n");

	INTCLASS *pData1 = freelist.Alloc();
	INTCLASS *pData2 = freelist.Alloc();
	INTCLASS *pData3 = freelist.Alloc();
	INTCLASS *pData4 = freelist.Alloc();
	INTCLASS *pData5 = freelist.Alloc();

	ptr = freelist._pFreeNode;
	while (ptr != NULL)
	{
		printf("%x\n", ptr);
		ptr = ptr->stpNextBlock;
	}

	printf("\n");

	freelist.Free(pData1);
	freelist.Free(pData2);
	freelist.Free(pData3);
	freelist.Free(pData4);
	freelist.Free(pData5);

	ptr = freelist._pFreeNode;
	while (ptr != NULL)
	{
		printf("%x\n", ptr);
		ptr = ptr->stpNextBlock;
	}

	printf("\n");

	pData1 = freelist.Alloc();
	pData1->num = 10;
	pData2 = freelist.Alloc();
	pData3 = freelist.Alloc();
	pData4 = freelist.Alloc();
	pData5 = freelist.Alloc();

	ptr = freelist._pFreeNode;
	while (ptr != NULL)
	{
		printf("%x\n", ptr);
		ptr = ptr->stpNextBlock;
	}

	printf("\n");


	freelist.Free(pData1);
	freelist.Free(pData2);
	freelist.Free(pData3);
	freelist.Free(pData4);
	freelist.Free(pData5);


	ptr = freelist._pFreeNode;
	while (ptr != NULL)
	{
		printf("%x\n", ptr);
		ptr = ptr->stpNextBlock;
	}

}