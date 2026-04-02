#include "VectorModel.h"

namespace Muninn::Model
{
	VectorModel VectorModel::operator-(const VectorModel& vector) const
	{
		VectorModel vectorBetweenPoints //vector to enemy (dest(enemy) - source(player))
		{
			x - vector.x, //distance between x postions
			y - vector.y, //distance between y positions
			z - vector.z //distance between z positions
		};

		return vectorBetweenPoints;
	}

	VectorModel VectorModel::ViewToVec() const
	{
		// x = yaw
		// y = pitch

		VectorModel viewVector{};
		if (x >= 0.0f && x < 90.0f) // +x, -y
		{
			viewVector.y = -1.0f;
			viewVector.x = tanf(DegreesToRadians(x));

		}
		else if (x >= 90.0f && x < 180.0f) // +x, +y
		{
			viewVector.x = 1.0f;
			viewVector.y = tanf(DegreesToRadians(x - 90.0f));
		}
		else if (x >= 180.0f && x < 270.0f) // -x, +y
		{
			viewVector.y = 1.0f;
			viewVector.x = -tanf(DegreesToRadians(x - 180.0f));
		}
		else // -x, -y
		{
			viewVector.x = -1.0f;
			viewVector.y = -tanf(DegreesToRadians(x - 270.0f));
		}

		//Pitch angles
		if (y >= 0.0f)
		{
			viewVector.z = tanf(DegreesToRadians(y)) * sqrtf((viewVector.x * viewVector.x) + (viewVector.y * viewVector.y));
		}
		else
		{
			viewVector.z = -tanf(DegreesToRadians(-y)) * sqrtf((viewVector.x * viewVector.x) + (viewVector.y * viewVector.y));
		}
		return viewVector;
	}

	VectorModel VectorModel::VecToView() const
	{
		//Yaw angles
		VectorModel aimAngles{};
		if (x >= 0.0f && y < 0.0f) // +x, -y
		{
			aimAngles.x = RadiansToDegrees(atanf(x / -y));
		}
		else if (x >= 0.0f && y >= 0.0f) // +x, +y
		{
			aimAngles.x = RadiansToDegrees(atanf(y / x)) + 90.0f;
		}
		else if (x < 0.0f && y >= 0.0f) // -x, +y
		{
			aimAngles.x = RadiansToDegrees(atanf(-x / y)) + 180.0f;
		}
		else // -x, -y
		{
			aimAngles.x = RadiansToDegrees(atanf(-y / -x)) + 270.0f;
		}

		//Pitch angles
		if (z >= 0.0f)
		{
			aimAngles.y = RadiansToDegrees(atanf(z / sqrtf(x * x + y * y)));
		}
		else
		{
			aimAngles.y = -RadiansToDegrees(atanf(-z / sqrtf(x * x + y * y)));
		}
		return aimAngles;
	}

	float VectorModel::CalcAngleBetweenVectors(const VectorModel& vector) const
	{
		return RadiansToDegrees(acosf(VectorDotProduct(vector) / (CalcVectorLength() * vector.CalcVectorLength())));
	}

	float VectorModel::VectorDotProduct(const VectorModel& vector) const
	{
		return (x * vector.x) + (y * vector.y) + (z * vector.z);
	}

	float VectorModel::CalcVectorLength() const
	{
		return sqrtf((x * x) + (y * y) + (z * z));
	}

	float VectorModel::RadiansToDegrees(float radians) const
	{
		return ((radians * 180.0f) / 3.14159265359f);
	}

	float VectorModel::DegreesToRadians(float degrees) const
	{
		return ((degrees / 180.0f) * 3.14159265359f);
	}
}