#pragma once

template <class T>
class TempPractice {
private:
	T m_data;
public:
	TempPractice() : m_data( T() ) {}

	void SetData( T data ) {
		m_data = data;
	}

	T GetData() {
		m_data = m_data % 10;
		return m_data;
	}
};