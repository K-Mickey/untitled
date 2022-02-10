#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// Класс для хранения даты и обработки основных логических ошибок (количество месяцев в году и дней в месяце)
class Date {
public:
    Date(int new_year, int new_month, int new_day) {

        if (new_month > 12 || new_month < 1) {
            throw logic_error("Month value is invalid: " + to_string(new_month));
        }
        if (new_day > 31 || new_day < 1) {
            throw logic_error("Day value is invalid: " + to_string(new_day));
        }

        year = new_year;
        month = new_month;
        day = new_day;
    }

    int GetYear() const {
        return year;
    }
    int GetMonth() const {
        return month;
    }
    int GetDay() const {
        return day;
    }

private:
    int year;
    int month;
    int day;
};

//Перезагрузка оператора сравнения для сравнения дат
bool operator<(const Date& lhs, const Date& rhs) {
    return vector<int>{lhs.GetYear(), lhs.GetMonth(), lhs.GetDay()} <
           vector<int>{rhs.GetYear(), rhs.GetMonth(), rhs.GetDay()};
}

//Перезагрузка оператора вывода для даты
ostream& operator<<(ostream& stream, const Date& date) {
    stream << setw(4) << setfill('0') << date.GetYear() <<
           "-" << setw(2) << setfill('0') << date.GetMonth() <<
           "-" << setw(2) << setfill('0') << date.GetDay();
    return stream;
}


//Класс для организации и управления простой БД
class Database {
public:
    //Добавление нового элемента в БД
    void AddEvent(const Date& date, const string& event) {
        storage[date].insert(event);
    }

    //Удаление только события из БД, если такое имеется
    //Возврат True при удалении
    bool DeleteEvent(const Date& date, const string& event) {
        if (storage.count(date) > 0 && storage[date].count(event) > 0) {
            storage[date].erase(event);
            return true;
        }
        return false;
    }

    //Удаление даты вместе со всеми её событиями
    //Возврат количества удаленных событий
    int DeleteDate(const Date& date) {
        if (storage.count(date) == 0) {
            return 0;
        } else {
            const int event_count = storage[date].size();
            storage.erase(date);
            return event_count;
        }
    }

    //Поиск событий по дате
    set<string> Find(const Date& date) const {
        if (storage.count(date) > 0) {
            return storage.at(date);
        } else {
            return {};
        }
    }

    //Печать всей БД
    void Print() const {
        for (const auto& item : storage) {
            for (const string& event : item.second) {
                cout << item.first << " " << event << endl;
            }
        }
    }

private:
    map<Date, set<string>> storage;
};

//Обработка даты из входного потока
//Вывод ошибки при некорректном вводе
Date ParseDate(const string& date) {
    istringstream date_stream(date);
    bool ok = true;

    int year;
    ok = ok && (date_stream >> year);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int month;
    ok = ok && (date_stream >> month);
    ok = ok && (date_stream.peek() == '-');
    date_stream.ignore(1);

    int day;
    ok = ok && (date_stream >> day);
    ok = ok && date_stream.eof();

    if (!ok) {
        throw logic_error("Wrong date format: " + date);
    }
    return Date(year, month, day);
}

int main() {
    Database db;

    string command_line;

    cout << "Please, enter a command: Add, Del, Find, Print, End" << endl <<
         "For example:" << endl << "Add 0000-01-01 Event" << endl <<
         "Del 0000-01-01 or Del 0000-01-01 Event" << endl <<
         "Find 0000-01-01" << endl;

    while (getline(cin, command_line)) {

        try {
            stringstream ss(command_line);

            string command;
            ss >> command;

            if (command == "Add") {

                string date_str, event;
                ss >> date_str >> event;
                const Date date = ParseDate(date_str);
                db.AddEvent(date, event);

            } else if (command == "Del") {

                string date_str;
                ss >> date_str;
                string event;
                if (!ss.eof()) {
                    ss >> event;
                }
                const Date date = ParseDate(date_str);
                if (event.empty()) {
                    const int count = db.DeleteDate(date);
                    cout << "Deleted " << count << " events" << endl;
                } else {
                    if (db.DeleteEvent(date, event)) {
                        cout << "Deleted successfully" << endl;
                    } else {
                        cout << "Event not found" << endl;
                    }
                }

            } else if (command == "Find") {

                string date_str;
                ss >> date_str;
                const Date date = ParseDate(date_str);
                for (const string &event: db.Find(date)) {
                    cout << event << endl;
                }

            } else if (command == "Print") {

                db.Print();

            } else if (command == "End") {

                cout << "Goodbye!";
                break;

            } else if (!command.empty()) {

                throw logic_error("Unknown command: " + command);

            }
        }
        catch (const exception &e) {
            cout << e.what() << endl;
        }
    }

    return 0;
}