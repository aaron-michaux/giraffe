
namespace giraffe
{
/**
 * auto contents = read_stream<string>(ifstream("filename"));
 * auto contents = read_stream<vector>(ifstream("filename"));
 */
template<typename T> inline T read_stream(auto&& in)
{
   in >> std::noskipws;
   std::istream_iterator<char> bb(in);
   std::istream_iterator<char> ee;
   return T(bb, ee);
}

} // namespace giraffe
