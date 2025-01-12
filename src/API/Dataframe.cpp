
#include "Series.hpp"
#include "Dataframe.hpp"


Dataframe(){
    table = std::make_shared<Table>();
}


Dataframe(Dataframe t){
    table = t.getTable();
}

void Dataframe::appendCol(std::string name, Series input){
    QueryPlanner::getInstance().addNode(
        table,
        "appendCol",
        0,
        {name, input}
    );
}
void Dataframe::appendRow(std::vector<SeriesType> entry){
    std::vector<std::optional<SeriesType>> opt_entry;

    std::transform(
        input.begin(), 
        input.end(), 
        std::back_inserter(opt_entry),
        [](int value) { return std::optional<int>(value); }
    );

    QueryPlanner::getInstance().addNode(
        table,
        "appendRow",
        0,
        {opt_entry}
    );
}
void Dataframe::rename(std::string oldName, std::string newName){

    QueryPlanner::getInstance().addNode(
        table,
        "rename",
        0,
        {oldName, newName}
    );
}

Series Dataframe::column(std::string name){
    QueryPlanner::getInstance().addNode(
        table,
        "column",
        4,
        {name}
    );
}
std::vector<SeriesType> Dataframe::row(size_t idx){ //similar to iloc in pandas
    QueryPlanner::getInstance().addNode(
        table,
        "row",
        5,
        {idx}
    );
}
std::vector<std::vector<SeriesType>> Dataframe::rows(std::vector<size_t> indexes){
    QueryPlanner::getInstance().addNode(
        table,
        "rows_vector",
        3,
        {indexes}
    );
}
std::vector<std::vector<SeriesType>> Dataframe::rows(size_t start_idx, size_t end_idx){
    
    QueryPlanner::getInstance().addNode(
        table,
        "rows_idx",
        3,
        {start_idx, end_idx}
    );

}

void Dataframe::removeCol(std::string colName){
    QueryPlanner::getInstance().addNode(
        table,
        "removeCol",
        0,
        {colName}
    );
}
void Dataframe::removeRows(std::vector<size_t> rowNumbers){

    QueryPlanner::getInstance().addNode(
        table,
        "removeRows",
        0,
        {rowNumbers}
    );
}

template <typename Func> 
void Dataframe::filterRow(Func f, std::string columnName){
    
    QueryPlanner::getInstance().addNode(
        table,
        "filterRow",
        0,
        {f, columnName}
    );
}
void Dataframe::filterCol(std::vector<std::string> choosen, bool remove){
    
    QueryPlanner::getInstance().addNode(
        table,
        "filterCol",
        0,
        {choosen, remove}
    );

}
template <typename Func> 
void Dataframe::map(Func f, std::string name){
    
    QueryPlanner::getInstance().addNode(
        table,
        "map",
        0,
        {f, name}
    );

}

Dataframe Dataframe::innerJoin( Dataframe right, std::vector<std::string> columns) const{
    
    QueryPlanner::getInstance().addNode(
        table,
        "innerJoin",
        2,
        {right, columns}
    );
    
}
Dataframe Dataframe::outerJoin( Dataframe right, std::vector<std::string> columns ) const{
    
    QueryPlanner::getInstance().addNode(
        table,
        "outerJoin",
        2,
        {right, columns}
    );

}

Dataframe Dataframe::leftJoin( Dataframe right, std::vector<std::string> columns) const{
    QueryPlanner::getInstance().addNode(
        table,
        "leftJoin",
        2,
        {right, columns}
    );
}
Dataframe Dataframe::rightJoin( Dataframe right, std::vector<std::string> columns)const{

    QueryPlanner::getInstance().addNode(
        table,
        "rightJoin",
        2,
        {right, columns}
    );
}

template <typename Func> 
SeriesType Dataframe::aggregateColumn(const std::string& columnName, Func aggFunc, SeriesType initialValue) const{
    QueryPlanner::getInstance().addNode(
        table,
        "aggregateColumn",
        1,
        {columnName, aggFunc, initialValue}
    );
}

template <typename Func> 
Dataframe Dataframe::aggregateColumns(const std::vector<std::string>& columnNames, Func aggFunc, SeriesType initialValue) const {
    QueryPlanner::getInstance().addNode(
        table,
        "aggregateColumns",
        2,
        {columnNames, aggFunc, initialValue}
    );
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
    return Series((*table)[columnName]);
}


std::any collect() {
    FuncReturnType result = executePlan();
    switch (queryPlan.back().resultType) { // Dispatch based on last operation
        case Type::ROW_VECTOR: return std::get<std::vector<std::optional<SeriesType>>>(result);
        case Type::MATRIX: return std::get<std::vector<std::vector<std::optional<SeriesType>>>>(result);
        case Type::ARRAY: return std::get<Array*>(result);
        case Type::TABLE: return std::get<Table*>(result);
        case Type::SCALAR: return std::get<SeriesType>(result);
        case Type::VOID: return table = std::get<Table*>(result);
        default: throw std::runtime_error("Unsupported result type.");
    }
}