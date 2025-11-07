#pragma once

namespace mce
{
    class EnableScissorTest
    {
    public:
        static int activeScissorBox[4];
        static bool scissorTestEnabled;

    private:
        bool m_testEnabled;

    public:
        EnableScissorTest(bool enableTest);
        EnableScissorTest(int x, int y, int w, int h);
        ~EnableScissorTest();
    };
}