#include "stdinc.hpp"

#include "command-node.hpp"

#define This CommandNode

namespace giraffe {

CommandNode * This::make_none() noexcept
{
   auto node = make_unique<CommandNode>();
   return node.release();   
}

CommandNode * This::make_include(String filename) noexcept
{
   auto node = make_unique<CommandNode>();
   node->type_ = CommandType::INCLUDE;
   assert(filename.size() >= 2);
   node->is_local_include_ = (filename[0] == '"');   
   node->text_ = string{filename.begin() + 1, filename.begin() + filename.size() - 2};
   return node.release();

}

CommandNode * This::make_undef(String identifier) noexcept
{
   auto node = make_unique<CommandNode>();
   node->type_ = CommandType::UNDEF;
   node->text_ = string{identifier.begin(), identifier.end()};
   return node.release();
}


std::ostream& This::stream(std::ostream& ss, const int indent) const noexcept
{

   return ss;
}

}
