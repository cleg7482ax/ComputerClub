#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>

using namespace std;

class ComputerClub
{
	struct Event // структура события
	{
		string time; // время события
		int id; //id ситуации
		string name; // имя клиента
	};

	struct Table
	{
		string entryTime = ""; // время посадки за стол
		int entryHour = 0;
		int entryMinute = 0;
		string exitTime = ""; // время высадки из-за стола
		int exitHour = 0;
		int exitMinute = 0;
		int entryTotalMinute = 0; 
		int exitTotalMinute = 0;
	};

	unordered_map <int, int> moneyTables; // выручка столов
	unordered_map <int, int> timeTables; // время за столом

	int tableNumber; // текущий номер стола
	int numTables; // количество столов
	string startTime; // время начала работы
	string endTime; // время конца работы
	int price; // стоимость стола в час
	ifstream file; // рабочий файл с событиями
	unordered_set <string> names; // список находящихся в клубе клиентов
	unordered_map <int, bool> tables; // список столов
	unordered_map <string, int> namesTables; // список гостей за столами
	queue <string> queClients; // очередь из клиентов

public:
	ComputerClub(){} // конструктор по умолчанию

	void convertTimeFormat(const string& time, int& hours, int& minutes); // функция преобразования формата времени ХХ:ХХ в часы и минуты
	void displayEventInfo(const Event& event);
	void GetValue(const string& filename);	
	bool isValidTimeFormat(const string& time);

};

bool ComputerClub::isValidTimeFormat(const string& time)
{
	// Формат времени ЧЧ:ММ имеет длину 5 символов и разделен двоеточием.
	if (time.length() != 5 || time[2] != ':')
		return false;

	// Проверяем, что все символы до и после двоеточия являются цифрами.
	for (int i = 0; i < 5; ++i)
	{
		if (i == 2)
			continue; // Пропускаем символ двоеточия
		if (!isdigit(time[i]))
			return false;
	}

	// Проверяем, что значения часов и минут находятся в разумных пределах.
	int hours = stoi(time.substr(0, 2));
	int minutes = stoi(time.substr(3, 2));
	if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
		return false;

	return true;
}

void ComputerClub:: GetValue(const string& filename)
{
	file.open(filename);
	if (!file.is_open())  // проверка открытия файла
	{
		cout << "Error: Cannot open file " << filename << endl;
		return;
	}

	file >> numTables >> startTime >> endTime >> price; // заносим начальные данные в переменные
	cout << startTime << endl; // вывод времени начала работы

	// заполняем map количеством столов (изначально все свободны)
	for (int i = 0; i < numTables; ++i)
	{
		tables.insert({ i,false });
		moneyTables.insert({ i,0 });
		timeTables.insert({ i,0 });
	}

	int startHour, startMinute, endHour, endMinute;
	// переводим часы и минуты в удобный формат
	convertTimeFormat(startTime, startHour, startMinute);
	convertTimeFormat(endTime, endHour, endMinute);
	
	Event event;
	Table * table = new Table[numTables];
	int currentHour, currentMinute;
	string prevTime = startTime; 
	bool firstClientBeforeOpening = false;

	while (file >> event.time >> event.id >> event.name)
	{
		displayEventInfo(event);
		convertTimeFormat(event.time, currentHour, currentMinute);

		auto nm = names.find(event.name);

		switch (event.id)
		{
		case 1:
			// находится ли клиент в клубе
			if (nm != names.end())
			{
				cout << endl << "13 YouShallNotPass";
			}
			// открыт ли клуб
			else if (currentHour < startHour || currentHour >= endHour)
			{
				firstClientBeforeOpening = true;
				cout << endl << event.time << " 13 NotOpenYet";
			}
			// заносим клиента в список посетителей
			else
			{
				firstClientBeforeOpening = false;
				names.insert(event.name);
			}
			break;
		case 2:
			file >> tableNumber;
			cout << " " << tableNumber;
			// находится ли клиент в клубе
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			// занят ли стол по номеру tableNumber
			else if (tables[tableNumber])
			{
				cout << endl << event.time << " 13 PlaceIsBusy";
				queClients.push(event.name);
			}
			// если стол свободен 
			else
			{
				// отмечаем стол как занятый
				tables[tableNumber] = true;
				namesTables.insert({ event.name,tableNumber });

				// вносим время начала брони стола 
				table[tableNumber - 1].entryTime = event.time;
				convertTimeFormat(table[tableNumber - 1].entryTime, table[tableNumber - 1].entryHour, table[tableNumber - 1].entryMinute);
				table[tableNumber - 1].entryTotalMinute = table[tableNumber - 1].entryHour * 60 + table[tableNumber - 1].entryMinute;
			}
			break;
		case 3:
			for (int i = 0; i < numTables; ++i)
			{
				if (!tables[tableNumber])
				{
					cout << endl << event.time << " 13 ICanWaitNoLonger!";
					break;
				}
			}
			if (queClients.size() > numTables)
			{
				cout << endl << event.time << " 11 " << event.name;
				names.erase(event.name);
				queClients.pop();
			}
			break;
		case 4:
			// находится ли клиент в клубе
			if (nm == names.end())
			{
				cout << endl << " 13 ClientUnknown";
			}
			else
			{
				// освобождение стола, за которым сидел клиент	
				tableNumber = namesTables[event.name];
				tables[tableNumber] = false;
				
				// вносим время конца брони стола
				table[tableNumber-1].exitTime = event.time;
				convertTimeFormat(table[tableNumber - 1].exitTime, table[tableNumber - 1].exitHour, table[tableNumber - 1].exitMinute);
				table[tableNumber - 1].exitTotalMinute = table[tableNumber - 1].exitHour * 60 + table[tableNumber - 1].exitMinute;
				// подсчет выручки и времени 
				moneyTables[tableNumber] += (ceil(static_cast<double>(table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute) / 60.0)) * price;
				timeTables[tableNumber] += table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute;

				// удаление клиента из списка посетителей
				names.erase(event.name);
				namesTables.erase(event.name);
			}		
			if (!queClients.empty())
			{
				cout << endl << event.time << " 12 " << queClients.front() << " " << tableNumber;
				namesTables.insert({ queClients.front(),tableNumber });
				queClients.pop();
				tables[tableNumber] = true;		
				table[tableNumber - 1].entryTime = event.time;
				convertTimeFormat(table[tableNumber - 1].entryTime, table[tableNumber - 1].entryHour, table[tableNumber - 1].entryMinute);
				table[tableNumber - 1].entryTotalMinute = table[tableNumber - 1].entryHour * 60 + table[tableNumber - 1].entryMinute;
			}
			break;
		}

		// проверка формата времени
		if (!isValidTimeFormat(event.time))
		{
			file.close();
			return;
		}

		// проверка формата последовательности событий
		if (event.time < prevTime && !firstClientBeforeOpening)
		{
			file.close();
			return;
		}

		// проверка формата имени клиента
		for (char ch : event.name)
		{
			if (!isalnum(ch) && ch != '_' && ch != '-')
			{
				file.close();
				return;
			}
		}

		cout << endl;
		prevTime = event.time;
	}

	while (!names.empty())
	{
		auto it = names.begin();
		cout << endTime << " 11 " << *it << endl;
		tableNumber = namesTables[*it];
		table[tableNumber - 1].exitTime = endTime;
		convertTimeFormat(table[tableNumber - 1].exitTime, table[tableNumber - 1].exitHour, table[tableNumber - 1].exitMinute);
		table[tableNumber - 1].exitTotalMinute = table[tableNumber - 1].exitHour * 60 + table[tableNumber - 1].exitMinute;
		moneyTables[tableNumber] += (ceil(static_cast<double>(table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute) / 60.0)) * price;
		timeTables[tableNumber] += table[tableNumber - 1].exitTotalMinute - table[tableNumber - 1].entryTotalMinute;
		names.erase(it);	
	}

	cout << endTime << endl; // вывод времени конца работы

	// вывод выручки
	for (int i = 0; i < numTables; ++i)
	{
		int hours = timeTables[i + 1] / 60;
		int minutes = timeTables[i + 1] % 60;
		cout << i + 1 << " " << moneyTables[i + 1] << " " << setfill('0') << setw(2) << hours << ":" << setw(2) << minutes << endl;
	}
	

	file.close();
}

void ComputerClub::convertTimeFormat(const string& time, int& hours, int& minutes) 
{
	hours = stoi(time.substr(0, 2));
	minutes = stoi(time.substr(3, 2));
}

void ComputerClub:: displayEventInfo(const Event& event) 
{
	cout << event.time << " " << event.id << " " << event.name;
}


int main(int argc, char* argv[])
{

	ComputerClub A;
	A.GetValue(argv[1]);

	return 0;
}
