namespace cpp clc.rpc

struct cursor {
  1: string file,
  2: i32 line,
  3: i32 column
}

struct unsaved_file {
  1: string file,
  2: string temp_file
}

struct request {
  2: cursor pos,
  3: optional list<unsaved_file> unsaved_files
}

struct completion_result {
  1: string value;
}

struct completion_answer {
  1: list<completion_result> completion_results
}

struct jump_answer {
  1: cursor pos
}

service clc_if {
  completion_answer complete(1: request r),
  jump_answer jump_to_declaration(1: request r),
  jump_answer jump_to_definition(1: request r),

  oneway void add_file(1: list<string> argv),
}
