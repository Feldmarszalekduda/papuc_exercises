#include "ShuntingYard.hpp"
#include "ASTNode.hpp"

namespace calc
{
ASTBuilder::ASTBuilder(common::Log& logInit, TokenStream& tsInit)
    : log{logInit}, ts{tsInit}
{
}

ASTBuilder::ASTBuilder(common::Log& logInit, TokenStream&& tsInit)
    : log{logInit}, tsVal{std::make_unique<TokenStream>(std::move(tsInit))},
      ts{*tsVal}
{
}

std::unique_ptr<ASTNode> ASTBuilder::getAST()
{
    bool binaryOpExpectedNext = false;

    while (ts.hasNext())
    {
        auto token = ts.get();
        log.debug() << "processing token: " << token;

        switch (token.typeId)
        {
        case '8':
        case 'S':
        {
            outStack.push(std::make_unique<ASTNode>(token));
            binaryOpExpectedNext = true;
            break;
        }
        case '=':
        {
            opStack.push(token);
            binaryOpExpectedNext = false;
            break;
        }
        case '+':
        case '-':
        {
            if (binaryOpExpectedNext)
            {
                while (not opStack.empty() and
                       hasLowerPrecedenceThanStackTop(token))
                {
                    if (isBinaryOp(opStack.top()))
                    {
                        processBinaryPop(opStack.top());
                    }
                    else if (isUnaryOp(opStack.top()))
                    {
                        processUnaryPop(opStack.top());
                    }
                    opStack.pop();
                }

                opStack.push(token);
                binaryOpExpectedNext = false;
            }
            else
            {
                if (token.typeId == '-')
                {
                    opStack.push(Token{'#'});
                }
            }
            break;
        }
        case '*':
        case '/':
        {
            while (not opStack.empty() and
                   hasLowerPrecedenceThanStackTop(token))
            {
                processOp(opStack.top());
                opStack.pop();
            }

            opStack.push(token);
            binaryOpExpectedNext = false;
            break;
        }
        case '(':
        {
            opStack.push(token);
            binaryOpExpectedNext = false;
            break;
        }
        case ')':
        {
            while (not opStack.empty() and opStack.top().typeId != '(')
            {
                processOp(opStack.top());
                opStack.pop();
            }

            if (opStack.empty() or opStack.top().typeId != '(')
            {
                log.error() << "no matching paren found on stack";
            }
            else
            {
                opStack.pop();
            }
            binaryOpExpectedNext = true;
            break;
        }
        default:
        {
            log.error() << "unhandled token type";
        }
        }
    }

    while (not opStack.empty())
    {
        processOp(opStack.top());
        opStack.pop();
    }

    if (outStack.size() == 1u)
    {
        return std::move(outStack.top());
    }
    else
    {
        log.error() << "algorithm finished with more than one element on the "
                       "output stack";
        return nullptr;
    }
}

void ASTBuilder::processOp(const Token& opTok)
{
    if (isBinaryOp(opTok))
    {
        processBinaryPop(opTok);
    }
    else if (isUnaryOp(opTok))
    {
        processUnaryPop(opTok);
    }
    else
    {
        throw std::logic_error{"unclassified token encountered"};
    }
}

bool ASTBuilder::hasLowerPrecedenceThanStackTop(const Token& opTok)
{
    auto incomming = opTok.typeId;
    auto stackTop = opStack.top().typeId;
    if (incomming == '+' or incomming == '-')
    {
        if (stackTop == '*' or stackTop == '/' or stackTop == '#')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else if (incomming == '*' or incomming == '/')
    {
        if (stackTop == '#')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool ASTBuilder::isBinaryOp(const Token& opTok)
{
    switch (opTok.typeId)
    {
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
    case '=':
    {
        return true;
    }
    default:
    {
        return false;
    }
    }
}

void ASTBuilder::processBinaryPop(const Token& opTok)
{
    if (not isBinaryOp(opTok.typeId))
    {
        log.error() << "processing non binary " << opTok.typeId
                    << " as binary operator!";
    }

    if (outStack.size() < 2u)
    {
        log.error() << lispyTreePrint(outStack.top().get());
        throw std::logic_error("malformed expression");
    }
    else
    {
        auto rightArg = std::move(outStack.top());
        outStack.pop();
        auto leftArg = std::move(outStack.top());
        outStack.pop();

        outStack.push(std::make_unique<ASTNode>(opTok, std::move(leftArg),
                                                std::move(rightArg)));
    }
}

bool ASTBuilder::isUnaryOp(const Token& opTok)
{
    switch (opTok.typeId)
    {
    case '#':
    {
        return true;
    }
    default:
    {
        return false;
    }
    }
}

void ASTBuilder::processUnaryPop(const Token& opTok)
{
    if (not isUnaryOp(opTok.typeId))
    {
        log.error() << "processing non unary " << opTok.typeId
                    << " as unary operator!";
    }

    if (outStack.size() < 1u)
    {
        throw std::logic_error("malformed expression");
    }
    else
    {
        auto arg = std::move(outStack.top());
        outStack.pop();
        outStack.push(
            std::make_unique<ASTNode>(opTok, nullptr, std::move(arg)));
    }
}
}
