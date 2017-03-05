// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"
#include <Foundation/Container/ConfigValue.h>

using namespace sb;


TEST_CASE(ConfigValue_Types)
{
	ConfigValue n;
	ASSERT_EXPR(n.IsNull());

	n.SetInt(123);
	ASSERT_EXPR(!n.IsNull());
	ASSERT_EXPR(n.IsInt());
	ASSERT_EXPR(n.IsNumber());
	ASSERT_EQUAL(n.AsInt(), 123);

	n.SetFloat(0.123f);
	ASSERT_EXPR(!n.IsNull());
	ASSERT_EXPR(n.IsFloat());
	ASSERT_EXPR(n.IsNumber());
	ASSERT_EQUAL_F(n.AsFloat(), 0.123f, FLT_EPSILON);

	n.SetString("test");
	ASSERT_EXPR(!n.IsNull());
	ASSERT_EXPR(n.IsString());
	ASSERT_EQUAL_STR(n.AsString(), "test");

}

TEST_CASE(ConfigValue_Array)
{
	ConfigValue n;
	n.SetEmptyArray();

	ASSERT_EXPR(n.IsArray());
	ASSERT_EQUAL(n.Size(), 0);

	for (uint32_t i = 0; i < 1024; ++i)
	{
		n.Append().SetUInt(i);
	}

	ASSERT_EQUAL(n.Size(), 1024);

	for (uint32_t i = 0; i < 1024; ++i)
	{
		ASSERT_EQUAL(n[i].AsUInt(), i);
	}

	n.SetNull();
	ASSERT_EXPR(n.IsNull());
	ASSERT_EQUAL(n.Size(), 0);
}

TEST_CASE(ConfigValue_Object)
{
	ConfigValue n;
	n.SetEmptyObject();

	ASSERT_EXPR(n.IsObject());
	ASSERT_EQUAL(n.Size(), 0);

	n["1"].SetInt(1);
	n["2"].SetInt(2);
	n["3"].SetInt(3);

	ASSERT_EQUAL(n.Size(), 3);

	ASSERT_EQUAL(n["1"].AsInt(), 1);
	ASSERT_EQUAL(n["2"].AsInt(), 2);
	ASSERT_EQUAL(n["3"].AsInt(), 3);

	uint32_t count = 0;
	for (ConfigValue::Iterator it = n.Begin();
		it != n.End();
		++it, ++count)
	{
		ASSERT_EXPR(it->second.IsInt());
		ASSERT_EQUAL(it->second.AsInt(), (count + 1));

		switch (count+1)
		{
		case 1:
			ASSERT_EQUAL_STR(it->first.c_str(), "1");
			break;
		case 2:
			ASSERT_EQUAL_STR(it->first.c_str(), "2");
			break;
		case 3:
			ASSERT_EQUAL_STR(it->first.c_str(), "3");
			break;
		}
	}
	ASSERT_EQUAL(count, 3);

	n.SetNull();
	ASSERT_EXPR(n.IsNull());
	ASSERT_EQUAL(n.Size(), 0);
}

TEST_CASE(ConfigValue_Assign)
{
	ConfigValue n;
	n.SetEmptyObject();

	n["1"].SetInt(1);
	n["2"].SetInt(2);
	n["3"].SetInt(3);

	ConfigValue copy;
	copy = n;
	ASSERT_EQUAL(copy.Size(), 3);
	ASSERT_EXPR(copy["1"].IsInt());
	ASSERT_EXPR(copy["2"].IsInt());
	ASSERT_EXPR(copy["3"].IsInt());

	ASSERT_EQUAL(copy["1"].AsInt(), 1);
	ASSERT_EQUAL(copy["2"].AsInt(), 2);
	ASSERT_EQUAL(copy["3"].AsInt(), 3);

}


TEST_CASE(ConfigValue_Copy)
{
	ConfigValue n;
	n.SetEmptyObject();

	n["1"].SetInt(1);
	n["2"].SetInt(2);
	n["3"].SetInt(3);

	ConfigValue copy(n);
	ASSERT_EQUAL(copy.Size(), 3);
	ASSERT_EXPR(copy["1"].IsInt());
	ASSERT_EXPR(copy["2"].IsInt());
	ASSERT_EXPR(copy["3"].IsInt());

	ASSERT_EQUAL(copy["1"].AsInt(), 1);
	ASSERT_EQUAL(copy["2"].AsInt(), 2);
	ASSERT_EQUAL(copy["3"].AsInt(), 3);
}

