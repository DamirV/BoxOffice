#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <array>
#include <Windows.h>

class CSVRow
{
public:
	std::string_view operator[](std::size_t index) const
	{
		return std::string_view(&m_line[m_data[index] + 1], m_data[index + 1] - (m_data[index] + 1));
	}

	std::size_t size() const
	{
		return m_data.size() - 1;
	}

	void readNextRow(std::istream& str)
	{
		std::getline(str, m_line);

		m_data.clear();
		m_data.emplace_back(-1);
		std::string::size_type pos = 0;
		while ((pos = m_line.find(';', pos)) != std::string::npos)
		{
			m_data.emplace_back(pos);
			++pos;
		}

		pos = m_line.size();
		m_data.emplace_back(pos);
	}

private:
	std::string m_line;
	std::vector<int> m_data;
};

std::istream& operator>>(std::istream& str, CSVRow& data)
{
	data.readNextRow(str);
	return str;
}

bool is_number(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
		s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

int main()
{
	setlocale(LC_ALL, "Russian");
	const std::string DATA_PATH = ".\\data\\products.csv";

	std::ifstream myFile(DATA_PATH);
	if (!myFile.is_open()) throw std::runtime_error("Could not open file");

	const int columns_count = 9;
	auto print_info = [](const std::pair<std::string, std::array<std::string, columns_count - 1>>& element) {
		std::cout << element.first << " ";

		for (auto it : element.second) {
			std::cout << it << " ";
		}

		std::cout << std::endl;
	};

	std::unordered_map<std::string, std::array<std::string, columns_count - 1>> data;
	std::unordered_map<std::string_view, std::string_view> matches;
	
	CSVRow row;

	while (myFile >> row)
	{
		std::string key(row[0]);
		std::array <std::string, columns_count - 1> value;
		
		for (int i = 1; i < columns_count; i++) {
			value[i - 1] = std::string(row[i]);
		}

		data[key] = std::move(value);
	}

	myFile.close();

	for (const auto& it : data) {
		matches[it.second[0]] = it.first;
	}

	int k = 0;
	for (const auto& it : matches) {
		std::cout << it.first << " - " << it.second << std::endl;
		if (k++ > 10) {
			break;
		}
	}

	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	// run
	std::string input;
	while (true) {
		std::cout << "---------------------------------" << '\n';
		std::cout << "Введите код/имя (!q - для выхода): ";
		std::cin >> input;

		if (input == "!q") {
			break;
		}

		auto search = is_number(input) ?
			data.find(input) : data.find(std::string(matches[input]));

		if (search != data.end()) {
			print_info(*search);
		}

		else {
			std::cout << "Не найдено\n";
		}
	}
}
