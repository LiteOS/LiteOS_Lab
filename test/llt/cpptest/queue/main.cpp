#include <cpptest.h>
#include <iostream>
#include <fstream>

// zz
#include "test_queue.h"


int main()
{
    Test::Suite ts;

    ts.add(std::auto_ptr<Test::Suite>(new TestQueue));

    std::ofstream html;
    html.open("Result.htm");

    Test::HtmlOutput output;
    ts.run(output);
    output.generate(html);
    html.close();
}
