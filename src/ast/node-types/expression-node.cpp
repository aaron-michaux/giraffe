#include "stdinc.hpp"

#include "expression-node.hpp"
#include "scanner/operators.hpp"

#define This ExpressionNode

namespace giraffe
{

// ------------------------------------------------------------------ make-empty

ExpressionNode* This::make_empty() noexcept
{
   return new ExpressionNode{ExprType::EMPTY};
}

// ------------------------------------------------------------- make-identifier

ExpressionNode* This::make_identifier(const SourceRange expr_range,
                                      string_view identifier) noexcept
{
   unique_ptr<ExpressionNode> node{new ExpressionNode{ExprType::IDENTIFIER, expr_range}};
   node->text_ = identifier;
   return node.release();
}

// ---------------------------------------------------------------- make-integer

ExpressionNode* This::make_integer(const SourceRange expr_range,
                                   string_view integer_str) noexcept
{
   unique_ptr<ExpressionNode> node{new ExpressionNode{ExprType::INTEGER, expr_range}};
   node->text_ = integer_str;
   return node.release();
}

// ---------------------------------------------------------------- make-subexpr

ExpressionNode* This::make_subexpr(const SourceRange expr_range,
                                   ExpressionNode* subexpr) noexcept
{
   unique_ptr<ExpressionNode> node{new ExpressionNode{ExprType::SUBEXPR, expr_range}};
   node->set_child_(subexpr);
   return node.release();
}

// ------------------------------------------------------------------ make-unary

ExpressionNode* This::make_unary(const int op,
                                 const SourceLocation op_loc,
                                 const SourceRange expr_range,
                                 ExpressionNode* expr) noexcept
{
   unique_ptr<ExpressionNode> node{new ExpressionNode{ExprType::UNARY, expr_range}};
   node->op_     = op;
   node->op_loc_ = op_loc;
   node->set_child_(expr);
   return node.release();
}

// ----------------------------------------------------------------- make-binary

ExpressionNode* This::make_binary(const int op,
                                  const SourceLocation op_loc,
                                  const SourceRange expr_range,
                                  ExpressionNode* lhs,
                                  ExpressionNode* rhs) noexcept
{
   unique_ptr<ExpressionNode> node{new ExpressionNode{ExprType::BINARY, expr_range}};
   node->op_     = op;
   node->op_loc_ = op_loc;
   node->set_children_(lhs, rhs);
   return node.release();
}

// ---------------------------------------------------------------------- stream

std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{
   auto print_op = [](int op) {
      switch(op) {
      case TPLUS: return "+";
      case TMINUS: return "-";
      case TSHOUT: return "!";
      case TTILDE: return "!";
      case TDEFINED: return "defined ";

      case TLE: return "<=";
      case TGE: return ">=";
      case TLT: return "<";
      case TGT: return ">";
      case TEQEQ: return "==";
      case TNE: return "!=";

      case TANDAND: return "&&";
      case TOROR: return "||";

      case TMULT: return "*";
      case TDIV: return "/";
      case TREMAINDER: return "%";

      case TAND: return "&";
      case TOR: return "|";
      case TCARROT: return "^";
      case TLTLT: return "<<";
      case TGTGT: return ">>";

      case TQUESTION: return "?";
      case TCOLON: return ":";

      default: FATAL("logic error");
      }
   };

   auto stream_child = [this](std::ostream& ss, int indent, ExpressionNode* child) {
      assert(expr_type() == ExprType::BINARY);
      const auto precedence = operator_precedence_q(op());
      const auto needs_paren
          = (child->expr_type() == ExprType::BINARY)
            && operator_precedence_q(op()) < binary_op_precedence(child->op());
      if(needs_paren) ss << '(';
      child->stream(ss, indent); // the sub-expression
      if(needs_paren) ss << ')';
   };

   switch(expr_type()) {
   case ExprType::NONE: ss << "<NONE-EXPRESSION>"; break;
   case ExprType::EMPTY: break; // do nothing!
   case ExprType::IDENTIFIER: ss << text(); break;
   case ExprType::INTEGER: ss << text(); break;
   case ExprType::SUBEXPR:
      ss << '(';
      children()[0]->stream(ss, indent); // the sub-expression
      ss << ')';
      break;
   case ExprType::UNARY:
      ss << print_op(op());
      children()[0]->stream(ss, indent); // the child expression
      break;
   case ExprType::BINARY:
      stream_child(ss, indent, lhs());
      ss << ' ' << print_op(op()) << ' ';
      stream_child(ss, indent, rhs());
      break;
   }
   return ss;
}

} // namespace giraffe
#undef This
