#include "EnableScissorTest.hpp"

using namespace mce;

EnableScissorTest::EnableScissorTest(bool enableTest)
{
    m_testEnabled = enableTest;
    scissorTestEnabled = enableTest;
}

EnableScissorTest::EnableScissorTest(int x, int y, int w, int h)
    : EnableScissorTest(true)
{
    activeScissorBox[0] = x;
    activeScissorBox[1] = y;
    activeScissorBox[2] = w;
    activeScissorBox[3] = h;
}

EnableScissorTest::~EnableScissorTest()
{
    if (m_testEnabled)
        scissorTestEnabled = false;
}