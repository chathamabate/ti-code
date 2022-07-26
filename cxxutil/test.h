#ifndef TEST_HPP
#define TEST_HPP


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
            ~Square();

            int area();
            int perimeter();
    };

    class Rectangle : public Shape {
        private:
            int length;
            int  width;
        public:
            Rectangle(int l, int w);   
            ~Rectangle();

            int area();
            int perimeter();
    };

}

#endif
