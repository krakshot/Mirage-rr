#pragma once


namespace Structs::Unity
{
	struct Matrix4x4
	{
		float m[4][4] = { 0 };

		Matrix4x4() {}

		float* operator[](int i) { return m[i]; }
	};
}