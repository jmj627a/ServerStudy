struct PackHeader
{
	//�� ��ŷ������ �⺻���
	unsigned int code; //������ �ڵ�
	unsigned long FileNum; // ������ ��� ������. �ᱹ ����� size�� ����

};

//�̰� ���� FileNum��ŭ �ݺ��ȴ�.
struct PackFile
{
	char FileName[64];
	unsigned long size;
	unsigned long offset;
};