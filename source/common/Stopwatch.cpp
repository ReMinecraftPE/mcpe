#include <sstream>

#include "Stopwatch.hpp"
#include "Utils.hpp"
#include "Logger.hpp"

Stopwatch::Stopwatch()
	: m_last(0.0),
	  m_count(0),
	  m_printcounter(0)
{
	reset();
}

Stopwatch::~Stopwatch()
{
}

void Stopwatch::start()
{
	m_st = getTimeS();
}

double Stopwatch::stop()
{
	if (isReset())
		return 0.0;

	double time = getTimeS();
	double diff = time - m_st;

	m_last = diff;
	if (diff > m_max)
		m_max = diff;

	m_count++;
	m_st = -1.0;
	m_tt += diff;
	return m_tt;
}

double Stopwatch::stopContinue()
{
	if (isReset())
		return 0.0;

	double time = getTimeS();
	double diff = time - m_st;

	m_last = diff;
	if (diff > m_max)
		m_max = diff;

	m_count++;
	m_st = time;
	m_tt += diff;
	return m_tt;
}

double Stopwatch::tick() const
{
	if (isReset())
		return 0.0;

	double time = getTimeS();
	double diff = time - m_st;

	return diff;
}

void Stopwatch::reset()
{
	m_tt = 0.0;
	m_max = 0.0;
	m_st = -1.0;
}

void Stopwatch::print(const std::string& prepend)
{
	LOG_I("%s\tTime (AVGms/LTs(MAXs)/TTs, C) : %f/%f(%f)/%f, %d", prepend.c_str(), (m_tt * 1000.0) / m_count, m_last, m_max, m_tt, m_count);
}

void Stopwatch::printEvery(int n, const std::string& prepend)
{
	if ((m_printcounter + 1) >= n)
	{
		m_printcounter = 0;
		print(prepend);
	}
	else
	{
		m_printcounter += 1;
	}
}
