/***** structs *****/
#pragma pack( push, LFDS_ALIGN_DOUBLE_POINTER )

/***** abstraction tests *****/
struct abstraction_test_cas_state
{
  volatile lfds_atom_t
    *shared_counter;

  lfds_atom_t
    local_counter;
};

struct abstraction_test_dcas_state
{
  volatile lfds_atom_t
    *shared_counter;

  lfds_atom_t
    local_counter;
};

/***** freelist tests *****/
struct freelist_test_popping_state
{
  struct lfds_freelist_state
    *fs,
    *fs_thread_local;
};

struct freelist_test_pushing_state
{
  lfds_atom_t
    *count,
    thread_number;

  struct lfds_freelist_state
    *source_fs,
    *fs;
};

struct freelist_test_popping_and_pushing_state
{
  struct lfds_freelist_state
    *local_fs,
    *fs;
};

struct freelist_test_counter_and_thread_number
{
  lfds_atom_t
    thread_number;

  unsigned long long int
    counter;
};

/***** queue tests *****/
struct queue_test_enqueuing_state
{
  struct lfds_queue_state
    *qs;

  lfds_atom_t
    counter;
};

struct queue_test_dequeuing_state
{
  struct lfds_queue_state
    *qs;

  int
    error_flag;
};

struct queue_test_enqueuing_and_dequeuing_state
{
  struct lfds_queue_state
    *qs;

  lfds_atom_t
    counter,
    thread_number,
    *per_thread_counters;

  unsigned int
    cpu_count;

  int
    error_flag;
};

struct queue_test_rapid_enqueuing_and_dequeuing_state
{
  struct lfds_queue_state
    *qs;

  lfds_atom_t
    counter;
};

/***** ringbuffer tests *****/
struct ringbuffer_test_reading_state
{
  struct lfds_ringbuffer_state
    *rs;

  int
    error_flag;

  lfds_atom_t
    read_count;
};

struct ringbuffer_test_writing_state
{
  struct lfds_ringbuffer_state
    *rs;

  lfds_atom_t
    write_count;
};

struct ringbuffer_test_reading_and_writing_state
{
  struct lfds_ringbuffer_state
    *rs;

  lfds_atom_t
    counter,
    *per_thread_counters;

  unsigned int
    cpu_count;

  int
    error_flag;
};

/***** slist tests *****/
struct slist_test_state
{
  struct lfds_slist_state
    *ss;

  size_t
    create_count,
    delete_count;

  lfds_atom_t
    thread_and_count;
};

/***** stack tests *****/
struct stack_test_popping_state
{
  struct lfds_stack_state
    *ss,
    *ss_thread_local;
};

struct stack_test_pushing_state
{
  lfds_atom_t
    thread_number;

  struct lfds_stack_state
    *ss;
};

struct stack_test_popping_and_pushing_state
{
  struct lfds_stack_state
    *local_ss,
    *ss;
};

struct stack_test_counter_and_thread_number
{
  lfds_atom_t
    thread_number,
    counter;
};

/***** freelist benchmarks *****/
struct lfds_freelist_benchmark
{
  struct lfds_freelist_state
    *fs;

  lfds_atom_t
    operation_count;
};

/***** queue benchmarks *****/
struct lfds_queue_benchmark
{
  struct lfds_queue_state
    *qs;

  lfds_atom_t
    operation_count;
};

/***** ringbuffer benchmarks *****/
struct lfds_ringbuffer_benchmark
{
  struct lfds_ringbuffer_state
    *rs;

  lfds_atom_t
    operation_count;
};

/***** stack benchmarks *****/
struct lfds_stack_benchmark
{
  struct lfds_stack_state
    *ss;

  lfds_atom_t
    operation_count;
};

#pragma pack( pop )

