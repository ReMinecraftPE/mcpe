#include "ConvertWorldScreen.hpp"

ConvertWorldScreen::ConvertWorldScreen(const LevelSummary& level) :
ConfirmScreen(nullptr,
              "A new world storage format is available!",
              "Would you like to convert '" + level.m_levelName + "'?",
              "Convert", "Don't Convert", 0)
{
    m_level = level;
}

void ConvertWorldScreen::postResult(bool b)
{
    if (b)
    {
        // Convert
        m_pMinecraft->selectLevel(m_level, true);
    }
    else
    {
        // Don't Convert
        m_pMinecraft->selectLevel(m_level.m_fileName, m_level.m_levelName, LevelSettings(), false);
    }
}
