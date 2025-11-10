#include "EnableScissorTest.hpp"

using namespace mce;

int EnableScissorTest::activeScissorBox[4];
bool EnableScissorTest::scissorTestEnabled;

void EnableScissorTest::_init(bool enableTest)
{
    m_testEnabled = enableTest;
    scissorTestEnabled = enableTest;
}

EnableScissorTest::EnableScissorTest(bool enableTest)
{
	_init(enableTest);
}

EnableScissorTest::EnableScissorTest(int x, int y, int w, int h)
{
	_init(true);
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