#include <cpptest.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "test_coap_core.h"
#include "test_litecoap_port.h"
#include "test_coap_osdepends.h"
#include "test_coap_al.h"


int main()
{
    Test::Suite ts;

    ts.add(std::auto_ptr<Test::Suite>(new TestCoapCore));
    ts.add(std::auto_ptr<Test::Suite>(new TestCoapOsDepends));

    ts.add(std::auto_ptr<Test::Suite>(new TestLiteCoapPort));

    ts.add(std::auto_ptr<Test::Suite>(new TestCoapAL));

    std::ofstream html;
    html.open("Result.htm");

    Test::HtmlOutput output;
    ts.run(output);
    output.generate(html);
    html.close();

    return 0;
}
