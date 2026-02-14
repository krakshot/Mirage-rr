#include "Types.hpp"
#include <corecrt_math.h>

Quaternion EulerToQuaternion(float pitch, float yaw, float roll) {
	pitch = pitch * 3.14159265358979323846f / 180.0f;
	yaw = yaw * 3.14159265358979323846f / 180.0f;
	roll = roll * 3.14159265358979323846f / 180.0f;

	float cy = cos(yaw * 0.5f);
	float sy = sin(yaw * 0.5f);
	float cp = cos(pitch * 0.5f);
	float sp = sin(pitch * 0.5f);
	float cr = cos(roll * 0.5f);
	float sr = sin(roll * 0.5f);

	Quaternion q;
	q.w = cr * cp * cy + sr * sp * sy;
	q.x = sr * cp * cy - cr * sp * sy;
	q.y = cr * sp * cy + sr * cp * sy;
	q.z = cr * cp * sy - sr * sp * cy;
	return q;
}