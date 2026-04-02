#pragma once
#include <math.h>

namespace Muninn::Model
{
	class VectorModel final
	{
	public:
		float x{};
		float y{};
		float z{};

		VectorModel operator-(const VectorModel& vector) const;

		VectorModel ViewToVec() const;
		VectorModel VecToView() const;
		float CalcAngleBetweenVectors(const VectorModel& vector) const;
		float VectorDotProduct(const VectorModel& vector) const;
		float CalcVectorLength() const;
		float RadiansToDegrees(float radians) const;
		float DegreesToRadians(float degrees) const;
	};
}