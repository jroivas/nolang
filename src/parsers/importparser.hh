#pragma once

#include <3pp/mpc/mpc.h>
#include "baseparser.hh"
#include "import.hh"

namespace nolang {

class ImportParser : public BaseParser<Import>
{
public:
    ImportParser(mpc_ast_t *);

private:
    bool isImport() const;
    bool isImportString() const;
    bool isAs() const { return expect(item, "string", "as"); }
    bool isAsDef() const { return gotAs && isIdentifier(); }
    bool isSub() const { return isNamespaceDef(); }

    void reset();
    virtual void parseItem();
    void createImport();
    void addAs();
    void addSub();
    void parseSub();
    void parseSubIdentifier();
    void parseAs();

    bool gotAs;
};

}
