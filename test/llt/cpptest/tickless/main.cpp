#include <cpptest.h>
#include <iostream>
#include <fstream>

#include "test_tickless.h"

int main()
{
    Test::Suite ts;

	ts.add(std::auto_ptr<Test::Suite>(new TestTickless));

    std::ofstream html;
    html.open("Result.htm");

    Test::HtmlOutput output;
    ts.run(output);
    output.generate(html);
    html.close();
}