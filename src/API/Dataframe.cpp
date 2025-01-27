
#include "Series.hpp"
#include "Dataframe.hpp"


Dataframe::Dataframe(){
    table = std::make_shared<Table>();
}


Dataframe::Dataframe(Dataframe& t){
    table = t.getTable();
}

Dataframe::Dataframe(Table t){
    table = std::make_shared<Table>(t);
}

void Dataframe::appendCol(std::string name, Series input){
   table->appendCol(name, input.getArray());
}

void Dataframe::rename(std::string oldName, std::string newName){

    table->rename(oldName, newName);
}

Series Dataframe::column(std::string name){
   return Series((*table)[name]);
}

void Dataframe::removeCol(std::string colName){
    table->removeCol(colName);
}
void Dataframe::removeRows(std::vector<size_t> rowNumbers){
    table->removeRows(rowNumbers);
}

void Dataframe::filterRow(std::function<bool(const int&)> f, std::string columnName){
    table->filterRow(f, columnName);
}
void Dataframe::filterRow(std::function<bool(const std::string&)> f, std::string columnName){
    table->filterRow(f, columnName);
}
void Dataframe::filterRow(std::function<bool(const float&)> f, std::string columnName){
    table->filterRow(f, columnName);
}
void Dataframe::filterCol(std::set<std::string> choosen, bool remove= false){
    table->filterCol(choosen, remove);
}

void Dataframe::map(std::function<void(int&)> f, std::string name){
    table->map(f,name);
}
void Dataframe::map(std::function<void(std::string&)> f, std::string name){
    table->map(f,name);
}
void Dataframe::map(std::function<void(float&)> f, std::string name){
    table->map(f,name);
}


int Dataframe::aggregateColumn( std::string& columnName, std::function<int(int, int)> aggFunc, int initialValue){
    return table->aggregateColumn(columnName, aggFunc, initialValue);
}
std::string Dataframe::aggregateColumn( std::string& columnName, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue){
    return table->aggregateColumn(columnName, aggFunc, initialValue);
}
float Dataframe::aggregateColumn( std::string& columnName, std::function<float(float, float)> aggFunc, float initialValue){
    return table->aggregateColumn(columnName, aggFunc, initialValue);
}

Table Dataframe::aggregateColumn( std::vector<std::string>& columnNames, std::function<int(int, int)> aggFunc, int initialValue){
    return table->aggregateColumn(columnNames, aggFunc, initialValue);
}
Table Dataframe::aggregateColumn( std::vector<std::string>& columnNames, std::function<std::string(std::string, std::string)> aggFunc, std::string initialValue){
    return table->aggregateColumn(columnNames, aggFunc, initialValue);
}
Table Dataframe::aggregateColumn( std::vector<std::string>& columnNames, std::function<float(float, float)> aggFunc, float initialValue){
    return table->aggregateColumn(columnNames, aggFunc, initialValue);
}

bool Dataframe::operator==(Dataframe& d){
    return table == d.getTable();
}
bool Dataframe::operator!=(Dataframe& d){
    return !(table == d.getTable());
}

Dataframe Dataframe::innerJoin(Dataframe left, Dataframe right, std::set<std::string> columns){

    return Dataframe(Table::innerJoin(*(left.getTable()), *(right.getTable()), columns));
}
Dataframe Dataframe::outerJoin(Dataframe left, Dataframe right, std::set<std::string> columns){
    return Dataframe(Table::outerJoin(*(left.getTable()), *(right.getTable()), columns));
}
Dataframe Dataframe::leftJoin(Dataframe left, Dataframe right, std::set<std::string> columns){
    return Dataframe(Table::leftJoin(*(left.getTable()), *(right.getTable()), columns));
}
Dataframe Dataframe::rightJoin(Dataframe left, Dataframe right, std::set<std::string> columns){
    return Dataframe(Table::rightJoin(*(left.getTable()), *(right.getTable()), columns));
}

std::shared_ptr<Table> Dataframe::getTable(){
    return table;
}

size_t Dataframe::getNumColumn(){
    return table->getNumColumn();
}
size_t Dataframe::getNumRow(){
    return table->getNumRow();
}

Series& Dataframe::operator[](const std::string& columnName) {
    Series tmp = Series((*table)[columnName]);
    Series& tm = tmp;
    return tm;
}


// std::any collect() {
//     FuncReturnType result = executePlan();
//     switch (queryPlan.back().resultType) { // Dispatch based on last operation
//         case Type::ROW_VECTOR: return std::get<std::vector<std::optional<SeriesType>>>(result);
//         case Type::MATRIX: return std::get<std::vector<std::vector<std::optional<SeriesType>>>>(result);
//         case Type::ARRAY: return std::get<Array*>(result);
//         case Type::TABLE: return std::get<Table*>(result);
//         case Type::SCALAR: return std::get<SeriesType>(result);
//         case Type::VOID: return table = std::get<Table*>(result);
//         default: throw std::runtime_error("Unsupported result type.");
//     }
// }