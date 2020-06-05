#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

using namespace rapidjson;

StringBuffer StringJSON;
Writer<StringBuffer, UTF16<> >writer(StringJSON);

void main()
{
	writer.StartObject();
	{
		writer.String(L"Account");
		{
			writer.StartArray();
			for (int i = 0; i < 5; ++i)
			{
				writer.StartObject();
				writer.String(L"AccountNO");
				writer.Uint64(1111 + i);
				writer.String(L"Nickname");
				writer.String(L"´Ð³×ÀÓ" + i);
				writer.EndObject();
			}
			writer.EndArray();
		}
	}
	writer.EndObject();

	const char* pJson = StringJSON.GetString();

	FILE* fp;
	int ret = fopen_s(&fp, "text.txt", "wb");

	ret = fwrite(pJson, 1, StringJSON.GetSize(), fp);

	fclose(fp);
}