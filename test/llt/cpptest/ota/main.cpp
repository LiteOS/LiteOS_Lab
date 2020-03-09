#include <cpptest.h>
#include <iostream>
#include <fstream>

// zz
#include "test_ota_flag.h"
#include "test_pcp.h"
#include "test_ota_checksum.h"
#include "test_ota_pack.h"

int main()
{
    Test::Suite ts;

    ts.add(std::auto_ptr<Test::Suite>(new TestOta));
    ts.add(std::auto_ptr<Test::Suite>(new TestPcp));
    ts.add(std::auto_ptr<Test::Suite>(new TestOtaChecksum));
    ts.add(std::auto_ptr<Test::Suite>(new TestOtaPack));

    std::ofstream html;
    html.open("Result.htm");

    Test::HtmlOutput output;
    ts.run(output);
    output.generate(html);
    html.close();
}
