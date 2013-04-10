
 #ifndef _ASM_MUTEX_H
 #define _ASM_MUTEX_H
 
 #if __LINUX_ARM_ARCH__ < 6
 # include <asm-generic/mutex-xchg.h>
 #else
 

 static inline void
 __mutex_fastpath_lock(atomic_t *count, void (*fail_fn)(atomic_t *))
 {
@@ -69,11 +51,6 @@
 	return __res;
 }
 

 static inline void
 __mutex_fastpath_unlock(atomic_t *count, void (*fail_fn)(atomic_t *))
 {
@@ -95,19 +72,8 @@
 		fail_fn(count);
 }
 

 #define __mutex_slowpath_needs_to_unlock()	1
 

 static inline int
 __mutex_fastpath_trylock(atomic_t *count, int (*fail_fn)(atomic_t *))
 {
