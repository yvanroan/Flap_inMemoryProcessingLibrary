#include <iostream>
#include <vector>
#include <string>
#include "data/Table.hpp"
#include "data/ArrayInt.hpp"
#include "data/ArrayString.hpp"
#include "data/ArrayFloat.hpp"
#include <limits>

// TODO: 1. use all methods in the class except joins, focus on single table interactions

int main () {


    Table t1 = Table();

    if(0 != t1.getNumRow() || 0 != t1.getNumColumn() ){
        
        printf("size of empty table is not empty");
        printf("rows: %d, column: %d",t1.getNumRow(), t1.getNumColumn() );
        return 0;
    }

    std::vector<int> veci;
    for(int i = 0; i<3; i++){
        veci.emplace_back(i);
    }

    std::vector<std::string> vecs;
    std::string s = "";
    for(int i = 0; i<3; i++){
        s+="a";
        vecs.emplace_back(s);
    }

    std::vector<float> vecf;
    for(float i = 0.5; i<3; i++){
        vecf.emplace_back(i);
    }
    
    std::shared_ptr<ArrayInt> arr1 = std::make_shared<ArrayInt>(veci);
    std::shared_ptr<ArrayString> arr2 = std::make_shared<ArrayString>(vecs);
    std::shared_ptr<ArrayFloat> arr3 = std::make_shared<ArrayFloat>(vecf);

    std::unordered_map<std::string, std::shared_ptr<Array>> map;
    map["col1"] = arr1;
    map["col2"] = arr2;

    Table t2 = Table(map);
        
    if(3 != t2.getNumRow() || 2 != t2.getNumColumn() ){
        
        printf("size of table is deosn't match the elements in it\n");
        std::cout << t2<<std::endl;
        std::cout <<"rows, col :" << t2.getNumRow() << t2.getNumColumn() << std::endl;
        return 0;
    }

    t1 = Table(t2);
    if( t1 != t2){
        
        printf("size of tables do not match 1 \n");
        printf("t1\n");
        std::cout << t1;
        printf("\nt2\n");
        std::cout << t2;
        return 0;
    }


    t1.appendCol("col3", arr3);
    t2.appendCol("col3", arr3);

    if( t1 != t2 || 3 != t2.getNumColumn()){
        
        printf("size of tables do not match \n");
        printf("t1\n");
        std::cout << t1;
        printf("\nt2\n");
        std::cout << t2;
        return 0;
    }

    std::vector<std::pair< std::string, std::any>> rows1; // in order, should work
    
    rows1.emplace_back(std::make_pair("col3", float(3.45)));
    rows1.emplace_back(std::make_pair("col1", 5));
    rows1.emplace_back(std::make_pair("col2", std::string("apple")));

    std::vector<std::pair< std::string, std::any>> rows2;//not in order, wont work
    rows2.emplace_back(std::make_pair("col3", float(3.45)));
    rows2.emplace_back(std::make_pair("col2", 5));
    rows2.emplace_back(std::make_pair("col1", std::string("apple")));

    
    t2.appendRow(rows1);
//issue here
    try {
        t1.appendRow(rows2);
    } catch(const std::exception& e){
        // Handle the exception
        std::cout << "Expected exception from Test: " << e.what() <<", it is expected" << std::endl;
    }


    if(4 != t2.getNumRow()){
        
        printf("We added rows but this doesnt seem to work\n");
        std::cout << t2;
        return 0;
    }

    t2.rename("col3", "floats");

    try {
        t1.rename("col", "floats");
    } catch(const std::exception& e){
        // Handle the exception
        std::cout << "Expected exception from Test: " << e.what() << std::endl;
    }

    if(t2.getTable().find("col3") != t2.getTable().end() || t2.getTable().find("floats") == t2.getTable().end() ){
        printf("the rename operation did not work\n");
        std::cout << t2;
        return 0;
    }
    size_t chosen_row = 2;


    rows2 = t1.row(chosen_row);

    try {
        t1.row(5);
    } catch(const std::exception& e){
        // Handle the exception
        std::cout << "Expected exception from Test: " << e.what() <<", this is expected" << std::endl;
    }


    // //testing getTable
    for(auto& [name, col]: t1.getTable()){
        if(t1[name] != col){
            printf("the arrays aren't equal\n");
            std::cout << t1[name] << "\n";
            std::cout << col << "\n";
        }
    }

    for(auto& [name_t, col]: t1.getTable()){
        for(auto& [name_c, cur]: rows2){
            if(name_t != name_c){
                continue;
            }
            if (cur.type() == typeid(int) and "int" != col->typedef_()) {
                auto intcol = std::dynamic_pointer_cast<ArrayInt>(col);
                if(std::any_cast<int>(cur) != (*intcol)[chosen_row]){
                    printf("row method failed");
                    std::cout << col << " . We need row" << chosen_row<< "\n";
                    std::cout << std::any_cast<int>(cur) << " this is what the row had.\n";
                }
            }
            else if (cur.type() == typeid(const char*) and "string" != col->typedef_()) {
                auto stringcol = std::dynamic_pointer_cast<ArrayString>(col);
                if(std::string(std::any_cast<const char*>(cur)) != (*stringcol)[chosen_row]){
                    printf("row method failed");
                    std::cout << col << " . We need row" << chosen_row<< "\n";
                    std::cout << std::any_cast<float>(cur) << " this is what the row had.\n";
                }
            }
            else if (cur.type() == typeid(double) and "float" != col->typedef_()) {
                auto floatcol = std::dynamic_pointer_cast<ArrayFloat>(col);
                if(float(std::any_cast<double>(cur)) != (*floatcol)[chosen_row]){
                    printf("row method failed");
                    std::cout << col << " . We need row" << chosen_row<< "\n";
                    std::cout << std::any_cast<float>(cur) << " this is what the row had.\n";
                }
            }
        }
    }

    std::vector<size_t> indexes = {0,2};
    
    std::vector<std::vector<std::pair< std::string, std::any>>> rows3 = t2.rows(indexes);// 0 and 2
    std::vector<std::vector<std::pair< std::string, std::any>>> rows4 = t2.rows(0,2); // 0 to 2


    if(rows3.size()!= 2 || rows4.size() != 3){
        printf("rows didn't work, row\n");
        std::cout << t2;
        return 0;
    }

    int i;
    //since rows is based on row directly, rows should work. will implement later.
        
    int col = t1.getNumColumn() ;
    t1.removeCol("col2");

    if(t1.getNumColumn() != col-1 || t1.getTable().size() != col-1){
        printf("Remove Column didn't work, col");
        std::cout << t1<< std::endl;
        return 0;
    }

    std::vector<size_t> idx = {0};
    int row = t1.getNumRow() ;
    t1.removeRows(idx);


    if(t1.getNumRow() != row - idx.size() || (*(t1.getTable().begin()->second)).getSize() != row - idx.size()){
        printf("Remove row didn't work, the array length: %ld\n", (*(t1.getTable().begin()->second)).getSize());
        std::cout << t1 << std::endl;
        return 0;

    }

    std::set<std::string> cols = {"floats"};

    col = t2.getNumColumn();
    t2.filterCol(cols, true);

    if(t2.getNumColumn() != col - cols.size()){
        printf("Filter column didn't work: \n");
        std::cout << t2 << std::endl;
        printf("prev colNumbers, cur colNumbers, expected colNumber: %d, %d, %d", col,t2.getNumColumn() ,  col - cols.size());
        return 0;
    }

    std::function<bool(const int&)> f = [](const int& val){
        return val >= 2;
    };

    row = t2.getNumRow();
    int numRowLeft = std::dynamic_pointer_cast<ArrayInt>(t2["col1"])->filteredIndex(f).size();
    t2.filterRow(f, "col1");
    
    
    if(t2.getNumRow() !=  numRowLeft){
        printf("Filter row didn't work, num of row removed: %d\n", numRowLeft);
        std::cout << t2 << std::endl;
        return 0;
    }

    std::function<void(std::string&)> map_f = [](std::string& val){
        val += "bb";
    };

    t2.map(map_f, "col2");
    //if array map works, this works. and we know array map works

    const std::vector<std::string> cols3 =  {"col1", "col2"};
    std::string sa = "col1";

    std::function<int(int, int)> sum = [](int a, int b){
        return a+b;
    };

    int sum_t2=0;

    for(auto val: std::dynamic_pointer_cast<ArrayInt>(t2[sa])->getArray()){
        sum_t2 += val;
    }

    int sum_agg = t2.aggregateColumn(sa, sum ,0);

    if(sum_t2 != sum_agg){
        printf("Column aggregation didn't work");
        std::cout << t2[sa] << std::endl;
        printf("expected, cur: %d, %d", sum_t2, sum_agg);
        return 0;
    }
   
    std::set<std::string> cols4 =  {"col1"};

    
    Table expected_t3;
    std::shared_ptr<ArrayInt> t3_col1 = std::make_shared<ArrayInt>();
    std::shared_ptr<ArrayString> t3_col2 = std::make_shared<ArrayString>();
    std::shared_ptr<ArrayFloat> t3_col3 = std::make_shared<ArrayFloat>();

    t3_col1->append(2);
    t3_col1->append(5);
    t3_col2->append(std::string("aaabb"));
    t3_col2->append(std::string("applebb"));
    t3_col3->append(float(2.5));
    t3_col3->append(std::numeric_limits<float>::infinity());

   
    expected_t3["col1"] = t3_col1;
    expected_t3["col2"] = t3_col2;
    expected_t3["col3"] = t3_col3;


    Table t3 = Table::leftJoin(t2, t1, cols4);

    if(t3 != expected_t3){
        printf("Left join isn't working");
        std::cout <<"t3\n" << t3 << std::endl;
        std::cout <<"expected_t3\n" << expected_t3 << std::endl;
        return 0;
    }
    
    
    Table t6 = Table::innerJoin(t2, t1, cols4);
   
   
    Table expected_t6;
    std::shared_ptr<ArrayInt> t6_col1 = std::make_shared<ArrayInt>();
    std::shared_ptr<ArrayString> t6_col2 = std::make_shared<ArrayString>();
    std::shared_ptr<ArrayFloat> t6_col3 = std::make_shared<ArrayFloat>();

    t6_col1->append(2);
    t6_col2->append("aaabb");
    t6_col3->append(2.5);

    expected_t6["col1"] = t6_col1;
    expected_t6["col2"] = t6_col2;
    expected_t6["col3"] = t6_col3;


    if(t6 != expected_t6){
        printf("Left join isn't working");
        std::cout <<"t6\n" << t6 << std::endl;
        std::cout <<"expected_t6\n" << expected_t6 << std::endl;
        return 0;
    }

    Table t5 = Table::outerJoin(t2, t1, cols4);

    Table expected_t5;
    std::shared_ptr<ArrayInt> t5_col1 = std::make_shared<ArrayInt>();
    std::shared_ptr<ArrayString> t5_col2 = std::make_shared<ArrayString>();
    std::shared_ptr<ArrayFloat> t5_col3 = std::make_shared<ArrayFloat>();

    t5_col1->append(2);
    t5_col2->append("aaabb");
    t5_col3->append(2.5);

    t5_col1->append(5);
    t5_col2->append(std::string("applebb"));
    t5_col3->append(std::numeric_limits<float>::infinity());

    t5_col1->append(1);
    t5_col2->append("null");
    t5_col3->append(1.5);

    expected_t5["col1"] = t5_col1;
    expected_t5["col2"] = t5_col2;
    expected_t5["col3"] = t5_col3;

    if(t5 != expected_t5){
        printf("Outer join isn't working");
        std::cout <<"t5\n" << t5 << std::endl;
        std::cout <<"expected_t5\n" << expected_t5 << std::endl;
        return 0;
    }

}