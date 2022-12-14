#include <data_convert.hpp>
#include <gtest/gtest.h>
using namespace std;

BOOST_DEFINE_ENUM(Gender,Male,Female);

/* 麻蛋，教导主任 标识符在GCC7表现不对劲 */
struct JDZR{
    vector/*年级*/<set/*班*/<map/*某班学生姓名*/<string,Gender>>> nnnest;//  
    string name;
    float height;
    double dheight;
    int numOfHands;
    friend bool operator==(const JDZR&l,const JDZR&r){
        return l.nnnest==r.nnnest &&l.name==r.name&&l.height==r.height&&l.dheight==r.dheight&&l.numOfHands==r.numOfHands;
    }
};
BOOST_DESCRIBE_STRUCT(JDZR,(),(nnnest,name,height,dheight,numOfHands));

struct privateThings{
    double length=-1;
    bool operator==(privateThings const & r){return length==r.length;}
};
BOOST_DESCRIBE_STRUCT(privateThings,(),(length));

struct SJDZR:public JDZR,private privateThings{
    int id=-1;
    SJDZR(JDZR a={},privateThings b={},int c=0):JDZR(a),privateThings(b),id(c){};
    friend bool operator ==(const SJDZR&l,const SJDZR &r){
        return(*(JDZR*)&l)==(*(JDZR*)&r)&&(*(privateThings*)&l)==(*(privateThings*)&r)&&l.id==r.id;
    }
};
BOOST_DESCRIBE_STRUCT(SJDZR,(JDZR,privateThings),(id));
TEST(cbp2cmake,data_convert){
    
    using namespace CTC;
    using namespace cpp_interface::Data;

    JDZR mas={
        {},
        "mas0",
        175,
        175,
        2
    };
    constexpr bool is=CTC::Detail::is_described<JDZR>;
    static_assert(is,"");
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
    auto from=CTC::DataFrom<JDZR>(to);
    EXPECT_EQ(i,4);   
    EXPECT_EQ(j,4);
    EXPECT_EQ(mas,from);

    using T=SJDZR;
    using Bases=boost::describe::describe_bases<T,boost::describe::mod_public| boost::describe::mod_protected| boost::describe::mod_private>;
    constexpr int index0=boost::mp11::mp_find<Bases,JDZR>::type::value;
    SJDZR smas{mas,{18},0};
    auto sto=CTC::DataTo(smas);
    auto sfrom=CTC::DataFrom<SJDZR>(sto);
    EXPECT_EQ(smas,sfrom);
}


int main(){
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}