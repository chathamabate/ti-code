#ifndef TEST_HPP
#define TEST_HPP

#include <stdint.h>

namespace cxxutil {
    class Student {
    private:
        int age;

    public:
        Student(int age);
        void sayAge();
        int getAge();
    };


    class Shape {
        public:
            virtual ~Shape();

            virtual int area() = 0;
            virtual int perimeter() = 0;

            void saySomething();
    };

    class Square : public Shape {
        private:
            int side;
        public:
            Square(int s);   
            virtual ~Square() override;

            virtual int area() override;
            virtual int perimeter() override;
    };

    class Rectangle : public Shape {
        private:
            int length;
            int  width;
        public:
            Rectangle(int l, int w);   
            virtual ~Rectangle() override;

            virtual int area() override;
            virtual int perimeter() override;
    };

    template <typename T>
    class C {
        private:
            T data;
        public:
            C(T d);
    };
}

#endif
