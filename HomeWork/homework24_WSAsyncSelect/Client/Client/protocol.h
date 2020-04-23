// 헤더
//2 byte - 패킷길이 16
unsigned short	Len;

// 패킷
//4 byte - Start X
//4 byte - Start Y
//4 byte - End X
//4 byte - End Y
//패킷의 총 사이즈는 18바이트
struct st_DRAW_PACKET
{
	int		iStartX;
	int		iStartY;
	int		iEndX;
	int		iEndY;
};

