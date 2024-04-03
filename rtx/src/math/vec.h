#pragma once

namespace math {
    class Vec3D {
        friend Vec3D operator*(const Vec3D &l, double s);
        friend Vec3D operator*(double s, const Vec3D& r);

        friend Vec3D operator+(const Vec3D &l, const Vec3D &r);
        friend Vec3D operator-(const Vec3D &l, const Vec3D &r);
        friend Vec3D operator-(const Vec3D &v);

    private:
        double x, y, z;

    public:
        inline Vec3D(double i, double j, double k) : x(i), y(j), z(k) {}

        inline double X() const {
            return this->x;
        }

        inline double Y() const {
            return this->y;
        }

        inline double Z() const {
            return this->z;
        }

        inline Vec3D& operator*=(double s) {
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

    inline Vec3D operator*(const Vec3D &l, double s) {
        return Vec3D(l.x * s, l.y * s, l.z * s);
    }

    inline Vec3D operator*(double s, const Vec3D& r) {
        return Vec3D(r.x * s, r.y * s, r.z * s);
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
