#include "test.h"
#include "ti/screen.h"

#include <tice.h>
#include <stdio.h>

using namespace cxxutil;

Student::Student(int a) {
    this->age = a;
}

void Student::SayAge() {
    char buff[40];
    sprintf(buff, "My age is %d", this->age);

    os_PutStrFull(buff);
    os_NewLine();
}
