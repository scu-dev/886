#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string>

#include "../util/concepts.hpp"

namespace InfixParser {
    typedef uint8_t u8;
    typedef uint64_t u64;
    using std::array, std::string, std::unique_ptr, std::make_unique, std::move, Util::all_is;

    enum struct Operator : u8 {
        Add, Subtract, Multiply, Divide, Power, UnaryMinus
    };

    inline constexpr array<u64, 12> bindingPowers = {
        1000, 2000, 1000, 2000, 3000, 4000, 3000, 4000, 7000, 6000, 5000, 5000
    };
    [[nodiscard]] inline constexpr u64 getBindPower(Operator op, bool right) noexcept { return bindingPowers[static_cast<u64>(op) * 2 + (right ? 1 : 0)]; }

    struct Expression {
        union {
            double literal;
            struct {
                unique_ptr<Expression> left, right;
                Operator op;
            } expr;
        };
        enum struct Type : u8 {
            Literal, Calculated, Count
        } type;

        #pragma warning(suppress : 26495)
        Expression() noexcept : type(Type::Count) {}
        Expression(double literal) noexcept : type(Type::Literal), literal(literal) {}
        Expression(const Expression& left, const Expression& right, Operator op) noexcept : type(Type::Calculated), expr{make_unique<Expression>(left), make_unique<Expression>(right), op} {}
        #pragma warning(suppress : 26495)
        Expression(const Expression& other) noexcept : type(other.type) {
            switch (type) {
                case Type::Literal: {
                    literal = other.literal;
                    break;
                }
                case Type::Calculated: {
                    auto tempLeft = make_unique<Expression>(*other.expr.left), tempRight = make_unique<Expression>(*other.expr.right);
                    new (&expr.left) unique_ptr<Expression>(move(tempLeft));
                    new (&expr.right) unique_ptr<Expression>(move(tempRight));
                    expr.op = other.expr.op;
                    break;
                }
                default: break;
            }
        }
        Expression& operator=(const Expression& other) noexcept {
            if (this == &other) return *this;
            if (this->type != Type::Count) this->~Expression();
            return *new (this) Expression(other);
        }
        ~Expression() {
            if (type == Type::Calculated) {
                expr.left.~unique_ptr();
                expr.right.~unique_ptr();
            }
        }
    };

    [[nodiscard]] bool parseInfixExpression(const string& expression, Expression& result) noexcept;
    [[nodiscard]] bool calculateInfixExpression(const Expression& expr, double& result) noexcept;
    [[nodiscard]] string getErrorMessage() noexcept;
}