#pragma once

struct TickNextTickData
{
	static int C;

	TickNextTickData(int a, int b, int c, int d);
	int hashCode() const;
	bool operator<(const TickNextTickData& other) const;
	bool operator==(const TickNextTickData& other) const;
	void setDelay(long);

	int m_ID;
	int field_4;
	int field_8;
	int field_C;
	int field_10;
	long m_delay;
};

