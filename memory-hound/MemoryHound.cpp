#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace clang;

class FindNamedClassVisitor
    : public RecursiveASTVisitor<FindNamedClassVisitor> {
public:
    explicit FindNamedClassVisitor(ASTContext *Context)
        : Context(Context) {}

	bool TraverseDecl(Decl *D)
	{
		std::string locString = D->getASTContext().getSourceManager().getFilename(D->getLocation()).str();
		if (locString == "")
		{
			std::cerr << "#### Before decl dump (location: " << locString << ") ####" << std::endl;
			D->dump();
			std::cerr << "#### After decl dump ####" << std::endl;
			RecursiveASTVisitor<FindNamedClassVisitor>::TraverseDecl(D);
		}
		return true;
	}

	bool TraverseStmt(Stmt *S)
	{
		std::cerr << "#### Before stmt dump ####" << std::endl;
		S->dump();
		std::cerr << "#### After stmt dump ####" << std::endl;

		RecursiveASTVisitor<FindNamedClassVisitor>::TraverseStmt(S);
		return true;
	}

	bool TraverseType(QualType T)
	{
		std::cerr << "#### Before type dump ####" << std::endl;
		T.dump();
		std::cerr << "#### After type dump ####" << std::endl;

		RecursiveASTVisitor<FindNamedClassVisitor>::TraverseType(T);
		return true;
	}
	
    //bool VisitCXXRecordDecl(CXXRecordDecl *Declaration) {
    //    if (Declaration->getQualifiedNameAsString() == "n::m::C") {
    //        FullSourceLoc FullLocation = Context->getFullLoc(Declaration->getLocStart());
    //        if (FullLocation.isValid())
    //            llvm::outs() << "Found declaration at "
    //                         << FullLocation.getSpellingLineNumber() << ":"
    //                         << FullLocation.getSpellingColumnNumber() << "\n";
    //    }
    //    return true;
    //}

private:
    ASTContext *Context;
};

class FindNamedClassConsumer : public clang::ASTConsumer {
public:
    explicit FindNamedClassConsumer(ASTContext *Context)
        : Visitor(Context) {}

    virtual void HandleTranslationUnit(clang::ASTContext &Context) {
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
private:
    FindNamedClassVisitor Visitor;
};

class FindNamedClassAction : public clang::ASTFrontendAction {
public:
    virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
        clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
        return std::unique_ptr<clang::ASTConsumer>(
                new FindNamedClassConsumer(&Compiler.getASTContext()));
    }
};

int main(int argc, char **argv) {
	assert (argc == 2);

	std::ifstream ifs(argv[1]);
	std::ostringstream oss;
	oss << ifs.rdbuf();

    if (argc > 1) {
        clang::tooling::runToolOnCode(new FindNamedClassAction, oss.str().c_str());
    }
}
