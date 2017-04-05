//
// Created by Yunming Zhang on 2/14/17.
//

#include <gtest.h>
#include <graphit/frontend/frontend.h>
#include <graphit/midend/mir_context.h>
#include <graphit/midend/midend.h>
#include <graphit/backend/backend.h>
#include <graphit/frontend/error.h>

using namespace std;
using namespace graphit;

//Frontend * fe = new Frontend();

//tests back end
TEST(CodeGenTest, SimpleVarDecl) {
    Frontend * fe = new Frontend();
    istringstream is("const a : int = 3 + 4;");
    graphit::FIRContext* fir_context = new graphit::FIRContext();
    std::vector<ParseError> * errors = new std::vector<ParseError>();
    fe->parseStream(is, fir_context, errors);
    graphit::MIRContext* mir_context  = new graphit::MIRContext();
    graphit::Midend* me = new graphit::Midend(fir_context);
    me->emitMIR(mir_context);
    graphit::Backend* be = new graphit::Backend(mir_context);
    std::cout << "generated c++: " << std::endl;
    EXPECT_EQ (0 ,  be->emitCPP());
}


TEST(CodeGenTest, SimpleFunctionDecl) {
    Frontend * fe = new Frontend();
    istringstream is("func add(a : int, b: int) -> c : int  end");
    graphit::FIRContext* fir_context = new graphit::FIRContext();
    std::vector<ParseError> * errors = new std::vector<ParseError>();
    fe->parseStream(is, fir_context, errors);
    graphit::MIRContext* mir_context  = new graphit::MIRContext();
    graphit::Midend* me = new graphit::Midend(fir_context);
    me->emitMIR(mir_context);
    graphit::Backend* be = new graphit::Backend(mir_context);
    std::cout << "generated c++: " << std::endl;
    EXPECT_EQ (0 ,  be->emitCPP());
}