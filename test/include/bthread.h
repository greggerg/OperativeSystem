//thread identifier -> a reference to private data
typedef unsigned long int bthread_t;
//bthread_attr_t structure stores additional thread attributes. This structure is not actually used in our implementation
typedef struct {} bthread_attr_t;
//execution state
typedef enum { __BTHREAD_READY = 0, __BTHREAD_BLOCKED, __BTHREAD_SLEEPING,__BTHREAD_ZOMBIE} bthread_state;
//bthread_routine type specifies the signature of the thread body routine
typedef void *(*bthread_routine) (void *);

int bthread_create(bthread_t *bthread, const bthread_attr_t *attr,void *(*start_routine) (void *), void *arg);

 int bthread_join(bthread_t bthread, void **retval);

 void bthread_yield();

 void bthread_exit(void *retval);
