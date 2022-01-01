
#pragma once

#include "scanner/token.hpp"

#ifndef NDEBUG
#include <atomic>
#endif

namespace giraffe
{
// ---------------------------------------------------------- Predfine All Nodes

class AstNode;
class EmptyNode; // a placeholder for parse errors
class StmtListNode;

class TranslationUnitNode;
class ModuleNode; // export? import? module
class IfThenNode; // if-...-endif

class DefineNode;
class UndefNode;
class IncludeNode;
class ErrorNode;

class ExpressionNode; // Expression, Primary, Unary, Binary

// --------------------------------------------------------------- NodeType Enum

enum class NodeType : uint8_t {
   NONE = 0,
   EMPTY,     // a placeholder for parse errors
   STMT_LIST, // A list of statements (!)
   TRANSLATION_UNIT,
   MODULE,  // export? import? module IDENTIFIER
   IFTHEN,  // #if #ifdef #ifndef #elif #endif
   DEFINE,  // #define
   UNDEF,   // #undef
   INCLUDE, // #include
   ERROR,   // #error #warning
   EXPRESSION
};

const char* str(NodeType) noexcept;

// ---------------------------------------------------------------------- Detail
// to assist in printing
namespace detail
{

   using AstNodeSV = std::pair<const AstNode*, const TokenTextFunctor&>;

   template<std::derived_from<AstNode> T>
   T* cast_ast_node_helper_(AstNode* o, NodeType type) noexcept
   {
#define ELSE_IF_(S, N)                                       \
   else if constexpr(std::is_same_v<std::decay_t<T>, N>)     \
   {                                                         \
      if(type == NodeType::S) ret = reinterpret_cast<T*>(o); \
   }

      T* ret = nullptr;
      if constexpr(std::is_same_v<std::decay_t<T>, AstNode>) {
         ret = o; // not a cast
      }
      ELSE_IF_(EMPTY, EmptyNode)
      ELSE_IF_(STMT_LIST, StmtListNode)
      ELSE_IF_(TRANSLATION_UNIT, TranslationUnitNode)
      ELSE_IF_(MODULE, ModuleNode)
      ELSE_IF_(IFTHEN, IfThenNode)
      ELSE_IF_(DEFINE, DefineNode)
      ELSE_IF_(UNDEF, UndefNode)
      ELSE_IF_(INCLUDE, IncludeNode)
      ELSE_IF_(ERROR, ErrorNode)
      ELSE_IF_(EXPRESSION, ExpressionNode)
      else
      {
         FATAL("missing case");
         assert(false);
      }
      return ret;

#undef ELSE_IF_
   }

} // namespace detail

// --------------------------------------------------------------------- AstNode

class AstNode
{
 private:
   AstNode* parent_           = nullptr;
   vector<AstNode*> children_ = {};
   size_t index_in_parent_    = 0;
   NodeType node_type_        = NodeType::NONE;
   SourceRange loc_           = {};

 protected:
   using PP = detail::AstNodeSV;

   vector<AstNode*> release_children_() noexcept
   {
      vector<AstNode*> ret = std::move(children_);
      children_            = {};
      return ret;
   }

   template<typename T> T* cast_child_(size_t index) const noexcept
   {
      assert(index < children_.size());
      auto o = children_[index];
      return detail::cast_ast_node_helper_<T>(o, o->node_type());
   }

   void set_location_(SourceRange loc) noexcept { loc_ = loc; }

   void set_location_(SourceLocation loc0, SourceLocation loc1) noexcept
   {
      set_location_(SourceRange{loc0, loc1});
   }

   void set_child_(AstNode* child) noexcept
   {
      vector<AstNode*> children;
      children.resize(1, nullptr);
      children[0] = child;
      set_children_(std::move(children));
   }

   void set_children_(AstNode* child0, AstNode* child1) noexcept
   {
      vector<AstNode*> children;
      children.resize(2, nullptr);
      children[0] = child0;
      children[1] = child1;
      set_children_(std::move(children));
   }

   void set_children_(vector<AstNode*>&& children) noexcept
   {
      children_    = std::move(children);
      size_t index = 0;
      for(auto ptr : children_) {
         ptr->parent_          = this;
         ptr->index_in_parent_ = index++;
      }
   }

#ifndef NDEBUG
   static std::atomic<int64_t> node_count_;
   static void add_to_node_count_(int64_t value) noexcept
   {
      node_count_.fetch_add(value, std::memory_order_relaxed);
   }
   static void inc_node_count_() noexcept { add_to_node_count_(1); }
   static void dec_node_count_() noexcept { add_to_node_count_(-1); }
#endif

 public:
   AstNode(NodeType type, SourceRange loc = {})
       : node_type_{type}
       , loc_{loc}
   {
#ifndef NDEBUG
      inc_node_count_();
#endif
   }
   AstNode(AstNode&&)      = delete;
   AstNode(const AstNode&) = delete;
   virtual ~AstNode()
   {
      for(auto child : children_) std::destroy_at(child);
#ifndef NDEBUG
      dec_node_count_();
#endif
   }
   AstNode& operator=(AstNode&&) = delete;
   AstNode& operator=(const AstNode&) = delete;

#ifdef NDEBUG
   static auto get_node_count() noexcept { return 0; }
#else
   static auto get_node_count() noexcept
   {
      return node_count_.load(std::memory_order_relaxed);
   }
#endif

   auto parent() const noexcept { return parent_; }
   const auto& children() const noexcept { return children_; }
   auto index_in_parent() const noexcept { return index_in_parent_; }
   const auto location() const noexcept { return loc_; }
   const auto loc0() const noexcept { return loc_.first; }
   const auto loc1() const noexcept { return loc_.second; }

   auto begin() const noexcept { return children_.begin(); }
   auto rbegin() const noexcept { return children_.rbegin(); }
   auto end() const noexcept { return children_.end(); }
   auto rend() const noexcept { return children_.rend(); }

   auto front() const noexcept { return children_.front(); }
   auto back() const noexcept { return children_.back(); }

   NodeType node_type() const noexcept { return node_type_; }
   size_t size() const noexcept { return children_.size(); }
   bool empty() const noexcept { return children_.empty(); }

   virtual std::ostream& stream(std::ostream&, const int) const noexcept = 0;
};

// ------------------------------------------------------------------ operator<<

// inline std::ostream& operator<<(std::ostream& ss, detail::AstNodeSV pp)
// {
//    if(std::get<0>(pp) == nullptr)
//       ss << "<nullptr>";
//    else
//       std::get<0>(pp)->stream(ss, std::get<1>(pp));
//    return ss;
// }

// -------------------------------------------------- safely downcast an AstNode

template<std::derived_from<AstNode> T> T* cast_ast_node(AstNode* o) noexcept
{
   return detail::cast_ast_node_helper_<T>(o, o->node_type());
}

template<std::derived_from<AstNode> T> const T* cast_ast_node(const AstNode* o) noexcept
{
   return cast_ast_node<T>(const_cast<AstNode*>(o));
}

// ------------------------------------------------- recursively visit all nodes

template<std::derived_from<AstNode> T, // T must be a subclass of AstNode
         std::invocable<AstNode*> F>   // f(T*) must work
inline bool preorder_traversal(T* node, F&& f) noexcept
{
   assert(node != nullptr);
   if(!f(node)) return false; // Visit the node
   for(auto&& child : node->children())
      if(!preorder_traversal(child, f)) return false; // Visit all children
   return true;
}

} // namespace giraffe
