//===-   FindFunctionCall.h - A frontend action outputting function calls -===//
//
//                     Cross Project Checking
//
// Author: Zhouyang Jia, PhD Candidate
// Affiliation: School of Computer Science, National University of Defense Technology
// Email: jiazhouyang@nudt.edu.cn
//
//===----------------------------------------------------------------------===//
//
// This file implements a frontend action to output function calls.
//
//===----------------------------------------------------------------------===//


#ifndef FindFunctionCall_h
#define FindFunctionCall_h

#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Option/OptTable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Signals.h"
#include "clang/Analysis/CFG.h"
#include "clang/Analysis/CallGraph.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/FrontendActions.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "clang/StaticAnalyzer/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CommonOptionsParser.h"

#include <string.h>
#include <map>

using namespace std;
using namespace clang::driver;
using namespace clang::tooling;
using namespace clang;


//===----------------------------------------------------------------------===//
//
// FindFunctionCallAction, FindFunctionCallConsumer and FindFunctionCallVisitor
//
//===----------------------------------------------------------------------===//
// These three classes work in tandem to prefrom the frontend analysis, if you
// have any question, please check the offical explanation:
//      http://clang.llvm.org/docs/RAVFrontendAction.html
//===----------------------------------------------------------------------===//

class FindFunctionCallVisitor : public RecursiveASTVisitor <FindFunctionCallVisitor> {
public:
    explicit FindFunctionCallVisitor(CompilerInstance* CI, StringRef InFile) : CI(CI), InFile(InFile){};

    // Visit the function declaration and travel the function body
    bool VisitFunctionDecl (FunctionDecl*);
    
    // Trave the statement and find function call
    void travelStmt(Stmt*);
    
private:
    CompilerInstance* CI;
    StringRef InFile;
};


class FindFunctionCallConsumer : public ASTConsumer {
public:
    explicit FindFunctionCallConsumer(CompilerInstance* CI, StringRef InFile) : Visitor(CI, InFile){}
    
    // Handle the translation unit and visit each function declaration
    virtual void HandleTranslationUnit (clang::ASTContext &Context);
    
private:
    FindFunctionCallVisitor Visitor;
    StringRef InFile;
};


class FindFunctionCallAction : public ASTFrontendAction {
public:
    // Creat FindFunctionCallConsuer instance and return to ActionFactory
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile);
    
private:
    // If the tool finds more than one entry in json file for a file, it just runs multiple times,
    // once per entry. As far as the tool is concerned, two compilations of the same file can be
    // entirely different due to differences in flags. However, we don't want these to ruin our
    // statistics. More detials see:
    // http://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools
    static map<string, bool> hasAnalyzed;
};

#endif /* FindFunctionCall_h */