// ���
//2 byte - ��Ŷ���� 16
unsigned short	Len;

// ��Ŷ
//4 byte - Start X
//4 byte - Start Y
//4 byte - End X
//4 byte - End Y
//��Ŷ�� �� ������� 18����Ʈ
struct st_DRAW_PACKET
{
	int		iStartX;
	int		iStartY;
	int		iEndX;
	int		iEndY;
};

