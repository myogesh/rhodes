/**********************************************************************

  vm_core.h - 

  $Author: yugui $
  created at: 04/01/01 19:41:38 JST

  Copyright (C) 2004-2007 Koichi Sasada

**********************************************************************/

#ifndef RUBY_VM_CORE_H
#define RUBY_VM_CORE_H

#define RUBY_VM_THREAD_MODEL 2

#include "ruby/ruby.h"
#include "ruby/st.h"

#include "node.h"
#include "debug.h"
#include "vm_opts.h"
#include "id.h"

#if   defined(_WIN32)
#include "thread_win32.h"
#elif defined(HAVE_PTHREAD_H)
#include "thread_pthread.h"
#else
#error "unsupported thread type"
#endif

#include <setjmp.h>
#include <signal.h>

#ifndef NSIG
# define NSIG (_SIGMAX + 1)      /* For QNX */
#endif

#define RUBY_NSIG NSIG

#ifdef HAVE_STDARG_PROTOTYPES
#include <stdarg.h>
#define va_init_list(a,b) va_start(a,b)
#else
#include <varargs.h>
#define va_init_list(a,b) va_start(a)
#endif

/*****************/
/* configuration */
/*****************/

/* gcc ver. check */
#if defined(__GNUC__) && __GNUC__ >= 2

#if OPT_TOKEN_THREADED_CODE
#if OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif
#endif

#else /* defined(__GNUC__) && __GNUC__ >= 2 */

/* disable threaded code options */
#if OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif
#if OPT_TOKEN_THREADED_CODE
#undef OPT_TOKEN_THREADED_CODE
#endif
#endif

/* call threaded code */
#if    OPT_CALL_THREADED_CODE
#if    OPT_DIRECT_THREADED_CODE
#undef OPT_DIRECT_THREADED_CODE
#endif /* OPT_DIRECT_THREADED_CODE */
#if    OPT_STACK_CACHING
#undef OPT_STACK_CACHING
#endif /* OPT_STACK_CACHING */
#endif /* OPT_CALL_THREADED_CODE */

/* likely */
#if __GNUC__ >= 3
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else /* __GNUC__ >= 3 */
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif /* __GNUC__ >= 3 */

typedef unsigned long rb_num_t;

struct iseq_compile_data_ensure_node_stack;

typedef struct rb_compile_option_struct {
    int inline_const_cache;
    int peephole_optimization;
    int tailcall_optimization;
    int specialized_instruction;
    int operands_unification;
    int instructions_unification;
    int stack_caching;
    int trace_instruction;
    int debug_level;
} rb_compile_option_t;

#if 1
#define GetCoreDataFromValue(obj, type, ptr) do { \
    ptr = (type*)DATA_PTR(obj); \
} while (0)
#else
#define GetCoreDataFromValue(obj, type, ptr) Data_Get_Struct(obj, type, ptr)
#endif

#define GetISeqPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_iseq_t, ptr)

struct rb_iseq_struct;

struct rb_iseq_struct {
    /***************/
    /* static data */
    /***************/

    VALUE type;          /* instruction sequence type */
    VALUE name;	         /* String: iseq name */
    VALUE filename;      /* file information where this sequence from */
    VALUE *iseq;         /* iseq (insn number and openrads) */
    VALUE *iseq_encoded; /* encoded iseq */
    unsigned long iseq_size;
    VALUE mark_ary;	/* Array: includes operands which should be GC marked */
    VALUE coverage;     /* coverage array */

    /* insn info, must be freed */
    struct iseq_insn_info_entry *insn_info_table;
    unsigned long insn_info_size;

    ID *local_table;		/* must free */
    int local_table_size;

    /* method, class frame: sizeof(vars) + 1, block frame: sizeof(vars) */
    int local_size; 

    /**
     * argument information
     *
     *  def m(a1, a2, ..., aM,                    # mandatory
     *        b1=(...), b2=(...), ..., bN=(...),  # optinal
     *        *c,                                 # rest
     *        d1, d2, ..., dO,                    # post
     *        &e)                                 # block
     * =>
     *
     *  argc           = M
     *  arg_rest       = M+N+1 // or -1 if no rest arg
     *  arg_opts       = N
     *  arg_opts_tbl   = [ (N entries) ]
     *  arg_post_len   = O // 0 if no post arguments
     *  arg_post_start = M+N+2
     *  arg_block      = M+N + 1 + O + 1 // -1 if no block arg
     *  arg_simple     = 0 if not simple arguments.
     *                 = 1 if no opt, rest, post, block.
     *                 = 2 if ambiguos block parameter ({|a|}).
     *  arg_size       = argument size.
     */

    int argc;
    int arg_simple;
    int arg_rest;
    int arg_block;
    int arg_opts;
    int arg_post_len;
    int arg_post_start;
    int arg_size;
    VALUE *arg_opt_table;

    int stack_max; /* for stack overflow check */

    /* catch table */
    struct iseq_catch_table_entry *catch_table;
    int catch_table_size;

    /* for child iseq */
    struct rb_iseq_struct *parent_iseq;
    struct rb_iseq_struct *local_iseq;

    /****************/
    /* dynamic data */
    /****************/

    VALUE self;
    VALUE orig;			/* non-NULL if its data have origin */

    /* block inlining */
    /* 
     * NODE *node;
     * void *special_block_builder;
     * void *cached_special_block_builder;
     * VALUE cached_special_block;
     */

    /* klass/module nest information stack (cref) */
    NODE *cref_stack;
    VALUE klass;

    /* misc */
    ID defined_method_id;	/* for define_method */

    /* used at compile time */
    struct iseq_compile_data *compile_data;
};

enum ruby_special_exceptions {
    ruby_error_reenter,
    ruby_error_nomemory,
    ruby_error_sysstack,
    ruby_special_error_count
};

typedef struct rb_iseq_struct rb_iseq_t;

#define GetVMPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_vm_t, ptr)

typedef struct rb_vm_struct {
    VALUE self;

    rb_thread_lock_t global_vm_lock;

    struct rb_thread_struct *main_thread;
    struct rb_thread_struct *running_thread;

    st_table *living_threads;
    VALUE thgroup_default;

    int running;
    int thread_abort_on_exception;
    unsigned long trace_flag;
    volatile int sleeper;

    /* object management */
    VALUE mark_object_ary;

    VALUE special_exceptions[ruby_special_error_count];

    /* load */
    VALUE top_self;
    VALUE load_path;
    VALUE loaded_features;
    struct st_table *loading_table;
    
    /* signal */
    struct {
	VALUE cmd;
	int safe;
    } trap_list[RUBY_NSIG];

    /* hook */
    rb_event_hook_t *event_hooks;

    int src_encoding_index;

    VALUE verbose, debug, progname;
    VALUE coverages;

#if defined(ENABLE_VM_OBJSPACE) && ENABLE_VM_OBJSPACE
    struct rb_objspace *objspace;
#endif
} rb_vm_t;

typedef struct {
    VALUE *pc;			/* cfp[0] */
    VALUE *sp;			/* cfp[1] */
    VALUE *bp;			/* cfp[2] */
    rb_iseq_t *iseq;		/* cfp[3] */
    VALUE flag;			/* cfp[4] */
    VALUE self;			/* cfp[5] / block[0] */
    VALUE *lfp;			/* cfp[6] / block[1] */
    VALUE *dfp;			/* cfp[7] / block[2] */
    rb_iseq_t *block_iseq;	/* cfp[8] / block[3] */
    VALUE proc;			/* cfp[9] / block[4] */
    ID method_id;               /* cfp[10] saved in special case */
    VALUE method_class;         /* cfp[11] saved in special case */
} rb_control_frame_t;

typedef struct rb_block_struct {
    VALUE self;			/* share with method frame if it's only block */
    VALUE *lfp;			/* share with method frame if it's only block */
    VALUE *dfp;			/* share with method frame if it's only block */
    rb_iseq_t *iseq;
    VALUE proc;
} rb_block_t;

#define GetThreadPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_thread_t, ptr)

enum rb_thread_status {
    THREAD_TO_KILL,
    THREAD_RUNNABLE,
    THREAD_STOPPED,
    THREAD_STOPPED_FOREVER,
    THREAD_KILLED
};

typedef RUBY_JMP_BUF rb_jmpbuf_t;

struct rb_vm_tag {
    rb_jmpbuf_t buf;
    VALUE tag;
    VALUE retval;
    struct rb_vm_tag *prev;
};

struct rb_vm_trap_tag {
    struct rb_vm_trap_tag *prev;
};

#define RUBY_VM_VALUE_CACHE_SIZE 0x1000
#define USE_VALUE_CACHE 0

struct rb_unblock_callback {
    rb_unblock_function_t *func;
    void *arg;
};

struct rb_mutex_struct;

typedef struct rb_thread_struct
{
    VALUE self;
    rb_vm_t *vm;

    /* execution information */
    VALUE *stack;		/* must free, must mark */
    unsigned long stack_size;
    rb_control_frame_t *cfp;
    int safe_level;
    int raised_flag;
    VALUE last_status; /* $? */
    
    /* passing state */
    int state;

    /* for rb_iterate */
    rb_block_t *passed_block;

    /* for load(true) */
    VALUE top_self;
    VALUE top_wrapper;

    /* eval env */
    rb_block_t *base_block;

    VALUE *local_lfp;
    VALUE local_svar;

    /* thread control */
    rb_thread_id_t thread_id;
    enum rb_thread_status status;
    int priority;
    int slice;

    native_thread_data_t native_thread_data;
    void *blocking_region_buffer;

    VALUE thgroup;
    VALUE value;

    VALUE errinfo;
    VALUE thrown_errinfo;
    int exec_signal;

    int interrupt_flag;
    rb_thread_lock_t interrupt_lock;
    struct rb_unblock_callback unblock;
    VALUE locking_mutex;
    struct rb_mutex_struct *keeping_mutexes;
    int transition_for_lock;

    struct rb_vm_tag *tag;
    struct rb_vm_trap_tag *trap_tag;

    int parse_in_eval;
    int mild_compile_error;

    /* storage */
    st_table *local_storage;
#if USE_VALUE_CACHE
    VALUE value_cache[RUBY_VM_VALUE_CACHE_SIZE + 1];
    VALUE *value_cache_ptr;
#endif

    struct rb_thread_struct *join_list_next;
    struct rb_thread_struct *join_list_head;

    VALUE first_proc;
    VALUE first_args;
    VALUE (*first_func)(ANYARGS);

    /* for GC */
    VALUE *machine_stack_start;
    VALUE *machine_stack_end;
    size_t machine_stack_maxsize;
#ifdef __ia64
    VALUE *machine_register_stack_start;
    VALUE *machine_register_stack_end;
    size_t machine_register_stack_maxsize;
#endif
    jmp_buf machine_regs;
    int mark_stack_len;

    /* statistics data for profiler */
    VALUE stat_insn_usage;

    /* tracer */
    rb_event_hook_t *event_hooks;
    rb_event_flag_t event_flags;
    int tracing;

    /* fiber */
    VALUE fiber;
    VALUE root_fiber;
    rb_jmpbuf_t root_jmpbuf;

    /* misc */
    int method_missing_reason;
    int abort_on_exception;
} rb_thread_t;

/* iseq.c */
VALUE rb_iseq_new(NODE*, VALUE, VALUE, VALUE, VALUE);
VALUE rb_iseq_new_top(NODE *node, VALUE name, VALUE filename, VALUE parent);
VALUE rb_iseq_new_main(NODE *node, VALUE filename);
VALUE rb_iseq_new_with_bopt(NODE*, VALUE, VALUE, VALUE, VALUE, VALUE);
VALUE rb_iseq_new_with_opt(NODE*, VALUE, VALUE, VALUE, VALUE, const rb_compile_option_t*);
VALUE rb_iseq_compile(VALUE src, VALUE file, VALUE line);
VALUE rb_iseq_disasm(VALUE self);
VALUE rb_iseq_disasm_insn(VALUE str, VALUE *iseqval, int pos, rb_iseq_t *iseq, VALUE child);
const char *ruby_node_name(int node);
int rb_iseq_first_lineno(rb_iseq_t *iseq);

RUBY_EXTERN VALUE rb_cISeq;
RUBY_EXTERN VALUE rb_cRubyVM;
RUBY_EXTERN VALUE rb_cEnv;
RUBY_EXTERN VALUE rb_mRubyVMFrozenCore;

/* each thread has this size stack : 128KB */
#define RUBY_VM_THREAD_STACK_SIZE (128 * 1024)

struct global_entry {
    struct global_variable *var;
    ID id;
};

#define GetProcPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_proc_t, ptr)

typedef struct {
    rb_block_t block;

    VALUE envval;		/* for GC mark */
    VALUE blockprocval;
    int safe_level;
    int is_from_method;
    int is_lambda;
} rb_proc_t;

#define GetEnvPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_env_t, ptr)

typedef struct {
    VALUE *env;
    int env_size;
    int local_size;
    VALUE prev_envval;		/* for GC mark */
    rb_block_t block;
} rb_env_t;

#define GetBindingPtr(obj, ptr) \
  GetCoreDataFromValue(obj, rb_binding_t, ptr)

typedef struct {
    VALUE env;
} rb_binding_t;


/* used by compile time and send insn */
#define VM_CALL_ARGS_SPLAT_BIT     (0x01 << 1)
#define VM_CALL_ARGS_BLOCKARG_BIT  (0x01 << 2)
#define VM_CALL_FCALL_BIT          (0x01 << 3)
#define VM_CALL_VCALL_BIT          (0x01 << 4)
#define VM_CALL_TAILCALL_BIT       (0x01 << 5)
#define VM_CALL_TAILRECURSION_BIT  (0x01 << 6)
#define VM_CALL_SUPER_BIT          (0x01 << 7)
#define VM_CALL_SEND_BIT           (0x01 << 8)

#define VM_SPECIAL_OBJECT_VMCORE   0x01
#define VM_SPECIAL_OBJECT_CBASE    0x02

#define VM_FRAME_MAGIC_METHOD 0x11
#define VM_FRAME_MAGIC_BLOCK  0x21
#define VM_FRAME_MAGIC_CLASS  0x31
#define VM_FRAME_MAGIC_TOP    0x41
#define VM_FRAME_MAGIC_FINISH 0x51
#define VM_FRAME_MAGIC_CFUNC  0x61
#define VM_FRAME_MAGIC_PROC   0x71
#define VM_FRAME_MAGIC_IFUNC  0x81
#define VM_FRAME_MAGIC_EVAL   0x91
#define VM_FRAME_MAGIC_LAMBDA 0xa1
#define VM_FRAME_MAGIC_MASK_BITS   8
#define VM_FRAME_MAGIC_MASK   (~(~0<<VM_FRAME_MAGIC_MASK_BITS))

#define VM_FRAME_TYPE(cfp) ((cfp)->flag & VM_FRAME_MAGIC_MASK)

/* other frame flag */
#define VM_FRAME_FLAG_PASSED 0x0100


#define RUBYVM_CFUNC_FRAME_P(cfp) \
  (VM_FRAME_TYPE(cfp) == VM_FRAME_MAGIC_CFUNC)


/* inline (method|const) cache */
#define NEW_INLINE_CACHE_ENTRY() NEW_WHILE(Qundef, 0, 0)
#define ic_class  u1.value
#define ic_method u2.node
#define ic_value  u2.value
#define ic_vmstat u3.cnt
typedef NODE *IC;

void rb_vm_change_state(void);

typedef VALUE CDHASH;

#ifndef FUNC_FASTCALL
#define FUNC_FASTCALL(x) x
#endif

typedef rb_control_frame_t *
  (FUNC_FASTCALL(*rb_insn_func_t))(rb_thread_t *, rb_control_frame_t *);

#define GC_GUARDED_PTR(p)     ((VALUE)((VALUE)(p) | 0x01))
#define GC_GUARDED_PTR_REF(p) ((void *)(((VALUE)p) & ~0x03))
#define GC_GUARDED_PTR_P(p)   (((VALUE)p) & 0x01)

#define RUBY_VM_PREVIOUS_CONTROL_FRAME(cfp) (cfp+1)
#define RUBY_VM_NEXT_CONTROL_FRAME(cfp) (cfp-1)
#define RUBY_VM_END_CONTROL_FRAME(th) \
  ((rb_control_frame_t *)((th)->stack + (th)->stack_size))
#define RUBY_VM_VALID_CONTROL_FRAME_P(cfp, ecfp) \
  ((void *)(ecfp) > (void *)(cfp))
#define RUBY_VM_CONTROL_FRAME_STACK_OVERFLOW_P(th, cfp) \
  (!RUBY_VM_VALID_CONTROL_FRAME_P((cfp), RUBY_VM_END_CONTROL_FRAME(th)))

#define RUBY_VM_IFUNC_P(ptr)        (BUILTIN_TYPE(ptr) == T_NODE)
#define RUBY_VM_NORMAL_ISEQ_P(ptr) \
  (ptr && !RUBY_VM_IFUNC_P(ptr))

#define RUBY_VM_CLASS_SPECIAL_P(ptr) (((VALUE)(ptr)) & 0x02)

#define RUBY_VM_GET_BLOCK_PTR_IN_CFP(cfp) ((rb_block_t *)(&(cfp)->self))
#define RUBY_VM_GET_CFP_FROM_BLOCK_PTR(b) \
  ((rb_control_frame_t *)((VALUE *)(b) - 5))

/* VM related object allocate functions */
VALUE rb_thread_alloc(VALUE klass);
VALUE rb_proc_alloc(VALUE klass);

/* for debug */
extern void rb_vmdebug_stack_dump_raw(rb_thread_t *, rb_control_frame_t *);
#define SDR() rb_vmdebug_stack_dump_raw(GET_THREAD(), GET_THREAD()->cfp)
#define SDR2(cfp) rb_vmdebug_stack_dump_raw(GET_THREAD(), (cfp))
void rb_vm_bugreport(void);


/* functions about thread/vm execution */

VALUE rb_iseq_eval(VALUE iseqval);
VALUE rb_iseq_eval_main(VALUE iseqval);
void rb_enable_interrupt(void);
void rb_disable_interrupt(void);
int rb_thread_method_id_and_class(rb_thread_t *th, ID *idp, VALUE *klassp);

VALUE rb_vm_invoke_proc(rb_thread_t *th, rb_proc_t *proc, VALUE self,
			int argc, const VALUE *argv, rb_block_t *blockptr);
VALUE rb_vm_make_proc(rb_thread_t *th, const rb_block_t *block, VALUE klass);
VALUE rb_vm_make_env_object(rb_thread_t *th, rb_control_frame_t *cfp);

void *rb_thread_call_with_gvl(void *(*func)(void *), void *data1);
int ruby_thread_has_gvl_p(void);
rb_control_frame_t *rb_vm_get_ruby_level_next_cfp(rb_thread_t *th, rb_control_frame_t *cfp);

NOINLINE(void rb_gc_save_machine_context(rb_thread_t *));

#define sysstack_error GET_VM()->special_exceptions[ruby_error_sysstack]

/* for thread */

#if RUBY_VM_THREAD_MODEL == 2
RUBY_EXTERN rb_thread_t *ruby_current_thread;
extern rb_vm_t *ruby_current_vm;
//RHO
extern rb_thread_t * ruby_thread_from_native(void);
extern rb_thread_t * __getCurrentThread();

#define GET_VM() ruby_current_vm
#define GET_THREAD() __getCurrentThread()
//ruby_current_thread
//RHO
#define rb_thread_set_current_raw(th) (void)(ruby_current_thread = (th))
#define rb_thread_set_current(th) do { \
    rb_thread_set_current_raw(th); \
    th->vm->running_thread = th; \
} while (0)

#else
#error "unsupported thread model"
#endif

#define RUBY_VM_SET_INTERRUPT(th) ((th)->interrupt_flag |= 0x02)
#define RUBY_VM_SET_TIMER_INTERRUPT(th) ((th)->interrupt_flag |= 0x01)
#define RUBY_VM_SET_FINALIZER_INTERRUPT(th) ((th)->interrupt_flag |= 0x04)
#define RUBY_VM_INTERRUPTED(th) ((th)->interrupt_flag & 0x02)

void rb_thread_execute_interrupts(rb_thread_t *);

#define RUBY_VM_CHECK_INTS_TH(th) do { \
  if (UNLIKELY(th->interrupt_flag)) { \
    rb_thread_execute_interrupts(th); \
  } \
} while (0)

#define RUBY_VM_CHECK_INTS() \
  RUBY_VM_CHECK_INTS_TH(GET_THREAD())

/* tracer */
static inline void
exec_event_hooks(rb_event_hook_t *hook, rb_event_flag_t flag, VALUE self, ID id, VALUE klass)
{
    if (self == rb_mRubyVMFrozenCore) return;
    while (hook) {
	if (flag & hook->flag) {
	    (*hook->func)(flag, hook->data, self, id, klass);
	}
	hook = hook->next;
    }
}

#define EXEC_EVENT_HOOK(th, flag, self, id, klass) do { \
    rb_event_flag_t wait_event__ = th->event_flags; \
    if (UNLIKELY(wait_event__)) { \
	if (wait_event__ & (flag | RUBY_EVENT_VM)) { \
	    VALUE self__ = (self), klass__ = (klass); \
	    ID id__ = (id); \
	    if (wait_event__ & flag) { \
		exec_event_hooks(th->event_hooks, flag, self__, id__, klass__); \
	    } \
	    if (wait_event__ & RUBY_EVENT_VM) { \
		exec_event_hooks(th->vm->event_hooks, flag, self__, id__, klass__); \
	    } \
	} \
    } \
} while (0)

#endif /* RUBY_VM_CORE_H */
