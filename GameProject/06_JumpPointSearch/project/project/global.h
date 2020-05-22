


enum GRID_TYPE
{
	eBLANK,
	eSTART,
	eEND,
	eBLOCKED,
	eOPEN,
	eCLOSE
};

struct TILE
{
	int grid_type;
	char r;
	char g;
	char b;
};


enum DIR_TYPE
{
	eDIR_UL, eDIR_UU, eDIR_UR,
	eDIR_LL,		  eDIR_RR, 
	eDIR_DL, eDIR_DD, eDIR_DR
};


#define WORLD_HEIGHT	30
#define WORLD_WIDTH		50

