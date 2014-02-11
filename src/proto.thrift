namespace cpp clc.ipc

enum request_type {
  COMPLETION,
  JUMP_TO_DECLARATION,
  JUMP_TO_DEFINITION
}

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
  1: request_type type,
  2: cursor pos,
  3: optional list<unsaved_file> unsaved_files
}
