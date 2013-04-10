#ifndef _LINUX_CGROUP_H
#define _LINUX_CGROUP_H
/*
 *  cgroup interface
 *
 *  Copyright (C) 2003 BULL SA
 *  Copyright (C) 2004-2006 Silicon Graphics, Inc.
 *
 */

#include <linux/sched.h>
#include <linux/cpumask.h>
#include <linux/nodemask.h>
#include <linux/rcupdate.h>
#include <linux/cgroupstats.h>
#include <linux/prio_heap.h>
#include <linux/rwsem.h>
#include <linux/idr.h>

#ifdef CONFIG_CGROUPS

struct cgroupfs_root;
struct cgroup_subsys;
struct inode;
struct cgroup;
struct css_id;

extern int cgroup_init_early(void);
extern int cgroup_init(void);
extern void cgroup_lock(void);
extern int cgroup_lock_is_held(void);
extern bool cgroup_lock_live_group(struct cgroup *cgrp);
extern void cgroup_unlock(void);
extern void cgroup_fork(struct task_struct *p);
extern void cgroup_fork_callbacks(struct task_struct *p);
extern void cgroup_post_fork(struct task_struct *p);
extern void cgroup_exit(struct task_struct *p, int run_callbacks);
extern int cgroupstats_build(struct cgroupstats *stats,
				struct dentry *dentry);
extern int cgroup_load_subsys(struct cgroup_subsys *ss);
extern void cgroup_unload_subsys(struct cgroup_subsys *ss);

extern const struct file_operations proc_cgroup_operations;

/* Define the enumeration of all builtin cgroup subsystems */
#define SUBSYS(_x) _x ## _subsys_id,
enum cgroup_subsys_id {
#include <linux/cgroup_subsys.h>
	CGROUP_BUILTIN_SUBSYS_COUNT
};
#undef SUBSYS
/*
 * This define indicates the maximum number of subsystems that can be loaded
 * at once. We limit to this many since cgroupfs_root has subsys_bits to keep
 * track of all of them.
 */
#define CGROUP_SUBSYS_COUNT (BITS_PER_BYTE*sizeof(unsigned long))

/* Per-subsystem/per-cgroup state maintained by the system. */
struct cgroup_subsys_state {
	/*
	 * The cgroup that this subsystem is attached to. Useful
	 * for subsystems that want to know about the cgroup
	 * hierarchy structure
	 */
	struct cgroup *cgroup;

	/*
	 * State maintained by the cgroup system to allow subsystems
	 * to be "busy". Should be accessed via css_get(),
	 * css_tryget() and and css_put().
	 */

	atomic_t refcnt;

	unsigned long flags;
	/* ID for this css, if possible */
	struct css_id __rcu *id;
};

/* bits in struct cgroup_subsys_state flags field */
enum {
	CSS_ROOT, /* This CSS is the root of the subsystem */
	CSS_REMOVED, /* This CSS is dead */
};

/* Caller must verify that the css is not for root cgroup */
static inline void __css_get(struct cgroup_subsys_state *css, int count)
{
	atomic_add(count, &css->refcnt);
}

/*
 * Call css_get() to hold a reference on the css; it can be used
 * for a reference obtained via:
 * - an existing ref-counted reference to the css
 * - task->cgroups for a locked task
 */

static inline void css_get(struct cgroup_subsys_state *css)
{
	/* We don't need to reference count the root state */
	if (!test_bit(CSS_ROOT, &css->flags))
		__css_get(css, 1);
}

static inline bool css_is_removed(struct cgroup_subsys_state *css)
{
	return test_bit(CSS_REMOVED, &css->flags);
}


static inline bool css_tryget(struct cgroup_subsys_state *css)
{
	if (test_bit(CSS_ROOT, &css->flags))
		return true;
	while (!atomic_inc_not_zero(&css->refcnt)) {
		if (test_bit(CSS_REMOVED, &css->flags))
			return false;
		cpu_relax();
	}
	return true;
}


extern void __css_put(struct cgroup_subsys_state *css, int count);
static inline void css_put(struct cgroup_subsys_state *css)
{
	if (!test_bit(CSS_ROOT, &css->flags))
		__css_put(css, 1);
}

/* bits in struct cgroup flags field */
enum {
	/* Control Group is dead */
	CGRP_REMOVED,
	/*
	 * Control Group has previously had a child cgroup or a task,
	 * but no longer (only if CGRP_NOTIFY_ON_RELEASE is set)
	 */
	CGRP_RELEASABLE,
	/* Control Group requires release notifications to userspace */
	CGRP_NOTIFY_ON_RELEASE,
	/*
	 * A thread in rmdir() is wating for this cgroup.
	 */
	CGRP_WAIT_ON_RMDIR,
	/*
	 * Clone cgroup values when creating a new child cgroup
	 */
	CGRP_CLONE_CHILDREN,
};

struct cgroup {
	unsigned long flags;		/* "unsigned long" so bitops work */

	/*
	 * count users of this cgroup. >0 means busy, but doesn't
	 * necessarily indicate the number of tasks in the cgroup
	 */
	atomic_t count;

	/*
	 * We link our 'sibling' struct into our parent's 'children'.
	 * Our children link their 'sibling' into our 'children'.
	 */
	struct list_head sibling;	/* my parent's children */
	struct list_head children;	/* my children */

	struct cgroup *parent;		/* my parent */
	struct dentry __rcu *dentry;	/* cgroup fs entry, RCU protected */

	/* Private pointers for each registered subsystem */
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_COUNT];

	struct cgroupfs_root *root;
	struct cgroup *top_cgroup;

	/*
	 * List of cg_cgroup_links pointing at css_sets with
	 * tasks in this cgroup. Protected by css_set_lock
	 */
	struct list_head css_sets;

	/*
	 * Linked list running through all cgroups that can
	 * potentially be reaped by the release agent. Protected by
	 * release_list_lock
	 */
	struct list_head release_list;

	/*
	 * list of pidlists, up to two for each namespace (one for procs, one
	 * for tasks); created on demand.
	 */
	struct list_head pidlists;
	struct mutex pidlist_mutex;

	/* For RCU-protected deletion */
	struct rcu_head rcu_head;

	/* List of events which userspace want to receive */
	struct list_head event_list;
	spinlock_t event_list_lock;
};

/*
 * A css_set is a structure holding pointers to a set of
 * cgroup_subsys_state objects. This saves space in the task struct
 * object and speeds up fork()/exit(), since a single inc/dec and a
 * list_add()/del() can bump the reference count on the entire cgroup
 * set for a task.
 */

struct css_set {

	/* Reference count */
	atomic_t refcount;

	/*
	 * List running through all cgroup groups in the same hash
	 * slot. Protected by css_set_lock
	 */
	struct hlist_node hlist;

	/*
	 * List running through all tasks using this cgroup
	 * group. Protected by css_set_lock
	 */
	struct list_head tasks;

	/*
	 * List of cg_cgroup_link objects on link chains from
	 * cgroups referenced from this css_set. Protected by
	 * css_set_lock
	 */
	struct list_head cg_links;

	/*
	 * Set of subsystem states, one for each subsystem. This array
	 * is immutable after creation apart from the init_css_set
	 * during subsystem registration (at boot time) and modular subsystem
	 * loading/unloading.
	 */
	struct cgroup_subsys_state *subsys[CGROUP_SUBSYS_COUNT];

	/* For RCU-protected deletion */
	struct rcu_head rcu_head;
};

/*
 * cgroup_map_cb is an abstract callback API for reporting map-valued
 * control files
 */

struct cgroup_map_cb {
	int (*fill)(struct cgroup_map_cb *cb, const char *key, u64 value);
	void *state;
};

/*
 * struct cftype: handler definitions for cgroup control files
 *
 * When reading/writing to a file:
 *	- the cgroup to use is file->f_dentry->d_parent->d_fsdata
 *	- the 'cftype' of the file is file->f_dentry->d_fsdata
 */

#define MAX_CFTYPE_NAME 64
struct cftype {
	/*
	 * By convention, the name should begin with the name of the
	 * subsystem, followed by a period
	 */
	char name[MAX_CFTYPE_NAME];
	int private;
	/*
	 * If not 0, file mode is set to this value, otherwise it will
	 * be figured out automatically
	 */
	umode_t mode;

	/*
	 * If non-zero, defines the maximum length of string that can
	 * be passed to write_string; defaults to 64
	 */
	size_t max_write_len;

	int (*open)(struct inode *inode, struct file *file);
	ssize_t (*read)(struct cgroup *cgrp, struct cftype *cft,
			struct file *file,
			char __user *buf, size_t nbytes, loff_t *ppos);
	/*
	 * read_u64() is a shortcut for the common case of returning a
	 * single integer. Use it in place of read()
	 */
	u64 (*read_u64)(struct cgroup *cgrp, struct cftype *cft);
	/*
	 * read_s64() is a signed version of read_u64()
	 */
	s64 (*read_s64)(struct cgroup *cgrp, struct cftype *cft);
	/*
	 * read_map() is used for defining a map of key/value
	 * pairs. It should call cb->fill(cb, key, value) for each
	 * entry. The key/value pairs (and their ordering) should not
	 * change between reboots.
	 */
	int (*read_map)(struct cgroup *cont, struct cftype *cft,
			struct cgroup_map_cb *cb);
	/*
	 * read_seq_string() is used for outputting a simple sequence
	 * using seqfile.
	 */
	int (*read_seq_string)(struct cgroup *cont, struct cftype *cft,
			       struct seq_file *m);

	ssize_t (*write)(struct cgroup *cgrp, struct cftype *cft,
			 struct file *file,
			 const char __user *buf, size_t nbytes, loff_t *ppos);

	int (*write_u64)(struct cgroup *cgrp, struct cftype *cft, u64 val);
	int (*write_s64)(struct cgroup *cgrp, struct cftype *cft, s64 val);

	int (*write_string)(struct cgroup *cgrp, struct cftype *cft,
			    const char *buffer);
	/*
	 * trigger() callback can be used to get some kick from the
	 * userspace, when the actual string written is not important
	 * at all. The private field can be used to determine the
	 * kick type for multiplexing.
	 */
	int (*trigger)(struct cgroup *cgrp, unsigned int event);

	int (*release)(struct inode *inode, struct file *file);

	int (*register_event)(struct cgroup *cgrp, struct cftype *cft,
			struct eventfd_ctx *eventfd, const char *args);
	void (*unregister_event)(struct cgroup *cgrp, struct cftype *cft,
			struct eventfd_ctx *eventfd);
};

struct cgroup_scanner {
	struct cgroup *cg;
	int (*test_task)(struct task_struct *p, struct cgroup_scanner *scan);
	void (*process_task)(struct task_struct *p,
			struct cgroup_scanner *scan);
	struct ptr_heap *heap;
	void *data;
};

int cgroup_add_file(struct cgroup *cgrp, struct cgroup_subsys *subsys,
		       const struct cftype *cft);

int cgroup_add_files(struct cgroup *cgrp,
			struct cgroup_subsys *subsys,
			const struct cftype cft[],
			int count);

int cgroup_is_removed(const struct cgroup *cgrp);

int cgroup_path(const struct cgroup *cgrp, char *buf, int buflen);

int cgroup_task_count(const struct cgroup *cgrp);

int cgroup_is_descendant(const struct cgroup *cgrp, struct task_struct *task);


void cgroup_exclude_rmdir(struct cgroup_subsys_state *css);
void cgroup_release_and_wakeup_rmdir(struct cgroup_subsys_state *css);

struct cgroup_taskset;
struct task_struct *cgroup_taskset_first(struct cgroup_taskset *tset);
struct task_struct *cgroup_taskset_next(struct cgroup_taskset *tset);
struct cgroup *cgroup_taskset_cur_cgroup(struct cgroup_taskset *tset);
int cgroup_taskset_size(struct cgroup_taskset *tset);

#define cgroup_taskset_for_each(task, skip_cgrp, tset)			\
	for ((task) = cgroup_taskset_first((tset)); (task);		\
	     (task) = cgroup_taskset_next((tset)))			\
		if (!(skip_cgrp) ||					\
		    cgroup_taskset_cur_cgroup((tset)) != (skip_cgrp))


struct cgroup_subsys {
	struct cgroup_subsys_state *(*create)(struct cgroup *cgrp);
	int (*pre_destroy)(struct cgroup *cgrp);
	void (*destroy)(struct cgroup *cgrp);
	int (*allow_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	int (*can_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*cancel_attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*attach)(struct cgroup *cgrp, struct cgroup_taskset *tset);
	void (*fork)(struct task_struct *task);
	void (*exit)(struct cgroup *cgrp, struct cgroup *old_cgrp,
		     struct task_struct *task);
	int (*populate)(struct cgroup_subsys *ss, struct cgroup *cgrp);
	void (*post_clone)(struct cgroup *cgrp);
	void (*bind)(struct cgroup *root);

	int subsys_id;
	int active;
	int disabled;
	int early_init;
	bool use_id;
#define MAX_CGROUP_TYPE_NAMELEN 32
	const char *name;

	struct mutex hierarchy_mutex;
	struct lock_class_key subsys_key;

	struct cgroupfs_root *root;
	struct list_head sibling;
	
	struct idr idr;
	spinlock_t id_lock;

	
	struct module *module;
};

#define SUBSYS(_x) extern struct cgroup_subsys _x ## _subsys;
#include <linux/cgroup_subsys.h>
#undef SUBSYS

static inline struct cgroup_subsys_state *cgroup_subsys_state(
	struct cgroup *cgrp, int subsys_id)
{
	return cgrp->subsys[subsys_id];
}

#define task_subsys_state_check(task, subsys_id, __c)			\
	rcu_dereference_check(task->cgroups->subsys[subsys_id],		\
			      lockdep_is_held(&task->alloc_lock) ||	\
			      cgroup_lock_is_held() || (__c))

static inline struct cgroup_subsys_state *
task_subsys_state(struct task_struct *task, int subsys_id)
{
	return task_subsys_state_check(task, subsys_id, false);
}

static inline struct cgroup* task_cgroup(struct task_struct *task,
					       int subsys_id)
{
	return task_subsys_state(task, subsys_id)->cgroup;
}

struct cgroup_iter {
	struct list_head *cg_link;
	struct list_head *task;
};

void cgroup_iter_start(struct cgroup *cgrp, struct cgroup_iter *it);
struct task_struct *cgroup_iter_next(struct cgroup *cgrp,
					struct cgroup_iter *it);
void cgroup_iter_end(struct cgroup *cgrp, struct cgroup_iter *it);
int cgroup_scan_tasks(struct cgroup_scanner *scan);
int cgroup_attach_task(struct cgroup *, struct task_struct *);
int cgroup_attach_task_all(struct task_struct *from, struct task_struct *);


void free_css_id(struct cgroup_subsys *ss, struct cgroup_subsys_state *css);


struct cgroup_subsys_state *css_lookup(struct cgroup_subsys *ss, int id);

struct cgroup_subsys_state *css_get_next(struct cgroup_subsys *ss, int id,
		struct cgroup_subsys_state *root, int *foundid);

bool css_is_ancestor(struct cgroup_subsys_state *cg,
		     const struct cgroup_subsys_state *root);

unsigned short css_id(struct cgroup_subsys_state *css);
unsigned short css_depth(struct cgroup_subsys_state *css);
struct cgroup_subsys_state *cgroup_css_from_dir(struct file *f, int id);

#else 

static inline int cgroup_init_early(void) { return 0; }
static inline int cgroup_init(void) { return 0; }
static inline void cgroup_fork(struct task_struct *p) {}
static inline void cgroup_fork_callbacks(struct task_struct *p) {}
static inline void cgroup_post_fork(struct task_struct *p) {}
static inline void cgroup_exit(struct task_struct *p, int callbacks) {}

static inline void cgroup_lock(void) {}
static inline void cgroup_unlock(void) {}
static inline int cgroupstats_build(struct cgroupstats *stats,
					struct dentry *dentry)
{
	return -EINVAL;
}

static inline int cgroup_attach_task_all(struct task_struct *from,
					 struct task_struct *t)
{
	return 0;
}

#endif 

#endif 
