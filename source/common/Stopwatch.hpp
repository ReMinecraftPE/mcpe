#pragma once
#include <string>

class Stopwatch
{
private:
	double m_st;
	double m_tt;
	double m_last;
	double m_max;
	int m_count;
	int m_printcounter;

public:
	Stopwatch();
	virtual ~Stopwatch();
public:
	void start();
	virtual double stop();
	virtual double stopContinue();
	double getLast() const
	{
		return m_last;
	}
	double getTotal() const
	{
		return m_tt;
	}
	double getMax() const
	{
		return m_max;
	}
	int getCount() const
	{
		return m_count;
	}
	double tick() const;
	bool isReset() const
	{
		return m_st == -1.0;
	}
	void reset();
	void printEvery(int n, const std::string& prepend);
	virtual void print(const std::string& prepend);
};
