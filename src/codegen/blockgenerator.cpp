#include "blockgenerator.hh"
#include "trim.hh"

using nolang::BlockGenerator;

bool BlockGenerator::isEOS(std::string ll) const
{
    return ll == "<EOS>";
}

void BlockGenerator::handleEOS()
{
    resline = trim(resline);
    if (!resline.empty()) lines.push_back(resline + ";\n");
    resline = "";
}

void BlockGenerator::handleBlock(std::vector<std::string> block)
{
    if (block.empty()) return;
    for (auto ll : block) {
        if (isEOS(ll)) handleEOS();
        else resline += ll;
    }
}

void BlockGenerator::reset()
{
    lines.clear();
    resline = "";
}

std::vector<std::string> BlockGenerator::generate()
{
    reset();
    for (auto line : block) handleBlock(cgen->generateStatements(line, method));
    return lines;
}
