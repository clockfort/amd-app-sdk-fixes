" Vim syn file
" Language:	C
" Maintainer:	Bram Moolenaar <Bram@vim.org>
" Last Change:	2009 Nov 17

" Quit whn a (custom) syn file was already loaded
if exists("b:current_syn")
  finish
endif

" A bunch of useful C keywords
syn keyword	cStatement	goto break return continue asm
syn keyword	cLabel		case default
syn keyword	cConditional	if else switch
syn keyword	cRepeat		while for do

syn keyword	cTodo		contained TODO FIXME XXX

" It's easy to accidentally add a space after a backslash that was intended
" for line continuation.  Some compilers allow it, which makes it
" unpredicatable and should be avoided.
syn match	cBadContinuation contained "\\\s\+$"

" cCommentGroup allows adding matches for special things in comments
syn cluster	cCommentGroup	contains=cTodo,cBadContinuation

" String and Character constants
" Highlight special characters (those which have a backslash) differently

syn match	cSpecial	display contained "\\\(x\x\+\|\o\{1,3}\|.\|$\)"
if !exists("c_no_utf")
  syn match	cSpecial	display contained "\\\(u\x\{4}\|U\x\{8}\)"
endif
if exists("c_no_cformat")
  syn region	cString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial,@Spell
  " cCppString: same as cString, but ends at end of line
  syn region	cCppString	start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$' contains=cSpecial,@Spell
else
  if !exists("c_no_c99") " ISO C99
    syn match	cFormat		display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlLjzt]\|ll\|hh\)\=\([aAbdiuoxXDOUfFeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
  else
    syn match	cFormat		display "%\(\d\+\$\)\=[-+' #0*]*\(\d*\|\*\|\*\d\+\$\)\(\.\(\d*\|\*\|\*\d\+\$\)\)\=\([hlL]\|ll\)\=\([bdiuoxXDOUfeEgGcCsSpn]\|\[\^\=.[^]]*\]\)" contained
  endif
  syn match	cFormat		display "%%" contained
  syn region	cString		start=+L\="+ skip=+\\\\\|\\"+ end=+"+ contains=cSpecial,cFormat,@Spell
  " cCppString: same as cString, but ends at end of line
  syn region	cCppString	start=+L\="+ skip=+\\\\\|\\"\|\\$+ excludenl end=+"+ end='$' contains=cSpecial,cFormat,@Spell
endif

syn match	cCharacter	"L\='[^\\]'"
syn match	cCharacter	"L'[^']*'" contains=cSpecial
if exists("c_gnu")
  syn match	cSpecialError	"L\='\\[^'\"?\\abefnrtv]'"
  syn match	cSpecialCharacter "L\='\\['\"?\\abefnrtv]'"
else
  syn match	cSpecialError	"L\='\\[^'\"?\\abfnrtv]'"
  syn match	cSpecialCharacter "L\='\\['\"?\\abfnrtv]'"
endif
syn match	cSpecialCharacter display "L\='\\\o\{1,3}'"
syn match	cSpecialCharacter display "'\\x\x\{1,2}'"
syn match	cSpecialCharacter display "L'\\x\x\+'"

"when wanted, highlight trailing white space
if exists("c_space_errors")
  if !exists("c_no_trail_space_error")
    syn match	cSpaceError	display excludenl "\s\+$"
  endif
  if !exists("c_no_tab_space_error")
    syn match	cSpaceError	display " \+\t"me=e-1
  endif
endif

" This should be before cErrInParen to avoid problems with #define ({ xxx })
if exists("c_curly_error")
  syn match cCurlyError "}"
  syn region	cBlock		start="{" end="}" contains=ALLBUT,cCurlyError,@cParenGroup,cErrInParen,cCppParen,cErrInBracket,cCppBracket,cCppString,@Spell fold
else
  syn region	cBlock		start="{" end="}" transparent fold
endif

"catch errors caused by wrong parenthesis and brackets
" also accept <% for {, %> for }, <: for [ and :> for ] (C99)
" But avoid matching <::.
syn cluster	cParenGroup	contains=cParenError,cIncluded,cSpecial,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cCommentStartError,cUserCont,cUserLabel,cBitField,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom
if exists("c_no_curly_error")
  syn region	cParen		transparent start='(' end=')' contains=ALLBUT,@cParenGroup,cCppParen,cCppString,@Spell
  " cCppParen: same as cParen but ends at end-of-line; used in cDefine
  syn region	cCppParen	transparent start='(' skip='\\$' excludenl end=')' end='$' contained contains=ALLBUT,@cParenGroup,cParen,cString,@Spell
  syn match	cParenError	display ")"
  syn match	cErrInParen	display contained "^[{}]\|^<%\|^%>"
elseif exists("c_no_bracket_error")
  syn region	cParen		transparent start='(' end=')' contains=ALLBUT,@cParenGroup,cCppParen,cCppString,@Spell
  " cCppParen: same as cParen but ends at end-of-line; used in cDefine
  syn region	cCppParen	transparent start='(' skip='\\$' excludenl end=')' end='$' contained contains=ALLBUT,@cParenGroup,cParen,cString,@Spell
  syn match	cParenError	display ")"
  syn match	cErrInParen	display contained "[{}]\|<%\|%>"
else
  syn region	cParen		transparent start='(' end=')' contains=ALLBUT,@cParenGroup,cCppParen,cErrInBracket,cCppBracket,cCppString,@Spell
  " cCppParen: same as cParen but ends at end-of-line; used in cDefine
  syn region	cCppParen	transparent start='(' skip='\\$' excludenl end=')' end='$' contained contains=ALLBUT,@cParenGroup,cErrInBracket,cParen,cBracket,cString,@Spell
  syn match	cParenError	display "[\])]"
  syn match	cErrInParen	display contained "[\]{}]\|<%\|%>"
  syn region	cBracket	transparent start='\[\|<::\@!' end=']\|:>' contains=ALLBUT,@cParenGroup,cErrInParen,cCppParen,cCppBracket,cCppString,@Spell
  " cCppBracket: same as cParen but ends at end-of-line; used in cDefine
  syn region	cCppBracket	transparent start='\[\|<::\@!' skip='\\$' excludenl end=']\|:>' end='$' contained contains=ALLBUT,@cParenGroup,cErrInParen,cParen,cBracket,cString,@Spell
  syn match	cErrInBracket	display contained "[);{}]\|<%\|%>"
endif

"integer number, or floating point number without a dot and with "f".
syn case ignore
syn match	cNumbers	display transparent "\<\d\|\.\d" contains=cNumber,cFloat,cOctalError,cOctal
" Same, but without octal error (for comments)
syn match	cNumbersCom	display contained transparent "\<\d\|\.\d" contains=cNumber,cFloat,cOctal
syn match	cNumber		display contained "\d\+\(u\=l\{0,2}\|ll\=u\)\>"
"hex number
syn match	cNumber		display contained "0x\x\+\(u\=l\{0,2}\|ll\=u\)\>"
" Flag the first zero of an octal number as something special
syn match	cOctal		display contained "0\o\+\(u\=l\{0,2}\|ll\=u\)\>" contains=cOctalZero
syn match	cOctalZero	display contained "\<0"
syn match	cFloat		display contained "\d\+f"
"floating point number, with dot, optional exponent
syn match	cFloat		display contained "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="
"floating point number, starting with a dot, optional exponent
syn match	cFloat		display contained "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"
"floating point number, without dot, with exponent
syn match	cFloat		display contained "\d\+e[-+]\=\d\+[fl]\=\>"
if !exists("c_no_c99")
  "hexadecimal floating point number, optional leading digits, with dot, with exponent
  syn match	cFloat		display contained "0x\x*\.\x\+p[-+]\=\d\+[fl]\=\>"
  "hexadecimal floating point number, with leading digits, optional dot, with exponent
  syn match	cFloat		display contained "0x\x\+\.\=p[-+]\=\d\+[fl]\=\>"
endif

" flag an octal number with wrong digits
syn match	cOctalError	display contained "0\o*[89]\d*"
syn case match

if exists("c_comment_strings")
  " A comment can contain cString, cCharacter and cNumber.
  " But a "*/" inside a cString in a cComment DOES end the comment!  So we
  " need to use a special type of cString: cCommentString, which also ends on
  " "*/", and sees a "*" at the start of the line as comment again.
  " Unfortunately this doesn't very well work for // type of comments :-(
  syn match	cCommentSkip	contained "^\s*\*\($\|\s\+\)"
  syn region cCommentString	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end=+\*/+me=s-1 contains=cSpecial,cCommentSkip
  syn region cComment2String	contained start=+L\=\\\@<!"+ skip=+\\\\\|\\"+ end=+"+ end="$" contains=cSpecial
  syn region  cCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,cComment2String,cCharacter,cNumbersCom,cSpaceError,@Spell
  if exists("c_no_comment_fold")
    " Use "extend" here to have preprocessor lines not terminate halfway a
    " comment.
    syn region cComment	matchgroup=cCommentStart start="/\*" end="\*/" contains=@cCommentGroup,cCommentStartError,cCommentString,cCharacter,cNumbersCom,cSpaceError,@Spell extend
  else
    syn region cComment	matchgroup=cCommentStart start="/\*" end="\*/" contains=@cCommentGroup,cCommentStartError,cCommentString,cCharacter,cNumbersCom,cSpaceError,@Spell fold extend
  endif
else
  syn region	cCommentL	start="//" skip="\\$" end="$" keepend contains=@cCommentGroup,cSpaceError,@Spell
  if exists("c_no_comment_fold")
    syn region	cComment	matchgroup=cCommentStart start="/\*" end="\*/" contains=@cCommentGroup,cCommentStartError,cSpaceError,@Spell extend
  else
    syn region	cComment	matchgroup=cCommentStart start="/\*" end="\*/" contains=@cCommentGroup,cCommentStartError,cSpaceError,@Spell fold extend
  endif
endif
" keep a // comment separately, it terminates a preproc. conditional
syn match	cCommentError	display "\*/"
syn match	cCommentStartError display "/\*"me=e-1 contained

syn keyword	cOperator	sizeof
if exists("c_gnu")
  syn keyword	cStatement	__asm__
  syn keyword	cOperator	typeof __real__ __imag__
endif
syn keyword	cType		int long short char void
syn keyword	cType		signed unsigned float double
if !exists("c_no_ansi") || exists("c_ansi_typedefs")
  syn keyword   cType		size_t ssize_t off_t wchar_t ptrdiff_t sig_atomic_t fpos_t
  syn keyword   cType		clock_t time_t va_list jmp_buf FILE DIR div_t ldiv_t
  syn keyword   cType		mbstate_t wctrans_t wint_t wctype_t
endif
if !exists("c_no_c99") " ISO C99
  syn keyword	cType		bool complex
  syn keyword	cType		int8_t int16_t int32_t int64_t
  syn keyword	cType		uint8_t uint16_t uint32_t uint64_t
  syn keyword	cType		int_least8_t int_least16_t int_least32_t int_least64_t
  syn keyword	cType		uint_least8_t uint_least16_t uint_least32_t uint_least64_t
  syn keyword	cType		int_fast8_t int_fast16_t int_fast32_t int_fast64_t
  syn keyword	cType		uint_fast8_t uint_fast16_t uint_fast32_t uint_fast64_t
  syn keyword	cType		intptr_t uintptr_t
  syn keyword	cType		intmax_t uintmax_t
endif
if exists("c_gnu")
  syn keyword	cType		__label__ __complex__ __volatile__
endif

syn keyword	cStructure	struct union enum typedef
syn keyword	cStorageClass	static register auto volatile extern const
if exists("c_gnu")
  syn keyword	cStorageClass	inline __attribute__
endif
if !exists("c_no_c99")
  syn keyword	cStorageClass	inline restrict
endif

if !exists("c_no_ansi") || exists("c_ansi_constants") || exists("c_gnu")
  if exists("c_gnu")
    syn keyword cConstant __GNUC__ __FUNCTION__ __PRETTY_FUNCTION__ __func__
  endif
  syn keyword cConstant __LINE__ __FILE__ __DATE__ __TIME__ __STDC__
  syn keyword cConstant __STDC_VERSION__
  syn keyword cConstant CHAR_BIT MB_LEN_MAX MB_CUR_MAX
  syn keyword cConstant UCHAR_MAX UINT_MAX ULONG_MAX USHRT_MAX
  syn keyword cConstant CHAR_MIN INT_MIN LONG_MIN SHRT_MIN
  syn keyword cConstant CHAR_MAX INT_MAX LONG_MAX SHRT_MAX
  syn keyword cConstant SCHAR_MIN SINT_MIN SLONG_MIN SSHRT_MIN
  syn keyword cConstant SCHAR_MAX SINT_MAX SLONG_MAX SSHRT_MAX
  if !exists("c_no_c99")
    syn keyword cConstant __func__
    syn keyword cConstant LLONG_MIN LLONG_MAX ULLONG_MAX
    syn keyword cConstant INT8_MIN INT16_MIN INT32_MIN INT64_MIN
    syn keyword cConstant INT8_MAX INT16_MAX INT32_MAX INT64_MAX
    syn keyword cConstant UINT8_MAX UINT16_MAX UINT32_MAX UINT64_MAX
    syn keyword cConstant INT_LEAST8_MIN INT_LEAST16_MIN INT_LEAST32_MIN INT_LEAST64_MIN
    syn keyword cConstant INT_LEAST8_MAX INT_LEAST16_MAX INT_LEAST32_MAX INT_LEAST64_MAX
    syn keyword cConstant UINT_LEAST8_MAX UINT_LEAST16_MAX UINT_LEAST32_MAX UINT_LEAST64_MAX
    syn keyword cConstant INT_FAST8_MIN INT_FAST16_MIN INT_FAST32_MIN INT_FAST64_MIN
    syn keyword cConstant INT_FAST8_MAX INT_FAST16_MAX INT_FAST32_MAX INT_FAST64_MAX
    syn keyword cConstant UINT_FAST8_MAX UINT_FAST16_MAX UINT_FAST32_MAX UINT_FAST64_MAX
    syn keyword cConstant INTPTR_MIN INTPTR_MAX UINTPTR_MAX
    syn keyword cConstant INTMAX_MIN INTMAX_MAX UINTMAX_MAX
    syn keyword cConstant PTRDIFF_MIN PTRDIFF_MAX SIG_ATOMIC_MIN SIG_ATOMIC_MAX
    syn keyword cConstant SIZE_MAX WCHAR_MIN WCHAR_MAX WINT_MIN WINT_MAX
  endif
  syn keyword cConstant FLT_RADIX FLT_ROUNDS
  syn keyword cConstant FLT_DIG FLT_MANT_DIG FLT_EPSILON
  syn keyword cConstant DBL_DIG DBL_MANT_DIG DBL_EPSILON
  syn keyword cConstant LDBL_DIG LDBL_MANT_DIG LDBL_EPSILON
  syn keyword cConstant FLT_MIN FLT_MAX FLT_MIN_EXP FLT_MAX_EXP
  syn keyword cConstant FLT_MIN_10_EXP FLT_MAX_10_EXP
  syn keyword cConstant DBL_MIN DBL_MAX DBL_MIN_EXP DBL_MAX_EXP
  syn keyword cConstant DBL_MIN_10_EXP DBL_MAX_10_EXP
  syn keyword cConstant LDBL_MIN LDBL_MAX LDBL_MIN_EXP LDBL_MAX_EXP
  syn keyword cConstant LDBL_MIN_10_EXP LDBL_MAX_10_EXP
  syn keyword cConstant HUGE_VAL CLOCKS_PER_SEC NULL
  syn keyword cConstant LC_ALL LC_COLLATE LC_CTYPE LC_MONETARY
  syn keyword cConstant LC_NUMERIC LC_TIME
  syn keyword cConstant SIG_DFL SIG_ERR SIG_IGN
  syn keyword cConstant SIGABRT SIGFPE SIGILL SIGHUP SIGINT SIGSEGV SIGTERM
  " Add POSIX signals as well...
  syn keyword cConstant SIGABRT SIGALRM SIGCHLD SIGCONT SIGFPE SIGHUP
  syn keyword cConstant SIGILL SIGINT SIGKILL SIGPIPE SIGQUIT SIGSEGV
  syn keyword cConstant SIGSTOP SIGTERM SIGTRAP SIGTSTP SIGTTIN SIGTTOU
  syn keyword cConstant SIGUSR1 SIGUSR2
  syn keyword cConstant _IOFBF _IOLBF _IONBF BUFSIZ EOF WEOF
  syn keyword cConstant FOPEN_MAX FILENAME_MAX L_tmpnam
  syn keyword cConstant SEEK_CUR SEEK_END SEEK_SET
  syn keyword cConstant TMP_MAX stderr stdin stdout
  syn keyword cConstant EXIT_FAILURE EXIT_SUCCESS RAND_MAX
  " Add POSIX errors as well
  syn keyword cConstant E2BIG EACCES EAGAIN EBADF EBADMSG EBUSY
  syn keyword cConstant ECANCELED ECHILD EDEADLK EDOM EEXIST EFAULT
  syn keyword cConstant EFBIG EILSEQ EINPROGRESS EINTR EINVAL EIO EISDIR
  syn keyword cConstant EMFILE EMLINK EMSGSIZE ENAMETOOLONG ENFILE ENODEV
  syn keyword cConstant ENOENT ENOEXEC ENOLCK ENOMEM ENOSPC ENOSYS
  syn keyword cConstant ENOTDIR ENOTEMPTY ENOTSUP ENOTTY ENXIO EPERM
  syn keyword cConstant EPIPE ERANGE EROFS ESPIPE ESRCH ETIMEDOUT EXDEV
  " math.h
  syn keyword cConstant M_E M_LOG2E M_LOG10E M_LN2 M_LN10 M_PI M_PI_2 M_PI_4
  syn keyword cConstant M_1_PI M_2_PI M_2_SQRTPI M_SQRT2 M_SQRT1_2
endif
if !exists("c_no_c99") " ISO C99
  syn keyword cConstant true false
endif

" Accept %: for # (C99)
syn region	cPreCondit	start="^\s*\(%:\|#\)\s*\(if\|ifdef\|ifndef\|elif\)\>" skip="\\$" end="$"  contains=cComment,cCommentL,cCppString,cCharacter,cCppParen,cParenError,cNumbers,cCommentError,cSpaceError
syn match	cPreCondit	display "^\s*\(%:\|#\)\s*\(else\|endif\)\>"
if !exists("c_no_if0")
  if !exists("c_no_if0_fold")
    syn region	cCppOut		start="^\s*\(%:\|#\)\s*if\s\+0\+\>" end=".\@=\|$" contains=cCppOut2 fold
  else
    syn region	cCppOut		start="^\s*\(%:\|#\)\s*if\s\+0\+\>" end=".\@=\|$" contains=cCppOut2
  endif
  syn region	cCppOut2	contained start="0" end="^\s*\(%:\|#\)\s*\(endif\>\|else\>\|elif\>\)" contains=cSpaceError,cCppSkip
  syn region	cCppSkip	contained start="^\s*\(%:\|#\)\s*\(if\>\|ifdef\>\|ifndef\>\)" skip="\\$" end="^\s*\(%:\|#\)\s*endif\>" contains=cSpaceError,cCppSkip
endif
syn region	cIncluded	display contained start=+"+ skip=+\\\\\|\\"+ end=+"+
syn match	cIncluded	display contained "<[^>]*>"
syn match	cInclude	display "^\s*\(%:\|#\)\s*include\>\s*["<]" contains=cIncluded
"syn match cLineSkip	"\\$"
syn cluster	cPreProcGroup	contains=cPreCondit,cIncluded,cInclude,cDefine,cErrInParen,cErrInBracket,cUserLabel,cSpecial,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom,cString,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cCommentStartError,cParen,cBracket,cMulti
syn region	cDefine		start="^\s*\(%:\|#\)\s*\(define\|undef\)\>" skip="\\$" end="$" keepend contains=ALLBUT,@cPreProcGroup,@Spell
syn region	cPreProc	start="^\s*\(%:\|#\)\s*\(pragma\>\|line\>\|warning\>\|warn\>\|error\>\)" skip="\\$" end="$" keepend contains=ALLBUT,@cPreProcGroup,@Spell

" Highlight User Labels
syn cluster	cMultiGroup	contains=cIncluded,cSpecial,cCommentSkip,cCommentString,cComment2String,@cCommentGroup,cCommentStartError,cUserCont,cUserLabel,cBitField,cOctalZero,cCppOut,cCppOut2,cCppSkip,cFormat,cNumber,cFloat,cOctal,cOctalError,cNumbersCom,cCppParen,cCppBracket,cCppString
syn region	cMulti		transparent start='?' skip='::' end=':' contains=ALLBUT,@cMultiGroup,@Spell
" Avoid matching foo::bar() in C++ by requiring that the next char is not ':'
syn cluster	cLabelGroup	contains=cUserLabel
syn match	cUserCont	display "^\s*\I\i*\s*:$" contains=@cLabelGroup
syn match	cUserCont	display ";\s*\I\i*\s*:$" contains=@cLabelGroup
syn match	cUserCont	display "^\s*\I\i*\s*:[^:]"me=e-1 contains=@cLabelGroup
syn match	cUserCont	display ";\s*\I\i*\s*:[^:]"me=e-1 contains=@cLabelGroup

syn match	cUserLabel	display "\I\i*" contained

" Avoid recognizing most bitfields as labels
syn match	cBitField	display "^\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=cType
syn match	cBitField	display ";\s*\I\i*\s*:\s*[1-9]"me=e-1 contains=cType

if exists("c_minlines")
  let b:c_minlines = c_minlines
else
  if !exists("c_no_if0")
    let b:c_minlines = 50	" #if 0 constructs can be long
  else
    let b:c_minlines = 15	" mostly for () constructs
  endif
endif
if exists("c_curly_error")
  syn sync fromstart
else
  exec "syn sync ccomment cComment minlines=" . b:c_minlines
endif

" Data Types
  syn keyword clType cl_char
  syn keyword clType cl_uchar
  syn keyword clType cl_short
  syn keyword clType cl_ushort
  syn keyword clType cl_int
  syn keyword clType cl_uint
  syn keyword clType cl_long
  syn keyword clType cl_ulong
  syn keyword clType cl_half
  syn keyword clType cl_float
  syn keyword clType cl_double
  "
  syn keyword clType cl_platform_id
  syn keyword clType cl_device_id
  syn keyword clType cl_context
  syn keyword clType cl_command_queue
  syn keyword clType cl_mem
  syn keyword clType cl_program
  syn keyword clType cl_kernel
  syn keyword clType cl_event
  syn keyword clType cl_sampler
  "
  syn keyword clType cl_bool
  syn keyword clType cl_bitfield
  syn keyword clType cl_device_type
  syn keyword clType cl_platform_info
  syn keyword clType cl_device_info
  syn keyword clType cl_device_address_info
  syn keyword clType cl_device_fp_config
  syn keyword clType cl_device_mem_cache_type
  syn keyword clType cl_device_local_mem_type
  syn keyword clType cl_device_exec_capabilities
  syn keyword clType cl_command_queue_properties
  "
  syn keyword clType cl_context_properties
  syn keyword clType cl_context_info
  syn keyword clType cl_command_queue_info
  syn keyword clType cl_channel_order
  syn keyword clType cl_channel_type
  syn keyword clType cl_mem_flags
  syn keyword clType cl_mem_object_type
  syn keyword clType cl_mem_info
  syn keyword clType cl_image_info
  syn keyword clType cl_addressing_mode
  syn keyword clType cl_filter_mode
  syn keyword clType cl_sampler_info
  syn keyword clType cl_map_flags
  syn keyword clType cl_program_info
  syn keyword clType cl_program_build_info
  syn keyword clType cl_build_status
  syn keyword clType cl_kernel_info
  syn keyword clType cl_kernel_work_group_info
  syn keyword clType cl_event_info
  syn keyword clType cl_command_type
  syn keyword clType cl_profiling_info
  syn keyword clType cl_image_format
"
"
" Constants
  syn keyword clConstant CL_FALSE
  syn keyword clConstant CL_TRUE
  "
  syn keyword clConstant CL_PLATFORM_PROFILE
  syn keyword clConstant CL_PLATFORM_VERSION
  syn keyword clConstant CL_PLATFORM_NAME
  syn keyword clConstant CL_PLATFORM_VENDOR
  syn keyword clConstant CL_PLATFORM_EXTENSIONS
  " 
  syn keyword clConstant CL_DEVICE_TYPE_DEFAULT 
  syn keyword clConstant CL_DEVICE_TYPE_CPU
  syn keyword clConstant CL_DEVICE_TYPE_GPU
  syn keyword clConstant CL_DEVICE_TYPE_ACCELERATOR
  syn keyword clConstant CL_DEVICE_TYPE_ALL
  "  
  syn keyword clConstant CL_DEVICE_TYPE
  syn keyword clConstant CL_DEVICE_VENDOR_ID
  syn keyword clConstant CL_DEVICE_MAX_COMPUTE_UNITS
  syn keyword clConstant CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS
  syn keyword clConstant CL_DEVICE_MAX_WORK_GROUP_SIZE
  syn keyword clConstant CL_DEVICE_MAX_WORK_ITEM_SIZES
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT
  syn keyword clConstant CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE
  syn keyword clConstant CL_DEVICE_MAX_CLOCK_FREQUENCY
  syn keyword clConstant CL_DEVICE_ADDRESS_BITS
  syn keyword clConstant CL_DEVICE_MAX_READ_IMAGE_ARGS
  syn keyword clConstant CL_DEVICE_MAX_WRITE_IMAGE_ARGS
  syn keyword clConstant CL_DEVICE_MAX_MEM_ALLOC_SIZE
  syn keyword clConstant CL_DEVICE_IMAGE2D_MAX_WIDTH
  syn keyword clConstant CL_DEVICE_IMAGE2D_MAX_HEIGHT
  syn keyword clConstant CL_DEVICE_IMAGE3D_MAX_WIDTH
  syn keyword clConstant CL_DEVICE_IMAGE3D_MAX_HEIGHT
  syn keyword clConstant CL_DEVICE_IMAGE3D_MAX_DEPTH
  syn keyword clConstant CL_DEVICE_IMAGE_SUPPORT
  syn keyword clConstant CL_DEVICE_MAX_PARAMETER_SIZE
  syn keyword clConstant CL_DEVICE_MAX_SAMPLERS
  syn keyword clConstant CL_DEVICE_MEM_BASE_ADDR_ALIGN
  syn keyword clConstant CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE
  syn keyword clConstant CL_DEVICE_SINGLE_FP_CONFIG
  syn keyword clConstant CL_DEVICE_GLOBAL_MEM_CACHE_TYPE
  syn keyword clConstant CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE
  syn keyword clConstant CL_DEVICE_GLOBAL_MEM_CACHE_SIZE
  syn keyword clConstant CL_DEVICE_GLOBAL_MEM_SIZE
  syn keyword clConstant CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
  syn keyword clConstant CL_DEVICE_MAX_CONSTANT_ARGS
  syn keyword clConstant CL_DEVICE_LOCAL_MEM_TYPE
  syn keyword clConstant CL_DEVICE_LOCAL_MEM_SIZE
  syn keyword clConstant CL_DEVICE_ERROR_CORRECTION_SUPPORT
  syn keyword clConstant CL_DEVICE_PROFILING_TIMER_RESOLUTION
  syn keyword clConstant CL_DEVICE_ENDIAN_LITTLE
  syn keyword clConstant CL_DEVICE_AVAILABLE
  syn keyword clConstant CL_DEVICE_COMPILER_AVAILABLE
  syn keyword clConstant CL_DEVICE_EXECUTION_CAPABILITIES
  syn keyword clConstant CL_DEVICE_QUEUE_PROPERTIES
  syn keyword clConstant CL_DEVICE_NAME
  syn keyword clConstant CL_DEVICE_VENDOR
  syn keyword clConstant CL_DRIVER_VERSION
  syn keyword clConstant CL_DEVICE_PROFILE
  syn keyword clConstant CL_DEVICE_VERSION
  syn keyword clConstant CL_DEVICE_EXTENSIONS
  syn keyword clConstant CL_DEVICE_PLATFORM
  "
  " cl_device_fp_config - bitfield
  syn keyword clConstant CL_FP_DENORM
  syn keyword clConstant CL_FP_INF_NAN
  syn keyword clConstant CL_FP_ROUND_TO_NEAREST
  syn keyword clConstant CL_FP_ROUND_TO_ZERO
  syn keyword clConstant CL_FP_ROUND_TO_INF
  syn keyword clConstant CL_FP_FMA
  "
  " cl_device_mem_cache_type
  syn keyword clConstant CL_NONE
  syn keyword clConstant CL_READ_ONLY_CACHE
  syn keyword clConstant CL_READ_WRITE_CACHE
  "
  " cl_device_local_mem_type
  syn keyword clConstant CL_LOCAL
  syn keyword clConstant CL_GLOBAL
  "
  " cl_device_exec_capabilities - bitfield
  syn keyword clConstant CL_EXEC_KERNEL
  syn keyword clConstant CL_EXEC_NATIVE_KERNEL
  "
  " cl_command_queue_properties - bitfield
  syn keyword clConstant CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
  syn keyword clConstant CL_QUEUE_PROFILING_ENABLE
  "
  " cl_context_info
  syn keyword clConstant CL_CONTEXT_REFERENCE_COUNT
  syn keyword clConstant CL_CONTEXT_DEVICES
  syn keyword clConstant CL_CONTEXT_PROPERTIES
  "
  " cl_context_properties
  syn keyword clConstant CL_CONTEXT_PLATFORM
  "
  " cl_command_queue_info
  syn keyword clConstant CL_QUEUE_CONTEXT
  syn keyword clConstant CL_QUEUE_DEVICE
  syn keyword clConstant CL_QUEUE_REFERENCE_COUNT
  syn keyword clConstant CL_QUEUE_PROPERTIES
  "
  " cl_mem_flags - bitfield
  syn keyword clConstant CL_MEM_READ_WRITE
  syn keyword clConstant CL_MEM_WRITE_ONLY
  syn keyword clConstant CL_MEM_READ_ONLY
  syn keyword clConstant CL_MEM_USE_HOST_PTR
  syn keyword clConstant CL_MEM_ALLOC_HOST_PTR
  syn keyword clConstant CL_MEM_COPY_HOST_PTR
  "
  " cl_channel_order
  syn keyword clConstant CL_R
  syn keyword clConstant CL_A
  syn keyword clConstant CL_RG
  syn keyword clConstant CL_RA
  syn keyword clConstant CL_RGB
  syn keyword clConstant CL_RGBA
  syn keyword clConstant CL_BGRA
  syn keyword clConstant CL_ARGB
  syn keyword clConstant CL_INTENSITY
  syn keyword clConstant CL_LUMINANCE
  "
  " cl_channel_type
  syn keyword clConstant CL_SNORM_INT8
  syn keyword clConstant CL_SNORM_INT16
  syn keyword clConstant CL_UNORM_INT8
  syn keyword clConstant CL_UNORM_INT16
  syn keyword clConstant CL_UNORM_SHORT_565
  syn keyword clConstant CL_UNORM_SHORT_555
  syn keyword clConstant CL_UNORM_INT_101010
  syn keyword clConstant CL_SIGNED_INT8
  syn keyword clConstant CL_SIGNED_INT16
  syn keyword clConstant CL_SIGNED_INT32
  syn keyword clConstant CL_UNSIGNED_INT8
  syn keyword clConstant CL_UNSIGNED_INT16
  syn keyword clConstant CL_UNSIGNED_INT32
  syn keyword clConstant CL_HALF_FLOAT
  syn keyword clConstant CL_FLOAT
  "
  " cl_mem_object_type
  syn keyword clConstant CL_MEM_OBJECT_BUFFER
  syn keyword clConstant CL_MEM_OBJECT_IMAGE2D
  syn keyword clConstant CL_MEM_OBJECT_IMAGE3D
  "
  " cl_mem_info
  syn keyword clConstant CL_MEM_TYPE
  syn keyword clConstant CL_MEM_FLAGS
  syn keyword clConstant CL_MEM_SIZECL_MEM_HOST_PTR
  syn keyword clConstant CL_MEM_HOST_PTR
  syn keyword clConstant CL_MEM_MAP_COUNT
  syn keyword clConstant CL_MEM_REFERENCE_COUNT
  syn keyword clConstant CL_MEM_CONTEXT
  "
  " cl_image_info
  syn keyword clConstant CL_IMAGE_FORMAT
  syn keyword clConstant CL_IMAGE_ELEMENT_SIZE
  syn keyword clConstant CL_IMAGE_ROW_PITCH
  syn keyword clConstant CL_IMAGE_SLICE_PITCH
  syn keyword clConstant CL_IMAGE_WIDTH
  syn keyword clConstant CL_IMAGE_HEIGHT
  syn keyword clConstant CL_IMAGE_DEPTH
  "
  " cl_addressing_mode
  syn keyword clConstant CL_ADDRESS_NONE
  syn keyword clConstant CL_ADDRESS_CLAMP_TO_EDGE
  syn keyword clConstant CL_ADDRESS_CLAMP
  syn keyword clConstant CL_ADDRESS_REPEAT
  "
  " cl_filter_mode
  syn keyword clConstant CL_FILTER_NEAREST
  syn keyword clConstant CL_FILTER_LINEAR
  "
  " cl_sampler_info
  syn keyword clConstant CL_SAMPLER_REFERENCE_COUNT
  syn keyword clConstant CL_SAMPLER_CONTEXT
  syn keyword clConstant CL_SAMPLER_NORMALIZED_COORDS
  syn keyword clConstant CL_SAMPLER_ADDRESSING_MODE
  syn keyword clConstant CL_SAMPLER_FILTER_MODE
  "
  " cl_map_flags - bitfield
  syn keyword clConstant CL_MAP_READ
  syn keyword clConstant CL_MAP_WRITE
  "
  " cl_program_info
  syn keyword clConstant CL_PROGRAM_REFERENCE_COUNT
  syn keyword clConstant CL_PROGRAM_CONTEXT
  syn keyword clConstant CL_PROGRAM_NUM_DEVICES
  syn keyword clConstant CL_PROGRAM_DEVICES
  syn keyword clConstant CL_PROGRAM_SOURCE
  syn keyword clConstant CL_PROGRAM_BINARY_SIZES
  syn keyword clConstant CL_PROGRAM_BINARIES
  "
  " cl_program_build_info
  syn keyword clConstant CL_PROGRAM_BUILD_STATUS
  syn keyword clConstant CL_PROGRAM_BUILD_OPTIONS
  syn keyword clConstant CL_PROGRAM_BUILD_LOG
  "
  " cl_build_status
  syn keyword clConstant CL_BUILD_SUCCESS
  syn keyword clConstant CL_BUILD_NONE
  syn keyword clConstant CL_BUILD_ERROR
  syn keyword clConstant CL_BUILD_IN_PROGRESS
  "
  " cl_kernel_info
  syn keyword clConstant CL_KERNEL_FUNCTION_NAME
  syn keyword clConstant CL_KERNEL_NUM_ARGS
  syn keyword clConstant CL_KERNEL_REFERENCE_COUNT
  syn keyword clConstant CL_KERNEL_CONTEXT
  syn keyword clConstant CL_KERNEL_PROGRAM
  "
  " cl_kernel_work_group_info
  syn keyword clConstant CL_KERNEL_WORK_GROUP_SIZE
  syn keyword clConstant CL_KERNEL_COMPILE_WORK_GROUP_SIZE
  syn keyword clConstant CL_KERNEL_LOCAL_MEM_SIZE
  "
  " cl_event_info
  syn keyword clConstant CL_EVENT_COMMAND_QUEUE
  syn keyword clConstant CL_EVENT_COMMAND_TYPE
  syn keyword clConstant CL_EVENT_REFERENCE_COUNT
  syn keyword clConstant CL_EVENT_COMMAND_EXECUTION_STATUS
  "
  " cl_command_type
  syn keyword clConstant CL_COMMAND_NDRANGE_KERNEL
  syn keyword clConstant CL_COMMAND_TASK
  syn keyword clConstant CL_COMMAND_NATIVE_KERNEL
  syn keyword clConstant CL_COMMAND_READ_BUFFER
  syn keyword clConstant CL_COMMAND_WRITE_BUFFER
  syn keyword clConstant CL_COMMAND_COPY_BUFFER
  syn keyword clConstant CL_COMMAND_READ_IMAGE
  syn keyword clConstant CL_COMMAND_WRITE_IMAGE
  syn keyword clConstant CL_COMMAND_COPY_IMAGE
  syn keyword clConstant CL_COMMAND_COPY_IMAGE_TO_BUFFER
  syn keyword clConstant CL_COMMAND_COPY_BUFFER_TO_IMAGE
  syn keyword clConstant CL_COMMAND_MAP_BUFFER
  syn keyword clConstant CL_COMMAND_MAP_IMAGE
  syn keyword clConstant CL_COMMAND_UNMAP_MEM_OBJECT
  syn keyword clConstant CL_COMMAND_MARKER
  syn keyword clConstant CL_COMMAND_ACQUIRE_GL_OBJECTS
  syn keyword clConstant CL_COMMAND_RELEASE_GL_OBJECTS
  "
  " command execution status
  syn keyword clConstant CL_COMPLETE
  syn keyword clConstant CL_RUNNING
  syn keyword clConstant CL_SUBMITTED
  syn keyword clConstant CL_QUEUED
  "
  " cl_profiling_info
  syn keyword clConstant CL_PROFILING_COMMAND_QUEUED
  syn keyword clConstant CL_PROFILING_COMMAND_SUBMIT
  syn keyword clConstant CL_PROFILING_COMMAND_START
  syn keyword clConstant CL_PROFILING_COMMAND_END
  "
  " from cl_platform.h
  syn keyword clConstant CL_CHAR_BIT
  syn keyword clConstant CL_SCHAR_MAX
  syn keyword clConstant CL_SCHAR_MIN
  syn keyword clConstant CL_CHAR_MAX
  syn keyword clConstant CL_CHAR_MIN
  syn keyword clConstant CL_UCHAR_MAX
  syn keyword clConstant CL_SHRT_MAX
  syn keyword clConstant CL_SHRT_MIN
  syn keyword clConstant CL_USHRT_MAX
  syn keyword clConstant CL_INT_MAX
  syn keyword clConstant CL_INT_MIN
  syn keyword clConstant CL_UINT_MAX
  syn keyword clConstant CL_LONG_MAX
  syn keyword clConstant CL_LONG_MIN
  syn keyword clConstant CL_ULONG_MAX
  "
  syn keyword clConstant CL_FLT_DIG
  syn keyword clConstant CL_FLT_MANT_DIG
  syn keyword clConstant CL_FLT_MAX_10_EXP
  syn keyword clConstant CL_FLT_MAX_EXP
  syn keyword clConstant CL_FLT_MIN_10_EXP
  syn keyword clConstant CL_FLT_MIN_EXP
  syn keyword clConstant CL_FLT_RADIX
  syn keyword clConstant CL_FLT_MAX
  syn keyword clConstant CL_FLT_MIN
  syn keyword clConstant CL_FLT_EPSILON
  "
  syn keyword clConstant CL_DBL_DIG
  syn keyword clConstant CL_DBL_MANT_DIG
  syn keyword clConstant CL_DBL_MAX_10_EXP
  syn keyword clConstant CL_DBL_MAX_EXP
  syn keyword clConstant CL_DBL_MIN_10_EXP
  syn keyword clConstant CL_DBL_MIN_EXP
  syn keyword clConstant CL_DBL_RADIX
  syn keyword clConstant CL_DBL_MAX
  syn keyword clConstant CL_DBL_MIN
  syn keyword clConstant CL_DBL_EPSILON
"
"
" Errors
  syn keyword clConstant CL_SUCCESS
  syn keyword clConstant CL_DEVICE_NOT_FOUND
  syn keyword clConstant CL_DEVICE_NOT_AVAILABLE
  syn keyword clConstant CL_COMPILER_NOT_AVAILABLE
  syn keyword clConstant CL_MEM_OBJECT_ALLOCATION_FAILURE
  syn keyword clConstant CL_OUT_OF_RESOURCES
  syn keyword clConstant CL_OUT_OF_HOST_MEMORY
  syn keyword clConstant CL_PROFILING_INFO_NOT_AVAILABLE
  syn keyword clConstant CL_MEM_COPY_OVERLAP
  syn keyword clConstant CL_IMAGE_FORMAT_MISMATCH
  syn keyword clConstant CL_IMAGE_FORMAT_NOT_SUPPORTED
  syn keyword clConstant CL_BUILD_PROGRAM_FAILURE
  syn keyword clConstant CL_MAP_FAILURE
  "  
  syn keyword clConstant CL_INVALID_VALUE
  syn keyword clConstant CL_INVALID_DEVICE_TYPE
  syn keyword clConstant CL_INVALID_PLATFORM
  syn keyword clConstant CL_INVALID_DEVICE
  syn keyword clConstant CL_INVALID_CONTEXT
  syn keyword clConstant CL_INVALID_QUEUE_PROPERTIES
  syn keyword clConstant CL_INVALID_COMMAND_QUEUE
  syn keyword clConstant CL_INVALID_HOST_PTR
  syn keyword clConstant CL_INVALID_MEM_OBJECT
  syn keyword clConstant CL_INVALID_IMAGE_FORMAT_DESCRIPTOR
  syn keyword clConstant CL_INVALID_IMAGE_SIZE
  syn keyword clConstant CL_INVALID_SAMPLER
  syn keyword clConstant CL_INVALID_BINARY
  syn keyword clConstant CL_INVALID_BUILD_OPTIONS
  syn keyword clConstant CL_INVALID_PROGRAM
  syn keyword clConstant CL_INVALID_PROGRAM_EXECUTABLE
  syn keyword clConstant CL_INVALID_KERNEL_NAME
  syn keyword clConstant CL_INVALID_KERNEL_DEFINITION
  syn keyword clConstant CL_INVALID_KERNEL
  syn keyword clConstant CL_INVALID_ARG_INDEX
  syn keyword clConstant CL_INVALID_ARG_VALUE
  syn keyword clConstant CL_INVALID_ARG_SIZE
  syn keyword clConstant CL_INVALID_KERNEL_ARGS
  syn keyword clConstant CL_INVALID_WORK_DIMENSION
  syn keyword clConstant CL_INVALID_WORK_GROUP_SIZE
  syn keyword clConstant CL_INVALID_WORK_ITEM_SIZE
  syn keyword clConstant CL_INVALID_GLOBAL_OFFSET
  syn keyword clConstant CL_INVALID_EVENT_WAIT_LIST
  syn keyword clConstant CL_INVALID_EVENT
  syn keyword clConstant CL_INVALID_OPERATION
  syn keyword clConstant CL_INVALID_GL_OBJECT
  syn keyword clConstant CL_INVALID_BUFFER_SIZE
  syn keyword clConstant CL_INVALID_MIP_LEVEL
  syn keyword clConstant CL_INVALID_GLOBAL_WORK_SIZE
"
"
" Functions
  "
" Work item Built in Functions
  syn keyword clFunction get_work_dim
  syn keyword clFunction get_global_size
  syn keyword clFunction get_global_id
  syn keyword clFunction get_local_size
  syn keyword clFunction get_local_id
  syn keyword clFunction get_num_groups
  syn keyword clFunction get_group_id
  syn keyword clFunction get_group_offset
" Work item Built in Functions End

"conversions & Typecasting
  syn keyword clFunction convert
  syn keyword clFunction sat
  syn keyword clFunction as
  syn keyword clFunction rte
  syn keyword clFunction rtp
  syn keyword clFunction rtz
  syn keyword clFunction rtn
" conversion & Typecasting Ends

"Interger Built in Functions
  syn keyword clFunction abs
  syn keyword clFunction abs_diff
  syn keyword clFunction add_sat
  syn keyword clFunction hadd
  syn keyword clFunction rhadd
  syn keyword clFunction clamp
  syn keyword clFunction mad_hi
  syn keyword clFunction mad_sat
  syn keyword clFunction max
  syn keyword clFunction min
  syn keyword clFunction mul_hi
  syn keyword clFunction rotate
  syn keyword clFunction upsample
  syn keyword clFunction mad24
  syn keyword clFunction mul24
" Integer Built in functions ends

" Common Builtin function
  syn keyword clFunction clamp
  syn keyword clFunction degrees
  syn keyword clFunction max
  syn keyword clFunction min
  syn keyword clFunction mix
  syn keyword clFunction radian
  syn keyword clFunction step
  syn keyword clFunction smoothstep
  syn keyword clFunction sign
" Common Builtin functions ends

" MATH Builtin functions
  syn keyword clFunction acos
  syn keyword clFunction acosh
  syn keyword clFunction acospi
  syn keyword clFunction asin
  syn keyword clFunction asinh
  syn keyword clFunction asinpi
  syn keyword clFunction atan
  syn keyword clFunction atan2
  syn keyword clFunction atanh
  syn keyword clFunction atanpi
  syn keyword clFunction cbrt
  syn keyword clFunction ceil
  syn keyword clFunction copysign
  syn keyword clFunction cos
  syn keyword clFunction cosh
  syn keyword clFunction cospi
  syn keyword clFunction half_divide
  syn keyword clFunction native_divide
  syn keyword clFunction erfc
  syn keyword clFunction erf
  syn keyword clFunction exp
  syn keyword clFunction exp2
  syn keyword clFunction exp10
  syn keyword clFunction expm1
  syn keyword clFunction fabs
  syn keyword clFunction fdim
  syn keyword clFunction floor
  syn keyword clFunction fma
  syn keyword clFunction fmax
  syn keyword clFunction fmin
  syn keyword clFunction fmod
  syn keyword clFunction fract
  syn keyword clFunction frexp
  syn keyword clFunction hypot
  syn keyword clFunction ilogb
  syn keyword clFunction ldexp
  syn keyword clFunction lgamma
  syn keyword clFunction log
  syn keyword clFunction log2
  syn keyword clFunction log10
  syn keyword clFunction log1p
  syn keyword clFunction logb
  syn keyword clFunction mad
  syn keyword clFunction maxmag
  syn keyword clFunction minmag
  syn keyword clFunction modf
  syn keyword clFunction nan
  syn keyword clFunction nextafter
  syn keyword clFunction pow
  syn keyword clFunction pown
  syn keyword clFunction powr
  syn keyword clFunction half_recip
  syn keyword clFunction native_recip
  syn keyword clFunction remainder
  syn keyword clFunction remquo
  syn keyword clFunction rint
  syn keyword clFunction root
  syn keyword clFunction round
  syn keyword clFunction rsqrt
  syn keyword clFunction sin
  syn keyword clFunction sincos
  syn keyword clFunction sinh
  syn keyword clFunction sinpi
  syn keyword clFunction sqrt
  syn keyword clFunction tan
  syn keyword clFunction tanh
  syn keyword clFunction tanpi
" Math Builtin Functions Ends

"Geometric Buitl int Functions
  syn keyword clFunction dot cross distance length normalize fast_distance fast_length fast_normalize
" Geometric Built in function ends

" Relational Builtin functions
  syn keyword clFunction isequal isnotequal isgreater isgreaterequal isless islessequal islessgreater isfinite isinf isnan isnormal isordered isunordered signbit 
  syn keyword clFunction any all bitselect select
" Relation Builtin functions ends

" Vector Data Load Store Functions
  syn keyword clFunction vloadn vstoren vload_half vload_halfn vstore_half vstore_half_R vstore_halfn vstore_halfn_R vloada_halfn vstorea_halfn vstorea_halfn_R
" Vector Data Load Store Functions ends

" atomic functions
  syn keyword clFunction atomic_add atomic_sub atomic_xchg atomic_inc atomic_dec atomic_cmpxchg atomic_min atomic_max atomic_and atomic_or atomic_xor
" atomic functions ends

" Async copies and Prefetch function
  syn keyword clFunction async_work_group_copy async_work_group_strided_copy wait_group_events prefetch
" Async copies and Prefetch function Ends 

" Miscellanous Vector Builtin functions
  syn keyword clFunction vec_step shuffle shuffle2
" Miscellanous Vector Builtin functions Ends

" Synchronization & Explicit Mem fence
 syn keyword clFunction barrier mem_fence read_mem_fence write_mem_fence
" Synchronization & Explicit Mem fence Ends

" Image Read Write Builtin functions
  syn keyword clFunction read_imagef read_imagei read_image_ui read_imageh write_imagef write_imageui write_imageh get_image_width get_image_height get_image_depth
  syn keyword clFunction get_image_channel_data_type get_image_channel_order get_image_dim  write_imagef write_imagei 
" Image Read Write Builtin functions Ends

" Access Qualifiers
 syn keyword clConstant read_only __read_only write_only __write_only
" Access Qualifier Ends

" address space Qualifiers
  syn keyword clStorageClass global
  syn keyword clStorageClass __global
  syn keyword clStorageClass local
  syn keyword clStorageClass __local
  syn keyword clStorageClass private
  syn keyword clStorageClass __private
  syn keyword clStorageClass constant
  syn keyword clStorageClass __constant
"address qualifiers end

"Function Qualifiers
  syn keyword clStatement kernel
  syn keyword clStatement __kernel
  syn keyword clStatement __attribure__
  syn keyword clStatement vec_type_hint
  syn keyword clStatement work_group_size_hint
  syn keyword clStatement reqd_work_group_size
"Function qualifier end

" Math Constants
  syn keyword clConstant    MAXFLOAT
  syn keyword clConstant    HUGE_VALF
  syn keyword clConstant    HUGE_VAL
  syn keyword clConstant    INFINITY
  syn keyword clConstant    NAN
  syn keyword clConstant    M_E_F
  syn keyword clConstant    M_LOG2E_F
  syn keyword clConstant    M_LOG10E_F
  syn keyword clConstant    M_LN2_F
  syn keyword clConstant    M_LN10_F
  syn keyword clConstant    M_PI_F
  syn keyword clConstant    M_PI_2_F
  syn keyword clConstant    M_PI_4_F
  syn keyword clConstant    M_1_PI_F
  syn keyword clConstant    M_2_PI_F
  syn keyword clConstant    M_2_SQRTPI_F
  syn keyword clConstant    M_SQRT2_F
  syn keyword clConstant    M_SQRT1_2_F
" Math constant End


  " Platform API
  syn keyword clFunction clGetPlatformIDs
  syn keyword clFunction clGetPlatformInfo
  "
  " Device APIs
  syn keyword clFunction clGetDeviceIDs
  syn keyword clFunction clGetDeviceInfo
  "
  " Context APIs
  syn keyword clFunction clCreateContext
  syn keyword clFunction clCreateContextFromType
  syn keyword clFunction clReleaseContext
  syn keyword clFunction clGetContextInfo
  "
  " Command Queue APIs
  syn keyword clFunction clCreateCommandQueue
  syn keyword clFunction clRetainCommandQueue
  syn keyword clFunction clReleaseCommandQueue
  syn keyword clFunction clGetCommandQueueInfo
  syn keyword clFunction clSetCommandQueueProperty
  "
  " Memory Object APIs
  syn keyword clFunction clCreateBuffer
  syn keyword clFunction clCreateImage2D
  syn keyword clFunction clCreateImage3D
  syn keyword clFunction clRetainMemObject
  syn keyword clFunction clReleaseMemObject
  syn keyword clFunction clGetSupportedImageFormats
  syn keyword clFunction clGetMemObjectInfo
  syn keyword clFunction clGetImageInfo
  "
  " Sampler APIs
  syn keyword clFunction clCreateSampler
  syn keyword clFunction clRetainSampler
  syn keyword clFunction clReleaseSampler
  syn keyword clFunction clGetSamplerInfo
  "
  " Program Object APIs
  syn keyword clFunction clCreateProgramWithSource
  syn keyword clFunction clCreateProgramWithBinary
  syn keyword clFunction clRetainProgram
  syn keyword clFunction clReleaseProgram
  syn keyword clFunction clBuildProgram
  syn keyword clFunction clUnloadCompiler
  syn keyword clFunction clGetProgramInfo
  syn keyword clFunction clGetProgramBuildInfo
  "
  " Kernel Object APIs
  syn keyword clFunction clCreateKernel
  syn keyword clFunction clCreateKernelsInProgram
  syn keyword clFunction clRetainKernel
  syn keyword clFunction clReleaseKernel
  syn keyword clFunction clSetKernelArg
  syn keyword clFunction clGetKernelInfo
  syn keyword clFunction clGetKernelWorkGroupInfo
  "
  " Event Object APIs
  syn keyword clFunction clWaitForEvents
  syn keyword clFunction clGetEventInfo
  syn keyword clFunction clRetainEvent
  syn keyword clFunction clReleaseEvent
  "
  " Profiling APIs
  syn keyword clFunction clGetEventProfilingInfo
  "
  " Flush and Finish APIs
  syn keyword clFunction clFlush
  syn keyword clFunction clFinish
  "
  " Enqueued Commands APIs
  syn keyword clFunction clEnqueueReadBuffer
  syn keyword clFunction clEnqueueWriteBuffer
  syn keyword clFunction clEnqueueCopyBuffer
  syn keyword clFunction clEnqueueReadImage
  syn keyword clFunction clEnqueueWriteImage
  syn keyword clFunction clEnqueueCopyImage
  syn keyword clFunction clEnqueueCopyImageToBuffer
  syn keyword clFunction clEnqueueCopyBufferToImage
  syn keyword clFunction clEnqueueMapBuffer
  syn keyword clFunction clEnqueueMapImage
  syn keyword clFunction clEnqueueUnmapMemObject
  syn keyword clFunction clEnqueueNDRangeKernel
  syn keyword clFunction clEnqueueTask
  syn keyword clFunction clEnqueueNativeKernel
  syn keyword clFunction clEnqueueMarker
  syn keyword clFunction clEnqueueWaitForEvents
  syn keyword clFunction clEnqueueBarrier
" 
"
" cl_gl.h
"
"if !exists ("c_opencl_no_gl")
"
" Data Types
  syn keyword clType cl_gl_object_type
  syn keyword clType cl_gl_texture_info
  syn keyword clType cl_gl_platform_info
  syn keyword clType cl_gl_context_info
"
"
" Constants
  syn keyword clConstant CL_GL_OBJECT_BUFFER
  syn keyword clConstant CL_GL_OBJECT_TEXTURE2D
  syn keyword clConstant CL_GL_OBJECT_TEXTURE3D
  syn keyword clConstant CL_GL_OBJECT_RENDERBUFFER
  "
  " cl_gl_object_type 
  syn keyword clConstant CL_GL_TEXTURE_TARGET
  syn keyword clConstant CL_GL_MIPMAP_LEVEL
  "
  " cl_khr_gl_sharing
  syn keyword clConstant CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR
  syn keyword clConstant CL_DEVICES_FOR_GL_CONTEXT_KHR
  syn keyword clConstant CL_GL_CONTEXT_KHR
  syn keyword clConstant CL_EGL_DISPLAY_KHR
  syn keyword clConstant CL_GLX_DISPLAY_KHR
  syn keyword clConstant CL_WGL_HDC_KHR
  syn keyword clConstant CL_CGL_SHAREGROUP_KHR
"
"
" Errors
  syn keyword clConstant CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR
"
"
" Functions  
  syn keyword clFunction clCreateFromGLBuffer
  syn keyword clFunction clCreateFromGLTexture2D
  syn keyword clFunction clCreateFromGLTexture3D
  syn keyword clFunction clCreateFromGLRenderbuffer
  syn keyword clFunction clGetGLObjectInfo
  syn keyword clFunction clGetGLTextureInfo
  syn keyword clFunction clEnqueueAcquireGLObjects
  syn keyword clFunction clEnqueueReleaseGLObjects
  syn keyword clFunction clGetGLContextInfoKHR
"
"endif


" Define the default highlighting.
" Only used when an item doesn't have highlighting yet
hi def link clFunction		Function
hi def link clType		Type
hi def link clConstant 		Constant
hi def link clStatement		Statement
hi def link clStorageClass	StorageClass
hi def link cCppString		cString
hi def link cCommentL		cComment
hi def link cCommentStart	cComment
hi def link cLabel		Label
hi def link cUserLabel		Label
hi def link cConditional	Conditional
hi def link cRepeat		Repeat
hi def link cCharacter		Character
hi def link cSpecialCharacter	cSpecial
hi def link cNumber		Number
hi def link cOctal		Number
hi def link cOctalZero		PreProc	 " link this to Error if you want
hi def link cFloat		Float
hi def link cOctalError		cError
hi def link cParenError		cError
hi def link cErrInParen		cError
hi def link cErrInBracket	cError
hi def link cCommentError	cError
hi def link cCommentStartError	cError
hi def link cSpaceError		cError
hi def link cSpecialError	cError
hi def link cCurlyError		cError
hi def link cOperator		Operator
hi def link cStructure		Structure
hi def link cStorageClass	StorageClass
hi def link cInclude		Include
hi def link cPreProc		PreProc
hi def link cDefine		Macro
hi def link cIncluded		cString
hi def link cError		Error
hi def link cStatement		Statement
hi def link cPreCondit		PreCondit
hi def link cType		Type
hi def link cConstant		Constant
hi def link cCommentString	cString
hi def link cComment2String	cString
hi def link cCommentSkip	cComment
hi def link cString		String
hi def link cComment		Comment
hi def link cSpecial		SpecialChar
hi def link cTodo		Todo
hi def link cBadContinuation	Error
hi def link cCppSkip		cCppOut
hi def link cCppOut2		cCppOut
hi def link cCppOut		Comment


 " vim: fdm=marker:
let b:current_syn = "c"

" vim: ts=8e
