#include <stdio.h>
#include <string.h>

#define SIZE 30

char dictionary[10][2][30] = {
	{"book", "책"},
	{"i", "나"},
	{"boy", "소년"},
	{"girl", "소녀"},
	{"love", "사랑"},
	{"facebook", "페이스북"},
	{"friend", "친구"},
	{"we", "우리"},
	{"a", "하나"},
	{"you", "너"}
};

void main()
{
	while (true)
	{
		printf("영어문장을 입력하세요 \n");
		char str[SIZE]{ 0, };
		gets_s(str);

		printf("\n");

		char s1[SIZE]{ 0, };
		char s2[SIZE]{ 0, };

		for (int i = 0; i < SIZE; ++i)
		{
			//띄어쓰기나 끝이 아니라면 뒤에 붙이기
			if (str[i] != ' ' && str[i] != NULL)
			{
				s2[0] = str[i];
				strcat_s(s1, s2);
			}
			//띄어쓰기나 끝이면 단어 찾아서 출력
			else if (str[i] == ' ' || str[i] == NULL)
			{
				bool isThere = false;

				//사전에 있는 단어와 비교
				for (int i = 0; i < 10; ++i)
				{
					if (0 == strcmp(s1, dictionary[i][0]))
					{
						isThere = true;
						printf("%s ", dictionary[i][1]);
						break;
					}
				}

				//배열에 없으면 그대로 영어 출력
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