#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_set>
#include <thread>
#include <string>

//проверяем, что максимальный размер строки - 7 символов 
bool isValidLength(const std::string& s) {
    return s.size() <= 7;
}

//проверяем, есть ли буква в строке
bool hasAlpha(const std::string& s) {
    for (char c : s)
        if (isalpha(c)) return true;

    return false;
}

//проверяем, состоит ли строка только из цифр
bool isAllDigits(const std::string& s) {
    for (char c : s)
        if (!isdigit(c)) return false;

    return !s.empty();
}

//проверяем, что строка состоит из валидного кода и номера
bool isValidFormat(const std::string& str) {

    //проверяем, что строка состоит из допустимых символов (латиница, цифры, пробел)
    for (char simv : str) {
        if (!(isalpha(simv)) && !(isdigit(simv)) && simv != ' ')
            return false;
    }

    //случай, если строка содержит пробел
    size_t space_pos = str.find(' ');
    if (space_pos != std::string::npos) {
        //если пробелов больше 1 то строка сразу не подходит
        if (str.find(' ', space_pos + 1) != std::string::npos) return false;

        //делим по пробелу на код и номер
        std::string code = str.substr(0, space_pos);
        std::string number = str.substr(space_pos + 1);

        //подойдет лишь строка, чей код ровно 2 символа в длину и содержит хотя бы 1 букву, а номер состоит из 1-5 цифр
        return code.size() == 2 &&
            hasAlpha(code) &&
            number.size() >= 1 &&
            number.size() <= 5 &&
            isAllDigits(number);
    }
    //теперь рассмотрим случаи, когда строка не содержит пробел

    //случай, когда код отсутствует и есть лишь номер, состоящий только из цифр:
    if (isAllDigits(str)) {
        return str.size() <= 5;
    }

    //теперь будем искать строки, подходящие под другие правила (отбросим те строки, длина которых меньше 3 символов)
    if (str.size() < 3)
        return false;

    //у оставшихся строк возьмем первые 3 символа и будем их проверять
    std::string first3 = str.substr(0, 3);
    std::string other = str.substr(3);

    //0 букв мы уже рассмотрели

    //рассматриваем случаи, когда в first3 лишь 1 буква

    if ((isalpha(first3[0]) && isdigit(first3[1]) && isdigit(first3[2])) //код подходит - буква стоит лишь на 1 позиции
        || (isdigit(first3[0]) && isalpha(first3[1]) && isdigit(first3[2]))) { //код подходит - буква стоит лишь на 2 позиции 
        if (other.empty() || isAllDigits(other)) //номер содержится в first3 и может содержаться в other
            return true;
    }

    if (isdigit(first3[0]) && isdigit(first3[1]) && isalpha(first3[2])) ///если буква стоит лишь на 3 позиции, код не подходит 
        return false;

    //рассматриваем случаи, когда в first3 лишь 2 буквы

    if ((isdigit(first3[0]) && isalpha(first3[1]) && isalpha(first3[2])) // когда буква на 2 и 3 позиции, код не подходит
        || (isalpha(first3[0]) && isdigit(first3[1]) && isalpha(first3[2]))) // когда буква на 1 и 3 позиции, код не подходит
        return false;

    if (isalpha(first3[0]) && isalpha(first3[1]) && isdigit(first3[2])) { //когда буква на 1 и 2 позиции, код подходит
        if (other.empty() || isAllDigits(other)) //теперь проверяем номер
            return true;
    }

    if (isalpha(first3[0]) && isalpha(first3[1]) && isalpha(first3[2])) { // если все буквы, код подходит
        if (isAllDigits(other)) //теперь проверяем номер
            return true;
    }

    return false; //я разобрал все удовлетворяющие случаи. если не подошло, то строка некорректна

}

//убираем незначащие нули из номера
std::string deleteZeros(const std::string& str) {

    size_t firstNotZero = str.find_first_not_of('0');
    if (firstNotZero == std::string::npos)
        return "0"; //случай если весь номер состоит из нулей

    return str.substr(firstNotZero);
}

//форматируем строку (убираем пробелы и незначащий ноль)
std::string formateString(const std::string& str) {

    size_t space_pos = str.find(' ');
    if (space_pos != std::string::npos) {

        //делим по пробелу на код и номер
        std::string code = str.substr(0, space_pos);
        std::string number = str.substr(space_pos + 1);

        deleteZeros(number);
        return (code + number);
    }

    if (isAllDigits(str))
        return deleteZeros(str);

    if (isalpha(str[0]) && isalpha(str[1]) && isalpha(str[2])) {
        std::string code = str.substr(0, 3);
        std::string number = str.substr(3);
        return (code + deleteZeros(number));
    }

    std::string code = str.substr(0, 2);
    std::string number = str.substr(2);

    return (code + deleteZeros(number));
}

//обработка файла
void processFile(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream fin(inputFile);
    if (!fin.is_open()) {
        std::cerr << "Ошибка открытия файла: " << inputFile << std::endl;
        return;
    }

    std::unordered_set<std::string> uniqueEntries;
    std::string line;

    while (getline(fin, line)) {
        if (isValidFormat(line)) {
            uniqueEntries.insert(formateString(line));
        }
    }
    fin.close();

    std::ofstream fout(outputFile);
    if (!fout.is_open()) {
        std::cerr << "Ошибка создания файла: " << outputFile << std::endl;
        return;
    }

    for (const auto& entry : uniqueEntries) {
        fout << entry << std::endl;
    }
    fout.close();
}

int main() {
    setlocale(LC_ALL, "Russian");

    //запуск обработки файлов в параллельных потоках
    std::thread t1(processFile, "1_in.txt", "1_out.txt");
    std::thread t2(processFile, "2_in.txt", "2_out.txt");

    //ожидание завершения потоков
    t1.join();
    t2.join();

    std::cout << "Окончание программы" << std::endl;
    return 0;
}