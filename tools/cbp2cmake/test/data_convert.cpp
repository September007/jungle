#include <data_convert.hpp>
#include <gtest/gtest.h>
using namespace std;

BOOST_DEFINE_ENUM(Gender,Male,Female);
struct 教导主任{
    vector/*年级*/<set/*班*/<map/*某班学生姓名*/<string,Gender>>> nnnest;//  
    string name;
    float height;
    double dheight;
    int numOfHands;
    friend bool operator==(const 教导主任&l,const 教导主任&r){
        return l.nnnest==r.nnnest &&l.name==r.name&&l.height==r.height&&l.dheight==r.dheight&&l.numOfHands==r.numOfHands;
    }
};
BOOST_DESCRIBE_STRUCT(教导主任,(),(nnnest,name,height,dheight,numOfHands));


TEST(cbp2cmake,data_convert){
    using namespace CTC;
    using namespace cpp_interface::Data;
    教导主任 mas={
        {},
        "mas0",
        175,
        175,
        2
    };
    set<map<string,Gender>> C1,C2;
    C1.insert(map<string,Gender>{{"C100",Male},{"C101",Female}});
    C1.insert(map<string,Gender>{{"C110",Female},{"C111",Female}});
    C1.insert(map<string,Gender>{{"C120",Female},{"C121",Male}});
    C1.insert(map<string,Gender>{{"C130",Male},{"C131",Male}});

    C2.insert(map<string,Gender>{{"C230",Male},{"C231",Male}});
    C2.insert(map<string,Gender>{{"C200",Male},{"C201",Female}});
    C2.insert(map<string,Gender>{{"C220",Female},{"C221",Male}});
    C2.insert(map<string,Gender>{{"C210",Female},{"C211",Female}});
    
    mas.nnnest.push_back(C1);
    mas.nnnest.push_back(C2);

    auto to=CTC::DataTo(mas);
    auto& nnnest= to.safe_access<TablePtr>()->at("nnnest");
    auto nnnestTable=nnnest.safe_access<TablePtr >();
    auto&v1=nnnestTable->at(0);
    auto &&v1t=v1.safe_access<std::shared_ptr<Table> >();
    int i=0;
    for(auto &p:*v1t){
        i++;
    }
    int j=0;
    auto it=v1t->begin(),end=v1t->end();
    while(it!=v1t->end())
    {
        j++;
        ++it;
    }
    auto from=CTC::DataFrom<教导主任>(to);
    EXPECT_EQ(i,4);   
    EXPECT_EQ(j,4);
    EXPECT_EQ(mas,from);
}


int main(){
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}