#include "Matrix4.h"

namespace TDS
{

    Mat4::Mat4()
    {
        m[0][0] = 1.0f; m[1][0] = 0.0f; m[2][0] = 0.0f; m[3][0] = 0.0f;
        m[0][1] = 0.0f; m[1][1] = 1.0f; m[2][1] = 0.0f; m[3][1] = 0.0f;
        m[0][2] = 0.0f; m[1][2] = 0.0f; m[2][2] = 1.0f; m[3][2] = 0.0f;
        m[0][3] = 0.0f; m[1][3] = 0.0f; m[2][3] = 0.0f; m[3][3] = 1.0f;     
    }

    Mat4::Mat4(const Mat4& var)
    {
        m[0][0] = var.m[0][0]; m[1][0] = var.m[1][0]; m[2][0] = var.m[2][0]; m[3][0] = var.m[3][0];
        m[0][1] = var.m[0][1]; m[1][1] = var.m[1][1]; m[2][1] = var.m[2][1]; m[3][1] = var.m[3][1];
        m[0][2] = var.m[0][2]; m[1][2] = var.m[1][2]; m[2][2] = var.m[2][2]; m[3][2] = var.m[3][2];
        m[0][3] = var.m[0][3]; m[1][3] = var.m[1][3]; m[2][3] = var.m[2][3]; m[3][3] = var.m[3][3];
    }

    Mat4::Mat4(float value)
    {
        m[0][0] = value; m[1][0] = value; m[2][0] = value; m[3][0] = value;
        m[0][1] = value; m[1][1] = value; m[2][1] = value; m[3][1] = value;
        m[0][2] = value; m[1][2] = value; m[2][2] = value; m[3][2] = value;
        m[0][3] = value; m[1][3] = value; m[2][3] = value; m[3][3] = value;
    }

    Mat4::Mat4(float data[])
    {
        m[0][0] = data[0]; m[1][0] = data[4]; m[2][0] = data[8];  m[3][0] = data[12];
        m[0][1] = data[1]; m[1][1] = data[5]; m[2][1] = data[9];  m[3][1] = data[13];
        m[0][2] = data[2]; m[1][2] = data[6]; m[2][2] = data[10]; m[3][2] = data[14];
        m[0][3] = data[3]; m[1][3] = data[7]; m[2][3] = data[11]; m[3][3] = data[15];
    }

    Mat4::Mat4(float m00, float m01, float m02, float m03,
               float m10, float m11, float m12, float m13,
               float m20, float m21, float m22, float m23,
               float m30, float m31, float m32, float m33)
    {
        m[0][0] = m00; m[1][0] = m10; m[2][0] = m20; m[3][0] = m30;
        m[0][1] = m01; m[1][1] = m11; m[2][1] = m21; m[3][1] = m31;
        m[0][2] = m02; m[1][2] = m12; m[2][2] = m22; m[3][2] = m32;
        m[0][3] = m03; m[1][3] = m13; m[2][3] = m23; m[3][3] = m33;
    }

    Mat4::Mat4(Vec4 v1, Vec4 v2, Vec4 v3, Vec4 v4)
    {
        m[0][0] = v1.x; m[1][0] = v2.x; m[2][0] = v3.x; m[3][0] = v4.x;
        m[0][1] = v1.y; m[1][1] = v2.y; m[2][1] = v3.y; m[3][1] = v4.y;
        m[0][2] = v1.z; m[1][2] = v2.z; m[2][2] = v3.z; m[3][2] = v4.z;
        m[0][3] = v1.w; m[1][3] = v2.w; m[2][3] = v3.w; m[3][3] = v4.w;
    }

    Mat4::Mat4(Mat3 var)
    {
        m[0][0] = var.m[0][0]; m[1][0] = var.m[1][0]; m[2][0] = var.m[2][0]; m[3][0] = 0.0f;
        m[0][1] = var.m[0][1]; m[1][1] = var.m[1][1]; m[2][1] = var.m[2][1]; m[3][1] = 0.0f;
        m[0][2] = var.m[0][2]; m[1][2] = var.m[1][2]; m[2][2] = var.m[2][2]; m[3][2] = 0.0f;
        m[0][3] = 0.0f;        m[1][3] = 0.0f;        m[2][3] = 0.0f;        m[3][3] = 1.0f;
    }

    //Mat4::~Mat4() {}

    Mat4 Mat4::identity()
    {
        return Mat4();
    }

    Mat4 Mat4::zero()
    {
        return Mat4(0.0f);
    }
    // not sure if this is correct or not (from Unity)
    FrustumPlanes Mat4::decomposeProjection()
    {
        Vec3 rightVec(m[0][0], m[0][1], m[0][2]);
        Vec3 upVec(m[1][0], m[1][1], m[1][2]);
        Vec3 forwardVec(m[2][0], m[2][1], m[2][2]);
        Vec3 transVec(m[3][0], m[3][1], m[3][2]);

        rightVec = rightVec.normalize();
        upVec = upVec.normalize();
        forwardVec = forwardVec.normalize();

        float near = transVec.z / forwardVec.z;
        float far = transVec.z / (forwardVec.z - 1.0f);

        float rightLength = rightVec.magnitude();
        float left = transVec.x + rightLength;
        float right = transVec.x - rightLength;

        float topLength = upVec.magnitude();
        float bottom = transVec.y + topLength;
        float top = transVec.y - topLength;

        FrustumPlanes planes;
        planes.left = left;
        planes.right = right;
        planes.bottom = bottom;
        planes.top = top;
        planes.zNear = near;
        planes.zFar = far;

        return planes;
    }

    float Mat4::determinant()
    {
        float m00 = m[0][0]; float m10 = m[1][0]; float m20 = m[2][0]; float m30 = m[3][0];
        float m01 = m[0][1]; float m11 = m[1][1]; float m21 = m[2][1]; float m31 = m[3][1];
        float m02 = m[0][2]; float m12 = m[1][2]; float m22 = m[2][2]; float m32 = m[3][2];
        float m03 = m[0][3]; float m13 = m[1][3]; float m23 = m[2][3]; float m33 = m[3][3];
        return m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 -
               m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
               m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 -
               m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
               m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 -
               m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
               m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 -
               m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
               m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 -
               m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
               m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 -
               m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;
    }

    Mat4 Mat4::inverse()
    {
        float m00 = m[0][0]; float m10 = m[1][0]; float m20 = m[2][0]; float m30 = m[3][0];
        float m01 = m[0][1]; float m11 = m[1][1]; float m21 = m[2][1]; float m31 = m[3][1];
        float m02 = m[0][2]; float m12 = m[1][2]; float m22 = m[2][2]; float m32 = m[3][2];
        float m03 = m[0][3]; float m13 = m[1][3]; float m23 = m[2][3]; float m33 = m[3][3];
        float det = determinant();
        if (det < FLT_EPSILON) throw std::invalid_argument("Matrix is not invertible.");
        float invDet = 1.0f / det;
        return Mat4((m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33) * invDet,
                    (m03 * m22 * m31 - m02 * m23 * m31 - m03 * m21 * m32 + m01 * m23 * m32 + m02 * m21 * m33 - m01 * m22 * m33) * invDet,
                    (m02 * m13 * m31 - m03 * m12 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 + m01 * m12 * m33) * invDet,
                    (m03 * m12 * m21 - m02 * m13 * m21 - m03 * m11 * m22 + m01 * m13 * m22 + m02 * m11 * m23 - m01 * m12 * m23) * invDet,
                    (m13 * m22 * m30 - m12 * m23 * m30 - m13 * m20 * m32 + m10 * m23 * m32 + m12 * m20 * m33 - m10 * m22 * m33) * invDet,
                    (m02 * m23 * m30 - m03 * m22 * m30 + m03 * m20 * m32 - m00 * m23 * m32 - m02 * m20 * m33 + m00 * m22 * m33) * invDet,
                    (m03 * m12 * m30 - m02 * m13 * m30 - m03 * m10 * m32 + m00 * m13 * m32 + m02 * m10 * m33 - m00 * m12 * m33) * invDet,
                    (m02 * m13 * m20 - m03 * m12 * m20 + m03 * m10 * m22 - m00 * m13 * m22 - m02 * m10 * m23 + m00 * m12 * m23) * invDet,
                    (m11 * m23 * m30 - m13 * m21 * m30 + m13 * m20 * m31 - m10 * m23 * m31 - m11 * m20 * m33 + m10 * m21 * m33) * invDet,
                    (m03 * m21 * m30 - m01 * m23 * m30 - m03 * m20 * m31 + m00 * m23 * m31 + m01 * m20 * m33 - m00 * m21 * m33) * invDet,
                    (m01 * m13 * m30 - m03 * m11 * m30 + m03 * m10 * m31 - m00 * m13 * m31 - m01 * m10 * m33 + m00 * m11 * m33) * invDet,
                    (m03 * m11 * m20 - m01 * m13 * m20 - m03 * m10 * m21 + m00 * m13 * m21 + m01 * m10 * m23 - m00 * m11 * m23) * invDet,
                    (m12 * m21 * m30 - m11 * m22 * m30 - m12 * m20 * m31 + m10 * m22 * m31 + m11 * m20 * m32 - m10 * m21 * m32) * invDet,
                    (m01 * m22 * m30 - m02 * m21 * m30 + m02 * m20 * m31 - m00 * m22 * m31 - m01 * m20 * m32 + m00 * m21 * m32) * invDet,
                    (m02 * m11 * m30 - m01 * m12 * m30 - m02 * m10 * m31 + m00 * m12 * m31 + m01 * m10 * m32 - m00 * m11 * m32) * invDet,
                    (m01 * m12 * m20 - m02 * m11 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 + m00 * m11 * m22) * invDet);
    }

    Mat4 Mat4::transpose()
    {
        Vec4 v1(m[0], m[4], m[8], m[12]);
        Vec4 v2(m[1], m[5], m[9], m[13]);
        Vec4 v3(m[2], m[6], m[10], m[14]);
        Vec4 v4(m[3], m[7], m[11], m[15]);
        return Mat4(v1, v2, v3, v4);
    }

    Vec4 Mat4::GetColumn(int index)
    {
        switch (index)
        {
        case 0: return Vec4(m[0][0], m[0][1], m[0][2], m[0][3]);
        case 1: return Vec4(m[1][0], m[1][1], m[1][2], m[1][3]);
        case 2: return Vec4(m[2][0], m[2][1], m[2][2], m[2][3]);
        case 3: return Vec4(m[3][0], m[3][1], m[3][2], m[3][3]);
        default: throw std::out_of_range("Mat4 index out of range");
        }
    }

    void Mat4::SetColumn(int index, Vec4 v)
    {
        switch (index)
        {
        case 0: m[0][0] = v.x; m[0][1] = v.y; m[0][2] = v.z; m[0][3] = v.w; break;
        case 1: m[1][0] = v.x; m[1][1] = v.y; m[1][2] = v.z; m[1][3] = v.w; break;
        case 2: m[2][0] = v.x; m[2][1] = v.y; m[2][2] = v.z; m[2][3] = v.w; break;
        case 3: m[3][0] = v.x; m[3][1] = v.y; m[3][2] = v.z; m[3][3] = v.w; break;
        default: throw std::out_of_range("Mat4 index out of range");
        }
    }

    Vec4 Mat4::GetRow(int index)
    {
        switch (index)
        {
        case 0: return Vec4(m[0][0], m[1][0], m[2][0], m[3][0]);
        case 1: return Vec4(m[0][1], m[1][1], m[2][1], m[3][1]);
        case 2: return Vec4(m[0][2], m[1][2], m[2][2], m[3][2]);
        case 3: return Vec4(m[0][3], m[1][3], m[2][3], m[3][3]);
        default: throw std::out_of_range("Mat4 index out of range");
        }
    }

    void Mat4::SetRow(int index, Vec4 v)
    {
        switch (index)
        {
        case 0: m[0][0] = v.x; m[1][0] = v.y; m[2][0] = v.z; m[3][0] = v.w; break;
        case 1: m[0][1] = v.x; m[1][1] = v.y; m[2][1] = v.z; m[3][1] = v.w; break;
        case 2: m[0][2] = v.x; m[1][2] = v.y; m[2][2] = v.z; m[3][2] = v.w; break;
        case 3: m[0][3] = v.x; m[1][3] = v.y; m[2][3] = v.z; m[3][3] = v.w; break;
        default: throw std::out_of_range("Mat4 index out of range");
        }
    }

    Vec3 Mat4::GetPosition()
    {
        return Vec3(m[3][0], m[3][1], m[3][2]);
    }

    Vec3 Mat4::MultiplyPoint(Vec3 pt)
    {
        Vec3 result;
        float w;
        result.x = m[0][0] * pt.x + m[1][0] * pt.y + m[2][0] * pt.z + m[3][0];
        result.y = m[0][1] * pt.x + m[1][1] * pt.y + m[2][1] * pt.z + m[3][1];
        result.z = m[0][2] * pt.x + m[1][2] * pt.y + m[2][2] * pt.z + m[3][2];
        w = m[0][3] * pt.x + m[1][3] * pt.y + m[2][3] * pt.z + m[3][3];

        w = 1.0f / w;
        result.x *= w;
        result.y *= w;
        result.z *= w;
        return result;
    }

    Vec3 Mat4::MultiplyPoint3x4(Vec3 pt)
    {
        Vec3 result;
        result.x = m[0][0] * pt.x + m[1][0] * pt.y + m[2][0] * pt.z + m[3][0];
        result.y = m[0][1] * pt.x + m[1][1] * pt.y + m[2][1] * pt.z + m[3][1];
        result.z = m[0][2] * pt.x + m[1][2] * pt.y + m[2][2] * pt.z + m[3][2];
        return result;
    }

    Vec3 Mat4::MultiplyVector(Vec3 vec)
    {
        Vec3 result;
        result.x = m[0][0] * vec.x + m[1][0] * vec.y + m[2][0] * vec.z;
        result.y = m[0][1] * vec.x + m[1][1] * vec.y + m[2][1] * vec.z;
        result.z = m[0][2] * vec.x + m[1][2] * vec.y + m[2][2] * vec.z;
        return result;
    }

    std::string Mat4::ToString()
    {
        std::stringstream ss;
        ss << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n";
        ss << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n";
        ss << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n";
        ss << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3] << "\n";
        return ss.str();
    }

    Vec3 Mat4::GetRotation() const 
    {
        Vec3 scale = GetScale();

        // Create a 3x3 matrix without scale
        Mat3 rotationMatrix = Mat3(
            m[0][0] / scale.x, m[0][1] / scale.y, m[0][2] / scale.z,
            m[1][0] / scale.x, m[1][1] / scale.y, m[1][2] / scale.z,
            m[2][0] / scale.x, m[2][1] / scale.y, m[2][2] / scale.z
        );

        // Assuming Euler angles in Y-X-Z order
        Vec3 euler;
        euler.y = asin(-std::clamp(rotationMatrix.m[2][0], -1.0f, 1.0f));
        if (abs(rotationMatrix.m[2][0]) < 0.999999) {
            euler.x = atan2(rotationMatrix.m[2][1], rotationMatrix.m[2][2]);
            euler.z = atan2(rotationMatrix.m[1][0], rotationMatrix.m[0][0]);
        }
        else {
            euler.x = atan2(-rotationMatrix.m[0][1], rotationMatrix.m[1][1]);
            euler.z = 0;
        }

        return euler;
    }

    Vec3 Mat4::GetScale() const {
        Vec3 scales;
        scales.x = Vec3(m[0][0], m[1][0], m[2][0]).magnitude();
        scales.y = Vec3(m[0][1], m[1][1], m[2][1]).magnitude();
        scales.z = Vec3(m[0][2], m[1][2], m[2][2]).magnitude();
        return scales;
    }


    Mat4 Mat4::Frustum(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        Mat4 result{};
        result.m[0][0] = 2.0f * zNear / (right - left);
        result.m[1][1] = 2.0f * zNear / (top - bottom);
        result.m[2][0] = (right + left) / (right - left);
        result.m[2][1] = (top + bottom) / (top - bottom);
        result.m[2][2] = -(zFar + zNear) / (zFar - zNear);
        result.m[2][3] = -1.0f;
        result.m[3][2] = -(2.0f * zFar * zNear) / (zFar - zNear);
        return result;
    }

    Mat4 Mat4::Frustum(FrustumPlanes planes)
    {
        return Frustum(planes.left, planes.right, planes.bottom, planes.top, planes.zNear, planes.zFar);
    }

    Mat4 Mat4::LookAt(Vec3 eye, Vec3 center, Vec3 up)
    {
        Vec3 f = (center - eye).normalize();
        Vec3 s = Vec3::Cross(f, up).normalize();
        Vec3 u = Vec3::Cross(s, f);

        Mat4 result{};
        result.m[0][0] = s.x;
        result.m[1][0] = s.y;
        result.m[2][0] = s.z;
        result.m[0][1] = u.x;
        result.m[1][1] = u.y;
        result.m[2][1] = u.z;
        result.m[0][2] = -f.x;
        result.m[1][2] = -f.y;
        result.m[2][2] = -f.z;
        result.m[3][0] = -Vec3::Dot(s, eye);
        result.m[3][1] = -Vec3::Dot(u, eye);
        result.m[3][2] = Vec3::Dot(f, eye);
        return result;
    }

    Mat4 Mat4::Ortho(float left, float right, float bottom, float top, float zNear, float zFar)
    {
        Mat4 result = Mat4::identity();
        result.m[0][0] = 2.0f / (right - left);
        result.m[1][1] = 2.0f / (top - bottom);
        result.m[2][2] = -2.0f / (zFar - zNear);
        result.m[3][0] = -(right + left) / (right - left);
        result.m[3][1] = -(top + bottom) / (top - bottom);
        result.m[3][2] = -(zFar + zNear) / (zFar - zNear);
        return result;
    }

    Mat4 Mat4::Perspective(float fov /*radians*/, float aspect, float zNear, float zFar)
    {
        float top, bottom, left, right;
        top = zNear * Mathf::Tan(fov * 0.5f);
        bottom = -top;
        right = top * aspect;
        left = -right;
        return Frustum(left, right, bottom, top, zNear, zFar);
    }

    Mat4 Mat4::Scale(Vec3 scale)
    {
        Mat4 result(0.f);
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;
        result.m[3][3] = 1.0f;
        return result;
    }

    Mat4 Mat4::Translate(Vec3 vec)
    {
        Vec4 v1(1.f, 0.f, 0.f, 0.f);
        Vec4 v2(0.f, 1.f, 0.f, 0.f);
        Vec4 v3(0.f, 0.f, 1.f, 0.f);
        Vec4 v4(vec.x, vec.y, vec.z, 1.f);
        return Mat4(v1, v2, v3, v4);
    }
    Mat4 Mat4::Rotate(const Vec3& eulerAngles)
    {
        float xRad = Mathf::Deg2Rad * eulerAngles.x;
        float yRad = Mathf::Deg2Rad * eulerAngles.y;
        float zRad = Mathf::Deg2Rad * eulerAngles.z;

        Mat4 rotX = Mat4::Rotate(Vec3(1.0f, 0.0f, 0.0f), xRad);
        Mat4 rotY = Mat4::Rotate(Vec3(0.0f, 1.0f, 0.0f), yRad);
        Mat4 rotZ = Mat4::Rotate(Vec3(0.0f, 0.0f, 1.0f), zRad);

        return rotZ * rotY * rotX;
    }
    Mat4 Mat4::Rotate(const Vec3& axis, float angleDegrees)
    {
        Mat4 result;

        float angleRadians = angleDegrees * 3.14159265359f / 180.0f;
        float c = cos(angleRadians);
        float s = sin(angleRadians);
        float t = 1.0f - c;

        float x = axis.x;
        float y = axis.y;
        float z = axis.z;

        // Normalize the axis
        float length = sqrt(x * x + y * y + z * z);
        if (length != 0.0f)
        {
            x /= length;
            y /= length;
            z /= length;
        }

        // Rotation matrix elements
        result.m[0][0] = t * x * x + c;
        result.m[0][1] = t * x * y + s * z;
        result.m[0][2] = t * x * z - s * y;
        result.m[0][3] = 0.0f;

        result.m[1][0] = t * x * y - s * z;
        result.m[1][1] = t * y * y + c;
        result.m[1][2] = t * y * z + s * x;
        result.m[1][3] = 0.0f;

        result.m[2][0] = t * x * z + s * y;
        result.m[2][1] = t * y * z - s * x;
        result.m[2][2] = t * z * z + c;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }
    float* Mat4::Mat4Value_ptr(const Mat4& mat)
    {
        float* ptr = new float[16];

        for (int i{ 0 }; i < 4; ++i)
        {
            for (int j{ 0 }; j < 4; ++j)
            {
                ptr[i * 4 + j] = mat.m[i][j];
            }
        }



        return  ptr;
    }
#if 0 // unsure if correct implementation
  	Quat Mat4::toQuat(Mat4 const& m4)
	{
		Mat3 m = Mat3(m4.m[0][0], m4.m[0][1], m4.m[0][2],
			m4.m[1][0], m4.m[1][1], m4.m[1][2],
			m4.m[2][0], m4.m[2][1], m4.m[2][2]);

        float fourXSquareMinus1 = m.m[0][0] - m.m[1][1] - m.m[2][2];
		float fourYSquareMinus1 = m.m[1][1] - m.m[0][0] - m.m[2][2];
		float fourZSquareMinus1 = m.m[2][2] - m.m[0][0] - m.m[1][1];
		float fourWSquareMinus1 = m.m[0][0] + m.m[1][1] + m.m[2][2];

		int biggestIndex = 0;
		float fourBiggestSquareMinus1 = fourWSquareMinus1;
		if (fourXSquareMinus1 > fourBiggestSquareMinus1)
		{
			fourBiggestSquareMinus1 = fourXSquareMinus1;
			biggestIndex = 1;
		}
		if (fourYSquareMinus1 > fourBiggestSquareMinus1)
		{
			fourBiggestSquareMinus1 = fourYSquareMinus1;
			biggestIndex = 2;
		}
		if (fourZSquareMinus1 > fourBiggestSquareMinus1)
		{
			fourBiggestSquareMinus1 = fourZSquareMinus1;
			biggestIndex = 3;
		}

		float biggestVal = Mathf::Sqrt(fourBiggestSquareMinus1 + 1.f) * 0.5f;
		float mult = 0.25f / biggestVal;

		switch (biggestIndex)
		{
		case 0:
			return Quat(biggestVal, (m.m[1][2] - m.m[2][1]) * mult, (m.m[2][0] - m.m[0][2]) * mult, (m.m[0][1] - m.m[1][0]) * mult);
		case 1:
			return Quat((m.m[1][2] - m.m[2][1]) * mult, biggestVal, (m.m[0][1] + m.m[1][0]) * mult, (m.m[2][0] + m.m[0][2]) * mult);
		case 2:
			return Quat((m.m[2][0] - m.m[0][2]) * mult, (m.m[0][1] + m.m[1][0]) * mult, biggestVal, (m.m[1][2] + m.m[2][1]) * mult);
		case 3:
			return Quat((m.m[0][1] - m.m[1][0]) * mult, (m.m[2][0] + m.m[0][2]) * mult, (m.m[1][2] + m.m[2][1]) * mult, biggestVal);
		default:
			return Quat();
        }
	}
#endif
    Quat Mat4::toQuat(Mat4 const& m)
    {
        Vec3 up = Vec3::Normalize(Vec3(m[4], m[5], m[6]));
        Vec3 forward = Vec3::Normalize(Vec3(m[8], m[9], m[10]));
        Vec3 right = Vec3::Cross(up, forward);
        up = Vec3::Cross(forward, right);

        return Quat::lookRotation(forward, up);

    }

    Mat4& Mat4::operator=(const Mat4& var)
    {
        m[0][0] = var.m[0][0]; m[1][0] = var.m[1][0]; m[2][0] = var.m[2][0]; m[3][0] = var.m[3][0];
        m[0][1] = var.m[0][1]; m[1][1] = var.m[1][1]; m[2][1] = var.m[2][1]; m[3][1] = var.m[3][1];
        m[0][2] = var.m[0][2]; m[1][2] = var.m[1][2]; m[2][2] = var.m[2][2]; m[3][2] = var.m[3][2];
        m[0][3] = var.m[0][3]; m[1][3] = var.m[1][3]; m[2][3] = var.m[2][3]; m[3][3] = var.m[3][3];
        return *this;
    }

    Mat4& Mat4::operator+=(float value)
    {
        m[0][0] += value; m[1][0] += value; m[2][0] += value; m[3][0] += value;
        m[0][1] += value; m[1][1] += value; m[2][1] += value; m[3][1] += value;
        m[0][2] += value; m[1][2] += value; m[2][2] += value; m[3][2] += value;
        m[0][3] += value; m[1][3] += value; m[2][3] += value; m[3][3] += value;
        return *this;
    }

    Mat4& Mat4::operator+=(const Mat4& var)
    {
        m[0][0] += var.m[0][0]; m[1][0] += var.m[1][0]; m[2][0] += var.m[2][0]; m[3][0] += var.m[3][0];
        m[0][1] += var.m[0][1]; m[1][1] += var.m[1][1]; m[2][1] += var.m[2][1]; m[3][1] += var.m[3][1];
        m[0][2] += var.m[0][2]; m[1][2] += var.m[1][2]; m[2][2] += var.m[2][2]; m[3][2] += var.m[3][2];
        m[0][3] += var.m[0][3]; m[1][3] += var.m[1][3]; m[2][3] += var.m[2][3]; m[3][3] += var.m[3][3];
        return *this;
    }

    Mat4& Mat4::operator-=(float value)
    {
        m[0][0] -= value; m[1][0] -= value; m[2][0] -= value; m[3][0] -= value;
        m[0][1] -= value; m[1][1] -= value; m[2][1] -= value; m[3][1] -= value;
        m[0][2] -= value; m[1][2] -= value; m[2][2] -= value; m[3][2] -= value;
        m[0][3] -= value; m[1][3] -= value; m[2][3] -= value; m[3][3] -= value;
        return *this;
    }

    Mat4& Mat4::operator-=(const Mat4& var)
    {
        m[0][0] -= var.m[0][0]; m[1][0] -= var.m[1][0]; m[2][0] -= var.m[2][0]; m[3][0] -= var.m[3][0];
        m[0][1] -= var.m[0][1]; m[1][1] -= var.m[1][1]; m[2][1] -= var.m[2][1]; m[3][1] -= var.m[3][1];
        m[0][2] -= var.m[0][2]; m[1][2] -= var.m[1][2]; m[2][2] -= var.m[2][2]; m[3][2] -= var.m[3][2];
        m[0][3] -= var.m[0][3]; m[1][3] -= var.m[1][3]; m[2][3] -= var.m[2][3]; m[3][3] -= var.m[3][3];
        return *this;
    }

    Mat4& Mat4::operator*=(float value)
    {
        m[0][0] *= value; m[1][0] *= value; m[2][0] *= value; m[3][0] *= value;
        m[0][1] *= value; m[1][1] *= value; m[2][1] *= value; m[3][1] *= value;
        m[0][2] *= value; m[1][2] *= value; m[2][2] *= value; m[3][2] *= value;
        m[0][3] *= value; m[1][3] *= value; m[2][3] *= value; m[3][3] *= value;
        return *this;
    }

    Mat4& Mat4::operator*=(Mat4 const& var)
    {
        return (*this = *this * var);
    }

    Mat4& Mat4::operator/=(float value)
    {
        m[0][0] /= value; m[1][0] /= value; m[2][0] /= value; m[3][0] /= value;
        m[0][1] /= value; m[1][1] /= value; m[2][1] /= value; m[3][1] /= value;
        m[0][2] /= value; m[1][2] /= value; m[2][2] /= value; m[3][2] /= value;
        m[0][3] /= value; m[1][3] /= value; m[2][3] /= value; m[3][3] /= value;
        return *this;
    }

    float& Mat4::operator[](int index)
    {
        switch (index)
        {
            case 0: return m[0][0];
            case 1: return m[0][1];
            case 2: return m[0][2];
            case 3: return m[0][3];
            case 4: return m[1][0];
            case 5: return m[1][1];
            case 6: return m[1][2];
            case 7: return m[1][3];
            case 8: return m[2][0];
            case 9: return m[2][1];
            case 10: return m[2][2];
            case 11: return m[2][3];
            case 12: return m[3][0];
            case 13: return m[3][1];
            case 14: return m[3][2];
            case 15: return m[3][3];
            default: throw std::out_of_range("Index out of range");
        }
    }

    float const& Mat4::operator[](int index) const
    {
        switch (index)
        {
            case 0: return m[0][0];
            case 1: return m[0][1];
            case 2: return m[0][2];
            case 3: return m[0][3];
            case 4: return m[1][0];
            case 5: return m[1][1];
            case 6: return m[1][2];
            case 7: return m[1][3];
            case 8: return m[2][0];
            case 9: return m[2][1];
            case 10: return m[2][2];
            case 11: return m[2][3];
            case 12: return m[3][0];
            case 13: return m[3][1];
            case 14: return m[3][2];
            case 15: return m[3][3];
            default: throw std::out_of_range("Index out of range");
        }
    }

    Mat4 operator-(const Mat4& var)
    {
        return Mat4(
            -var.m[0][0], -var.m[0][1], -var.m[0][2], -var.m[0][3],
            -var.m[1][0], -var.m[1][1], -var.m[1][2], -var.m[1][3],
            -var.m[2][0], -var.m[2][1], -var.m[2][2], -var.m[2][3],
            -var.m[3][0], -var.m[3][1], -var.m[3][2], -var.m[3][3]
        );
    }

    Mat4 operator+(Mat4 const& var, float const& value)
    {
        return Mat4(
            var.m[0][0] + value, var.m[0][1] + value, var.m[0][2] + value, var.m[0][3] + value,
            var.m[1][0] + value, var.m[1][1] + value, var.m[1][2] + value, var.m[1][3] + value,
            var.m[2][0] + value, var.m[2][1] + value, var.m[2][2] + value, var.m[2][3] + value,
            var.m[3][0] + value, var.m[3][1] + value, var.m[3][2] + value, var.m[3][3] + value
        );
    }

    Mat4 operator+(float const& value, Mat4 const& var)
    {
        return Mat4(
            var.m[0][0] + value, var.m[0][1] + value, var.m[0][2] + value, var.m[0][3] + value,
            var.m[1][0] + value, var.m[1][1] + value, var.m[1][2] + value, var.m[1][3] + value,
            var.m[2][0] + value, var.m[2][1] + value, var.m[2][2] + value, var.m[2][3] + value,
            var.m[3][0] + value, var.m[3][1] + value, var.m[3][2] + value, var.m[3][3] + value
        );
    }

    Mat4 operator+(Mat4 const& var1, Mat4 const& var2)
    {
        return Mat4(
            var1.m[0][0] + var2.m[0][0], var1.m[0][1] + var2.m[0][1], var1.m[0][2] + var2.m[0][2], var1.m[0][3] + var2.m[0][3],
            var1.m[1][0] + var2.m[1][0], var1.m[1][1] + var2.m[1][1], var1.m[1][2] + var2.m[1][2], var1.m[1][3] + var2.m[1][3],
            var1.m[2][0] + var2.m[2][0], var1.m[2][1] + var2.m[2][1], var1.m[2][2] + var2.m[2][2], var1.m[2][3] + var2.m[2][3],
            var1.m[3][0] + var2.m[3][0], var1.m[3][1] + var2.m[3][1], var1.m[3][2] + var2.m[3][2], var1.m[3][3] + var2.m[3][3]
        );
    }

    Mat4 operator-(Mat4 const& var, float const& value)
    {
        return Mat4(
            var.m[0][0] - value, var.m[0][1] - value, var.m[0][2] - value, var.m[0][3] - value,
            var.m[1][0] - value, var.m[1][1] - value, var.m[1][2] - value, var.m[1][3] - value,
            var.m[2][0] - value, var.m[2][1] - value, var.m[2][2] - value, var.m[2][3] - value,
            var.m[3][0] - value, var.m[3][1] - value, var.m[3][2] - value, var.m[3][3] - value
        );
    }

    Mat4 operator-(float const& value, Mat4 const& var)
    {
        return Mat4(
            value - var.m[0][0], value - var.m[0][1], value - var.m[0][2], value - var.m[0][3],
            value - var.m[1][0], value - var.m[1][1], value - var.m[1][2], value - var.m[1][3],
            value - var.m[2][0], value - var.m[2][1], value - var.m[2][2], value - var.m[2][3],
            value - var.m[3][0], value - var.m[3][1], value - var.m[3][2], value - var.m[3][3]
        );
    }

    Mat4 operator-(Mat4 const& var1, Mat4 const& var2)
    {
        return Mat4(
            var1.m[0][0] - var2.m[0][0], var1.m[0][1] - var2.m[0][1], var1.m[0][2] - var2.m[0][2], var1.m[0][3] - var2.m[0][3],
            var1.m[1][0] - var2.m[1][0], var1.m[1][1] - var2.m[1][1], var1.m[1][2] - var2.m[1][2], var1.m[1][3] - var2.m[1][3],
            var1.m[2][0] - var2.m[2][0], var1.m[2][1] - var2.m[2][1], var1.m[2][2] - var2.m[2][2], var1.m[2][3] - var2.m[2][3],
            var1.m[3][0] - var2.m[3][0], var1.m[3][1] - var2.m[3][1], var1.m[3][2] - var2.m[3][2], var1.m[3][3] - var2.m[3][3]
        );
    }

    Mat4 operator*(Mat4 const& var, float const& value)
    {
        return Mat4(
            var.m[0][0] * value, var.m[0][1] * value, var.m[0][2] * value, var.m[0][3] * value,
            var.m[1][0] * value, var.m[1][1] * value, var.m[1][2] * value, var.m[1][3] * value,
            var.m[2][0] * value, var.m[2][1] * value, var.m[2][2] * value, var.m[2][3] * value,
            var.m[3][0] * value, var.m[3][1] * value, var.m[3][2] * value, var.m[3][3] * value
        );
    }

    Mat4 operator*(float const& value, Mat4 const& var)
    {
        return Mat4(
            value * var.m[0][0], value * var.m[0][1], value * var.m[0][2], value * var.m[0][3],
            value * var.m[1][0], value * var.m[1][1], value * var.m[1][2], value * var.m[1][3],
            value * var.m[2][0], value * var.m[2][1], value * var.m[2][2], value * var.m[2][3],
            value * var.m[3][0], value * var.m[3][1], value * var.m[3][2], value * var.m[3][3]
        );
    }

    Mat4 operator*(Mat4 const& lhs, Mat4 const& rhs)
    {
        float m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33;
        m00 = lhs.m[0][0] * rhs.m[0][0] + lhs.m[1][0] * rhs.m[0][1] + lhs.m[2][0] * rhs.m[0][2] + lhs.m[3][0] * rhs.m[0][3];
        m01 = lhs.m[0][1] * rhs.m[0][0] + lhs.m[1][1] * rhs.m[0][1] + lhs.m[2][1] * rhs.m[0][2] + lhs.m[3][1] * rhs.m[0][3];
        m02 = lhs.m[0][2] * rhs.m[0][0] + lhs.m[1][2] * rhs.m[0][1] + lhs.m[2][2] * rhs.m[0][2] + lhs.m[3][2] * rhs.m[0][3];
        m03 = lhs.m[0][3] * rhs.m[0][0] + lhs.m[1][3] * rhs.m[0][1] + lhs.m[2][3] * rhs.m[0][2] + lhs.m[3][3] * rhs.m[0][3];
        m10 = lhs.m[0][0] * rhs.m[1][0] + lhs.m[1][0] * rhs.m[1][1] + lhs.m[2][0] * rhs.m[1][2] + lhs.m[3][0] * rhs.m[1][3];
        m11 = lhs.m[0][1] * rhs.m[1][0] + lhs.m[1][1] * rhs.m[1][1] + lhs.m[2][1] * rhs.m[1][2] + lhs.m[3][1] * rhs.m[1][3];
        m12 = lhs.m[0][2] * rhs.m[1][0] + lhs.m[1][2] * rhs.m[1][1] + lhs.m[2][2] * rhs.m[1][2] + lhs.m[3][2] * rhs.m[1][3];
        m13 = lhs.m[0][3] * rhs.m[1][0] + lhs.m[1][3] * rhs.m[1][1] + lhs.m[2][3] * rhs.m[1][2] + lhs.m[3][3] * rhs.m[1][3];
        m20 = lhs.m[0][0] * rhs.m[2][0] + lhs.m[1][0] * rhs.m[2][1] + lhs.m[2][0] * rhs.m[2][2] + lhs.m[3][0] * rhs.m[2][3];
        m21 = lhs.m[0][1] * rhs.m[2][0] + lhs.m[1][1] * rhs.m[2][1] + lhs.m[2][1] * rhs.m[2][2] + lhs.m[3][1] * rhs.m[2][3];
        m22 = lhs.m[0][2] * rhs.m[2][0] + lhs.m[1][2] * rhs.m[2][1] + lhs.m[2][2] * rhs.m[2][2] + lhs.m[3][2] * rhs.m[2][3];
        m23 = lhs.m[0][3] * rhs.m[2][0] + lhs.m[1][3] * rhs.m[2][1] + lhs.m[2][3] * rhs.m[2][2] + lhs.m[3][3] * rhs.m[2][3];
        m30 = lhs.m[0][0] * rhs.m[3][0] + lhs.m[1][0] * rhs.m[3][1] + lhs.m[2][0] * rhs.m[3][2] + lhs.m[3][0] * rhs.m[3][3];
        m31 = lhs.m[0][1] * rhs.m[3][0] + lhs.m[1][1] * rhs.m[3][1] + lhs.m[2][1] * rhs.m[3][2] + lhs.m[3][1] * rhs.m[3][3];
        m32 = lhs.m[0][2] * rhs.m[3][0] + lhs.m[1][2] * rhs.m[3][1] + lhs.m[2][2] * rhs.m[3][2] + lhs.m[3][2] * rhs.m[3][3];
        m33 = lhs.m[0][3] * rhs.m[3][0] + lhs.m[1][3] * rhs.m[3][1] + lhs.m[2][3] * rhs.m[3][2] + lhs.m[3][3] * rhs.m[3][3];
        return Mat4(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );
    }

    Vec4 operator*(Mat4 const& mat, Vec4 const& vec)
    {
        float vec0, vec1, vec2, vec3;
        vec0 = mat.m[0][0] * vec.x + mat.m[1][0] * vec.y + mat.m[2][0] * vec.z + mat.m[3][0] * vec.w;
        vec1 = mat.m[0][1] * vec.x + mat.m[1][1] * vec.y + mat.m[2][1] * vec.z + mat.m[3][1] * vec.w;
        vec2 = mat.m[0][2] * vec.x + mat.m[1][2] * vec.y + mat.m[2][2] * vec.z + mat.m[3][2] * vec.w;
        vec3 = mat.m[0][3] * vec.x + mat.m[1][3] * vec.y + mat.m[2][3] * vec.z + mat.m[3][3] * vec.w;

        return Vec4(vec0, vec1, vec2, vec3);
    }

    Mat4 operator/(Mat4 const& var, float const& value)
    {
        return Mat4(
            var.m[0][0] / value, var.m[0][1] / value, var.m[0][2] / value, var.m[0][3] / value,
            var.m[1][0] / value, var.m[1][1] / value, var.m[1][2] / value, var.m[1][3] / value,
            var.m[2][0] / value, var.m[2][1] / value, var.m[2][2] / value, var.m[2][3] / value,
            var.m[3][0] / value, var.m[3][1] / value, var.m[3][2] / value, var.m[3][3] / value
        );
    }

    Mat4 operator/(float const& value, Mat4 const& var)
    {
        return Mat4(
            value / var.m[0][0], value / var.m[0][1], value / var.m[0][2], value / var.m[0][3],
            value / var.m[1][0], value / var.m[1][1], value / var.m[1][2], value / var.m[1][3],
            value / var.m[2][0], value / var.m[2][1], value / var.m[2][2], value / var.m[2][3],
            value / var.m[3][0], value / var.m[3][1], value / var.m[3][2], value / var.m[3][3]
        );
    }

    bool operator==(Mat4 const& var1, Mat4 const& var2)
    {
        return (
            var1.m[0][0] == var2.m[0][0] && var1.m[0][1] == var2.m[0][1] && var1.m[0][2] == var2.m[0][2] && var1.m[0][3] == var2.m[0][3] &&
            var1.m[1][0] == var2.m[1][0] && var1.m[1][1] == var2.m[1][1] && var1.m[1][2] == var2.m[1][2] && var1.m[1][3] == var2.m[1][3] &&
            var1.m[2][0] == var2.m[2][0] && var1.m[2][1] == var2.m[2][1] && var1.m[2][2] == var2.m[2][2] && var1.m[2][3] == var2.m[2][3] &&
            var1.m[3][0] == var2.m[3][0] && var1.m[3][1] == var2.m[3][1] && var1.m[3][2] == var2.m[3][2] && var1.m[3][3] == var2.m[3][3]
        );
    }

    bool operator!=(Mat4 const& var1, Mat4 const& var2)
    {
        return !(var1 == var2);
    }
    std::ostream& operator<<(std::ostream& os, const Mat4& var)
    {
        std::stringstream m00, m01, m02, m03;
        std::stringstream m10, m11, m12, m13;
        std::stringstream m20, m21, m22, m23;
        std::stringstream m30, m31, m32, m33;
        m00 << var.m[0][0]; m10 << var.m[1][0]; m20 << var.m[2][0]; m30 << var.m[3][0];
        m01 << var.m[0][1]; m11 << var.m[1][1]; m21 << var.m[2][1]; m31 << var.m[3][1];
        m02 << var.m[0][2]; m12 << var.m[1][2]; m22 << var.m[2][2]; m32 << var.m[3][2];
        m03 << var.m[0][3]; m13 << var.m[1][3]; m23 << var.m[2][3]; m33 << var.m[3][3];
        int maxLength[] = { static_cast<int>(m00.str().size()), static_cast<int>(m01.str().size()), static_cast<int>(m02.str().size()), static_cast<int>(m03.str().size()),
                              static_cast<int>(m10.str().size()), static_cast<int>(m11.str().size()), static_cast<int>(m12.str().size()), static_cast<int>(m13.str().size()),
                              static_cast<int>(m20.str().size()), static_cast<int>(m21.str().size()), static_cast<int>(m22.str().size()), static_cast<int>(m23.str().size()),
                              static_cast<int>(m30.str().size()), static_cast<int>(m31.str().size()), static_cast<int>(m32.str().size()), static_cast<int>(m33.str().size()) };
        int maxLen = Mathf::Max(maxLength, 16);
        os  << std::left
            << "| " << std::setw(maxLen) << var.m[0][0] << " " << std::setw(maxLen) << var.m[1][0] << " " << std::setw(maxLen) << var.m[2][0] << " " << std::setw(maxLen) << var.m[3][0] << " |" << std::endl
            << "| " << std::setw(maxLen) << var.m[0][1] << " " << std::setw(maxLen) << var.m[1][1] << " " << std::setw(maxLen) << var.m[2][1] << " " << std::setw(maxLen) << var.m[3][1] << " |" << std::endl
            << "| " << std::setw(maxLen) << var.m[0][2] << " " << std::setw(maxLen) << var.m[1][2] << " " << std::setw(maxLen) << var.m[2][2] << " " << std::setw(maxLen) << var.m[3][2] << " |" << std::endl
            << "| " << std::setw(maxLen) << var.m[0][3] << " " << std::setw(maxLen) << var.m[1][3] << " " << std::setw(maxLen) << var.m[2][3] << " " << std::setw(maxLen) << var.m[3][3] << " |";
        return os;
    }

    Vec4 mat4Vec4(Mat4& mat, Vec4& vec)
    {
        return mat * vec;
    }

}  // namespace WD