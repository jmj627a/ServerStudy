#include <stdio.h>
#include <string.h>

#define SIZE 30

char dictionary[10][2][30] = {
	{"book", "å"},
	{"i", "��"},
	{"boy", "�ҳ�"},
	{"girl", "�ҳ�"},
	{"love", "���"},
	{"facebook", "���̽���"},
	{"friend", "ģ��"},
	{"we", "�츮"},
	{"a", "�ϳ�"},
	{"you", "��"}
};

void main()
{
	while (true)
	{
		printf("������� �Է��ϼ��� \n");
		char str[SIZE]{ 0, };
		gets_s(str);

		printf("\n");

		char s1[SIZE]{ 0, };
		char s2[SIZE]{ 0, };

		for (int i = 0; i < SIZE; ++i)
		{
			//���⳪ ���� �ƴ϶�� �ڿ� ���̱�
			if (str[i] != ' ' && str[i] != NULL)
			{
				s2[0] = str[i];
				strcat_s(s1, s2);
			}
			//���⳪ ���̸� �ܾ� ã�Ƽ� ���
			else if (str[i] == ' ' || str[i] == NULL)
			{
				bool isThere = false;

				//������ �ִ� �ܾ�� ��
				for (int i = 0; i < 10; ++i)
				{
					if (0 == strcmp(s1, dictionary[i][0]))
					{
						isThere = true;
						printf("%s ", dictionary[i][1]);
						break;
					}
				}

				//�迭�� ������ �״�� ���� ���
				if (!isThere)
					printf("%s ", s1);

				for (int i = 0; i < SIZE; ++i)
				{
					s1[i] = 0;
					s2[i] = 0;
				}
			}
		}
		printf("\n\n");

	}

}