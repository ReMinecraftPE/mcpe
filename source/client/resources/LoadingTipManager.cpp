#include "LoadingTipManager.hpp"
#include "Resource.hpp"
#include "common/Util.hpp"
#include "client/locale/Language.hpp"
#include "thirdparty/rapidjson/document.h"

LoadingTip LoadingTip::EMPTY(Util::EMPTY_STRING);

LoadingTipManager* LoadingTipManager::instance = nullptr;

LoadingTipManager::LoadingTipManager() :
	m_actual(LoadingTip::EMPTY),
	m_actualTime(0)
{
}

void LoadingTipManager::init()
{
	m_all.clear();
	m_using.clear();

	using namespace rapidjson;

	std::string file;
	if (!Resource::load("texts/tips.json", file)) return;
	Document d;
	d.Parse(file.c_str());

	if (d.Empty()) return;

	for (Value::ConstValueIterator it = d.Begin(); it != d.End(); ++it)
	{
		if (it->IsString())
			m_all.push_back(LoadingTip(it->GetString()));
		else if (it->IsObject())
		{
			Value::ConstObject translateValue = it->GetObj();
			Value::ConstMemberIterator translate = translateValue.FindMember("translate");
			Value::ConstMemberIterator time = translateValue.FindMember("time");

			if (translate != translateValue.MemberEnd())
			{
				std::string translation = Language::singleton().get(translate->value.GetString());
				if (time == translateValue.MemberEnd())
					m_all.push_back(LoadingTip(translation));
				else
					m_all.push_back(LoadingTip(translation, time->value.GetInt()));
			}
		}
	}
}

const LoadingTip& LoadingTipManager::getActual()
{
	if (m_using.empty())
	{
		if (m_all.empty()) return LoadingTip::EMPTY;
		else
		{
			for (std::vector<LoadingTip>::iterator it = m_all.begin(); it != m_all.end(); ++it)
			{
				m_using.push_back(&(*it));
			}
		}
	}

	if (m_actual.text.empty())
	{
		int i = m_random.nextInt(m_using.size());
		m_actual = *m_using[i];
		m_actualTime = getTimeMs();
	}
	else if (getTimeMs() - m_actualTime >= m_actual.timeOnScreen)
	{
		m_actual = LoadingTip::EMPTY;
		m_actualTime = 0;
		return getActual();
	}

	return m_actual;
}
