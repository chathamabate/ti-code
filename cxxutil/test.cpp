#include "test.h"
#include "ti/screen.h"

#include <tice.h>
#include <stdio.h>

using namespace cxxutil;

Student::Student(int a) {
    this->age = a;
}

void Student::sayAge() {
    char buff[40];
    sprintf(buff, "My age is %d", this->age);

    os_PutStrFull(buff);
    os_NewLine();
}

int Student::getAge() {
    return this->age;
}

Shape::~Shape() {
    os_PutStrFull("Shape Destructor!");
    os_NewLine();
}

void Shape::saySomething() {
    os_PutStrFull("Something");
    os_NewLine();
}

Square::Square(int s) {
    this->side = s;
}

Square::~Square() {
    os_PutStrFull("Square Destructor!");
    os_NewLine();
}

int Square::area() {
    return this->side * this->side; 
}

int Square::perimeter() {
    return 4 * this->side;
}

Rectangle::Rectangle(int l, int w) {
    this->length = l;
    this->width = w;
}

Rectangle::~Rectangle() {
    os_PutStrFull("Rectangle Destructor!");
    os_NewLine();
}

int Rectangle::area() {
    return this->length*this->width;
}

int Rectangle::perimeter() {
    return 2*this->width + 2*this->length;
}

template <typename T> 
C<T>::C(T d) {
    this->data = d;
}

// From here out this works though...
// Must occur in namespace I guess??
template class cxxutil::C<int>;


