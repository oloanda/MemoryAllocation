#pragma once



#include "Matrices.h"

#define QUATERNION_EPSILON      0.000001f


typedef struct Quat
{
    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        struct 
        {
            vec3 vector;
            float scalar;
        };

        float asArray[4];
    };

    inline Quat() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    inline Quat(float inX, float inY, float inZ, float inW) : x(inX), y(inY), z(inZ), w(inW) {}
    
    Quat(const Quat& other) { x = other.x; y = other.y; z = other.z; w = other.w; };
    Quat& operator=(const Quat& other);

} Quat;


void PrintQuat(const Quat& q);

Quat AngleAxis(float angle, const vec3& axis);
Quat FromTo(const vec3& from, const vec3& to);
vec3 GetAxis(const Quat& quat);
float GetAngle(const Quat& quat);

Quat operator+(const Quat& a,const Quat& b);
Quat operator-(const Quat& a,const Quat& b);
Quat operator*(const Quat& a, float scalar);
Quat operator-(const Quat& quat);
bool operator==(const Quat& left,const Quat& right);
bool operator!=(const Quat& left,const Quat& right);

bool SameOrientation(const Quat& left,const Quat& right);

float Dot(const Quat& a,const Quat& b);
float Length(const Quat& quat);
float LengthSq(const Quat& quat);
void Normalize(Quat& quat);
Quat Normalized(const Quat& quat);
Quat Conjugate(const Quat& quat);
Quat Inverse(const Quat& quat);

Quat operator*(const Quat& Q1,const Quat& Q2);
vec3 operator*(const Quat& q,const vec3& vec);

Quat Mix(const Quat& from, const Quat& to, float t);
Quat NLerp(const Quat& from, const Quat& to, float t);

Quat operator^(const Quat& q, float f);
Quat Slerp(const Quat& start, const Quat& end, float t);
Quat LookRotation(const vec3& direction, const vec3& up);
mat4 QuatToMat4(const Quat& q);
Quat Mat4ToQuat(const mat4& mat);

