#pragma once
#include <math.h>

namespace math {
    class Vec3D {
        friend Vec3D operator*(const Vec3D &l, float s);
        friend Vec3D operator*(float s, const Vec3D& r);

        friend Vec3D operator/(const Vec3D &l, float s);
        friend Vec3D operator/(float s, const Vec3D& r);

        // Dot product.
        friend float operator*(const Vec3D &l, const Vec3D &r);

        friend Vec3D operator+(const Vec3D &l, const Vec3D &r);
        friend Vec3D operator-(const Vec3D &l, const Vec3D &r);
        friend Vec3D operator-(const Vec3D &v);

    private:
        float x, y, z;

    public:
        inline Vec3D() : x(0), y(0), z(0) {}
        inline Vec3D(float i, float j, float k) : x(i), y(j), z(k) {}

        inline float getX() const {
            return this->x;
        }

        inline float getY() const {
            return this->y;
        }

        inline float getZ() const {
            return this->z;
        }

        Vec3D cross(const Vec3D &o) const;
        
        inline float mag() const {
            return sqrt((*this) * (*this));
        }

        inline Vec3D proj(const Vec3D &o) const {
            return (((*this) * o) / (o * o)) * o;
        }

        inline Vec3D& operator*=(float s) {
            this->x *= s;
            this->y *= s;
            this->z *= s;

            return *this;
        }

        inline Vec3D& operator+=(const Vec3D &o) {
            this->x += o.x;
            this->y += o.y;
            this->z += o.z;

            return *this;
        }

        inline Vec3D& operator-=(const Vec3D &o) {
            this->x -= o.x;
            this->y -= o.y;
            this->z -= o.z;

            return *this;
        }

        void display() const;
    };

    inline Vec3D operator*(const Vec3D &l, float s) {
        return Vec3D(l.x * s, l.y * s, l.z * s);
    }

    inline Vec3D operator*(float s, const Vec3D& r) {
        return Vec3D(r.x * s, r.y * s, r.z * s);
    }

    inline Vec3D operator/(const Vec3D &l, float s) {
        return Vec3D(l.x / s, l.y / s, l.z / s);
    }

    inline Vec3D operator/(float s, const Vec3D& r) {
        return Vec3D(r.x / s, r.y / s, r.z / s);
    }

    inline float operator*(const Vec3D &l, const Vec3D &r) {
        return (l.x * r.x) + (l.y * r.y) + (l.z * r.z);
    }

    inline Vec3D operator+(const Vec3D &l, const Vec3D &r) {
        return Vec3D(l.x + r.x, l.y + r.y, l.z + r.z);
    }

    inline Vec3D operator-(const Vec3D &l, const Vec3D &r) {
        return Vec3D(l.x - r.x, l.y - r.y, l.z - r.z);
    }

    inline Vec3D operator-(const Vec3D &v) {
        return Vec3D(-v.x, -v.y, -v.z);
    }
}
