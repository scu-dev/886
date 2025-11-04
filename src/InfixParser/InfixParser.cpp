#include <cmath>
#include <string>
#include <vector>

#include "InfixParser.hpp"

namespace InfixParser {
    typedef uint64_t u64;
    using std::string, std::vector, std::reverse, std::to_string, std::pow;

    static string errorMessage;
    static u64 cursor{0};
    [[nodiscard]] string getErrorMessage() noexcept { return errorMessage; }

    [[nodiscard]] static bool getLiteral(vector<char>& input, double& result) noexcept {
        result = 0.0;
        bool reachedNumber = false, reachedDecimal = false;
        u64 decimalPlaces = 0;
        while (true) {
            char nextToken = input.back();
            switch (nextToken) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                    reachedNumber = true;
                    if (reachedDecimal) {
                        result += static_cast<double>(nextToken - '0') / pow(10, decimalPlaces);
                        decimalPlaces++;
                    }
                    else {
                        result *= 10;
                        result += static_cast<double>(nextToken - '0');
                    }
                    input.pop_back();
                    cursor++;
                    break;
                }
                case '.': {
                    if (reachedDecimal || !reachedNumber) {
                        errorMessage = string("Unexpected token '") + nextToken + "' in literal at pos " + to_string(cursor) + ".";
                        return false;
                    }
                    else {
                        reachedDecimal = true;
                        decimalPlaces++;
                        input.pop_back();
                        cursor++;
                        break;
                    }
                }
                default: {
                    if (!reachedNumber) {
                        errorMessage = string("Unexpected token '") + nextToken + "' in literal at pos " + to_string(cursor) + ".";
                        return false;
                    }
                    if(reachedDecimal && decimalPlaces == 1) {
                        errorMessage = string("Unexpected token '") + nextToken + "' in literal at pos " + to_string(cursor) + ".";
                        return false;
                    }
                    else return true;
                }
            }
        }
    }

    [[nodiscard]] static bool parse(vector<char>& input, Expression& result, u64 minRBindingPower) noexcept {
        //Left hand side literal or unary operator
        result.type = Expression::Type::Literal;
        switch (input.back()) {
            case '(': {
                input.pop_back();
                cursor++;
                if (!parse(input, result, 0)) return false;
                else if (input.back() != ')') {
                    errorMessage = string("Expected ')' at pos ") + to_string(cursor) + ".";
                    return false;
                }
                else {
                    input.pop_back();
                    cursor++;
                    break;
                }
            }
            case '-': {
                input.pop_back();
                cursor++;
                Expression operand{};
                if (!parse(input, operand, getBindPower(Operator::UnaryMinus, true))) return false;
                result = Expression(Expression(0), operand, Operator::Subtract);
                break;
            }
            case '+': {
                input.pop_back();
                cursor++;
                if (!parse(input, result, getBindPower(Operator::UnaryMinus, true))) return false;
                break;
            }
            default: if(!getLiteral(input, result.literal)) return false;
        }
        while (true) {
            //Operator
            Operator op{};
            switch (input.back()) {
                case '+': {
                    op = Operator::Add;
                    break;
                }
                case '-': {
                    op = Operator::Subtract;
                    break;
                }
                case '*': {
                    op = Operator::Multiply;
                    break;
                }
                case '/': {
                    op = Operator::Divide;
                    break;
                }
                case '^': {
                    op = Operator::Power;
                    break;
                }
                case ')':
                    [[fallthrough]];
                case '\0':
                    return true;
                default: {
                    errorMessage = string("Unexpected token '") + input.back() + "' at pos " + to_string(cursor) + ".";
                    return false;
                }
            }
            u64 lBindingPower = getBindPower(op, false), rBindingPower = getBindPower(op, true);
            if (lBindingPower < minRBindingPower) return true;
            input.pop_back();
            cursor++;
            //Right hand side expression
            Expression exprR{};
            if (!parse(input, exprR, rBindingPower)) return false;
            result = Expression(result, exprR, op);
        }
        return true;
    }

    [[nodiscard]] bool parseInfixExpression(const string& expr, Expression& result) noexcept {
        cursor = 0;
        vector<char> trimmed;
        for (u64 i = 0; i < expr.size(); i++) {
            //I don't trust modern compilers will optimize this out, compared to the for loop size() above
            auto temp = expr[i];
            if (temp != ' ' && temp != '\t' && temp != '\n' && temp != '\r') trimmed.push_back(temp);
        }
        if (trimmed.size() == 0) {
            errorMessage = "Expression is empty.";
            return false;
        };
        //EOF
        trimmed.push_back('\0');
        reverse(trimmed.begin(), trimmed.end());
        return parse(trimmed, result, 0);
    }

    [[nodiscard]] static bool calculate(const Expression& expr, double& result) noexcept {
        switch (expr.type) {
            case Expression::Type::Literal: {
                result = expr.literal;
                return true;
            }
            case Expression::Type::Calculated: {
                double lhsResult = 0.0, rhsResult = 0.0;
                if (!calculate(*expr.expr.left.get(), lhsResult)) return false;
                if (!calculate(*expr.expr.right.get(), rhsResult)) return false;
                switch (expr.expr.op) {
                    case Operator::Add: {
                        result = lhsResult + rhsResult;
                        return true;
                    }
                    case Operator::Subtract: {
                        result = lhsResult - rhsResult;
                        return true;
                    }
                    case Operator::Multiply: {
                        result = lhsResult * rhsResult;
                        return true;
                    }
                    case Operator::Divide: {
                        if (rhsResult == 0) {
                            errorMessage = "Tried to divide by 0.";
                            return false;
                        }
                        result = lhsResult / rhsResult;
                        return true;
                    }
                    case Operator::Power: {
                        result = pow(lhsResult, rhsResult);
                        return true;
                    }
                    default: {
                        errorMessage = "Invalid operator.";
                        return false;
                    }
                }
            }
            default: {
                errorMessage = "Invalid expression.";
                return false;
            }
        }
    }

    [[nodiscard]] bool calculateInfixExpression(const Expression& expr, double& result) noexcept { return calculate(expr, result); }
}