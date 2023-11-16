#include "Quat.h"
#include <stdio.h>
#include <cmath>


void PrintQuat(const Quat& q)
{
    printf("(%f; %f; %f; %f)", q.x, q.y, q.z, q.w);
}

Quat& Quat::operator=(const Quat& other)
{

    if (this == &other)
        return *this;

    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;

    return *this;

}

Quat AngleAxis(float angle, const vec3& axis)
{
    vec3 unitAxis = Normalized(axis);
    float s = sinf(angle * 0.5f);

    return Quat(
        unitAxis.x * s,
        unitAxis.y * s,
        unitAxis.z * s,
        cosf(angle * 0.5f)
    );
}

Quat FromTo(const vec3& from, const vec3& to)
{
    vec3 unitFrom = Normalized(from);
    vec3 unitTo = Normalized(to);

    if(unitFrom == unitTo)
    {
        return Quat();
    }
    else if(unitFrom == unitTo * - 1.0f)
    {
        vec3 ortho = vec3(1.0f, 0.0f, 0.0f);
        if(fabsf(unitFrom.y) < fabsf(unitFrom.x))
        {
            ortho = vec3(0.0f, 1.0f, 0.0f);
        }
        if(fabsf(unitFrom.z) < fabs(unitFrom.y) && fabs(unitFrom.z) < fabsf(unitFrom.x))
        {
            ortho = vec3(0.0f, 0.0f, 1.0f);
        }

        vec3 axis = Normalized(Cross(unitFrom,ortho));
        return Quat(axis.x, axis.y, axis.z, 0.0f);
    }


    vec3 half = Normalized(unitFrom + unitTo);
    vec3 axis = Cross(unitFrom,half);

    return Quat(
        axis.x,
        axis.y,
        axis.z,
        Dot(unitFrom,half)
    );
}

vec3 GetAxis(const Quat& quat)
{
    return Normalized(vec3(quat.x, quat.y, quat.z));
}

float GetAngle(const Quat& quat)
{
    return 2.0f * acosf(quat.w);
}

Quat operator+(const Quat& a,const Quat& b)
{
    return Quat(
        a.x + b.x,
        a.y + b.y,
        a.z + b.z,
        a.w + b.w
    );
}

Quat operator-(const Quat& a,const Quat& b)
{
    return Quat(
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
        a.w - b.w
    );
}

Quat operator*(const Quat& a, float scalar)
{
    return Quat(
        a.x * scalar,
        a.y * scalar,
        a.z * scalar,
        a.w * scalar
    );
}

Quat operator-(const Quat& quat)
{
    return Quat(
      -quat.x,  
      -quat.y,  
      -quat.z,  
      -quat.w  
    );
}

bool operator==(const Quat& left,const Quat& right)
{
    return (fabsf(left.x - right.x) <= QUATERNION_EPSILON &&
        fabsf(left.y - right.y) <= QUATERNION_EPSILON &&
        fabsf(left.z - right.z) <= QUATERNION_EPSILON &&
        fabsf(left.w - right.w) <= QUATERNION_EPSILON);
}

bool operator!=(const Quat& left,const Quat& right)
{
    return !(left == right);
}

bool SameOrientation(const Quat& left,const Quat& right)
{
    return (fabsf(left.x - right.x) <= QUATERNION_EPSILON && fabsf(left.y - right.y) <= QUATERNION_EPSILON &&
            fabsf(left.z - right.z) <= QUATERNION_EPSILON && fabsf(left.w - right.w) <= QUATERNION_EPSILON)
            ||(fabsf(left.x + right.x) <= QUATERNION_EPSILON && fabsf(left.y + right.y) <= QUATERNION_EPSILON &&
            fabsf(left.z + right.z) <= QUATERNION_EPSILON && fabsf(left.w + right.w) <= QUATERNION_EPSILON) ;
}


float Dot(const Quat& a,const Quat& b)
{
    return {a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w};
}

float LengthSq(const Quat& quat)
{
    return (quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w);
}

float Length(const Quat& quat)
{
    float lengthSq = quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
    if(lengthSq < QUATERNION_EPSILON)
    {
        return 0.0f;
    }

    return sqrtf(lengthSq);
}

void Normalize(Quat& quat)
{
    float lengthSq = quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
    if(lengthSq < QUATERNION_EPSILON)
    {
        return;
    }

    float ivLengthSq = 1.0f / sqrtf(lengthSq);

    quat.x *= ivLengthSq;
    quat.y *= ivLengthSq;
    quat.z *= ivLengthSq;
    quat.w *= ivLengthSq;
}

Quat Normalized(const Quat& quat)
{
    float lengthSq = quat.x * quat.x + quat.y * quat.y + quat.z * quat.z + quat.w * quat.w;
    if(lengthSq < QUATERNION_EPSILON)
    {
        return Quat();
    }

    float ivLengthSq = 1.0f / sqrtf(lengthSq);

    return {
        quat.x * ivLengthSq,
        quat.y * ivLengthSq,
        quat.z * ivLengthSq,
        quat.w * ivLengthSq
    };
}

Quat Conjugate(const Quat& quat)
{
    return {
        -quat.x,
        -quat.y,
        -quat.z,
        quat.w
    };
}

Quat Inverse(const Quat& quat)
{
    float recip = 1.0f;// / lengthSq;

    return {
        quat.x * recip,
        quat.y * recip,
        quat.z * recip,
        -quat.w * recip
    };
}

#if 1
Quat operator*(const Quat& Q1,const Quat& Q2)
{
    return Quat(
        Q2.x * Q1.w + Q2.y * Q1.z - Q2.z * Q1.y + Q2.w * Q1.x,
		-Q2.x * Q1.z + Q2.y * Q1.w + Q2.z * Q1.x + Q2.w * Q1.y,
		Q2.x * Q1.y - Q2.y * Q1.x + Q2.z * Q1.w + Q2.w * Q1.z,
		-Q2.x * Q1.x - Q2.y * Q1.y - Q2.z * Q1.z + Q2.w * Q1.w
    );
}
#else
Quat operator*(const Quat& Q1,const Quat& Q2)
{
	Quat result;
	result.scalar = Q2.scalar * Q1.scalar - Dot(Q2.vector, Q1.vector);
	result.vector = (Q1.vector * Q2.scalar) + (Q2.vector * Q1.scalar) + Cross(Q2.vector, Q1.vector);
	return result;
}
#endif

vec3 operator*(const Quat& q,const vec3& vec)
{
    return q.vector * 2.0f * Dot(q.vector,vec) +
        vec * (q.scalar * q.scalar - Dot(q.vector,q.vector)) +
        Cross(q.vector, vec) * 2.0f * q.scalar;
}

Quat Mix(const Quat& from, const Quat& to, float t)
{
    return from * (1.0f - t) + to * t;
}

Quat NLerp(const Quat& from, const Quat& to, float t)
{
    return Normalized(from + (to - from) * t);
}

Quat operator^(const Quat& q, float f)
{
    float angle = 2.0f * acosf(q.scalar);
    vec3 axis = Normalized(q.vector);

    float halfCos = cosf(f * angle * 0.5f);
    float halfSin = sinf(f * angle * 0.5f);

    return Quat(
        axis.x * halfSin,
        axis.y * halfSin,
        axis.z * halfSin,
        halfCos
    );
}

Quat Slerp(const Quat& start, const Quat& end, float t)
{
    if(fabsf(Dot(start, end)) > 1.0f - QUATERNION_EPSILON)
    {
        return NLerp(start, end, t);
    }

    return Normalized(((Inverse(start) * end) ^ t) * start);
}

Quat LookRotation(const vec3& direction, const vec3& up)
{
    vec3 f = Normalized(direction);
    vec3 u = Normalized(up);
    vec3 r = Cross(f, r);

  
    Quat f2d = FromTo(vec3(0,0, 1), f);

    
    vec3 objectUp = f2d * vec3(0, 1, 0);
    
    Quat u2u = FromTo(objectUp, u);

    
    Quat result = f2d * u2u;

    return Normalized(result);
}

mat4 QuatToMat4(const Quat& q)
{
    vec3 right = q * vec3(1, 0, 0);
    vec3 up = q * vec3(0, 1, 0);
    vec3 forward = q * vec3(0, 0, 1);

    return mat4(right.x, right.y, right.z, 0,
            up.x, up.y, up.z, 0,
            forward.x, forward.y, forward.z, 0,
            0, 0, 0, 1
        );
}

Quat Mat4ToQuat(const mat4& mat)
{
    vec3 up = Normalized(vec3(mat.up.x, mat.up.y, mat.up.z));
    vec3 forward = Normalized(vec3(mat.forward.x, mat.forward.y, mat.forward.z));
    vec3 right = Cross(up, forward);
    up = Cross(forward, right);

    return LookRotation(forward, up);
}
