namespace cpp clc.rpc

struct cursor
{
  1: string file,
  2: i32 line,
  3: i32 column
}

service clc_if
{
  oneway void register_compilation(1: list<string> argv, 2: string cwd),
  cursor find_definition(1: string usr),
}
