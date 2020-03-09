#include <cpptest.h>
#include <iostream>
#include <fstream>

#include "test_storage.h"

int main()
{
    Test::Suite ts;

	ts.add(std::auto_ptr<Test::Suite>(new TEST_storage));
    ts.add(std::auto_ptr<Test::Suite>(new TEST_partition));

    std::ofstream html;
    html.open("Result.htm");

    Test::HtmlOutput output;
    ts.run(output);
    output.generate(html);
    html.close();
}
