#include "puremethod.hh"
#include "tools.hh"

using namespace nolang;

void PureMethod::setParameters(std::vector<TypeIdent*> p)
{
    applyToVector<TypeIdent*>(m_params, p);
}
