au FileType c,cpp call <SID>clc_init()

let s:plugin_path = escape(expand('<sfile>:p:h'), '\')

function! s:clc_init()
  exe 'python sys.path = ["' . s:plugin_path . '"] + sys.path'
  python import clc
  python vim.command('let l:res = ' + str(clc.clc_init()))

  if l:res == 1
    nnoremap <buffer> <silent> <C-k> :python clc.clc_goto_definition()<CR>
  else
    echoe 'Fail to initialize clang-cache client'
  endif
endfunction
