namespace cpp clc.rpc

struct cursor
{
  1: string file,
  2: i32 line,
  3: i32 column
}

struct unsaved_file
{
  1: string file,
  2: string temp_file
}

struct request
{
  2: cursor pos,
  3: optional list<unsaved_file> unsaved_files
}

struct completion_result
{
  1: string value;
}

struct completion_answer
{
  1: list<completion_result> completion_results
}

struct jump_answer
{
  1: cursor pos
}

service clc_if
{
  oneway void register_compilation(1: list<string> argv, 2: string cwd),
  jump_answer find_declaration(1: request r),
  jump_answer find_definition(1: request r),
  completion_answer complete(1: request r),
}
