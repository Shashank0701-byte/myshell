#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

// Job states
typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} job_state_t;

// Job structure
typedef struct job {
    int job_id;              // Job number (1, 2, 3, ...)
    pid_t pid;               // Process ID
    char *command;           // Command string
    job_state_t state;       // Current state
    int background;          // 1 if started in background
} job_t;

/**
 * Initialize the job control system
 */
void init_jobs(void);

/**
 * Add a new job to the job list
 * @param pid: Process ID
 * @param command: Command string
 * @param background: 1 if background job
 * @return: Job ID or -1 on error
 */
int add_job(pid_t pid, const char *command, int background);

/**
 * Remove a job from the job list
 * @param job_id: Job ID to remove
 */
void remove_job(int job_id);

/**
 * Get job by job ID
 * @param job_id: Job ID
 * @return: Pointer to job or NULL if not found
 */
job_t *get_job(int job_id);

/**
 * Get job by PID
 * @param pid: Process ID
 * @return: Pointer to job or NULL if not found
 */
job_t *get_job_by_pid(pid_t pid);

/**
 * Update job state
 * @param job_id: Job ID
 * @param state: New state
 */
void update_job_state(int job_id, job_state_t state);

/**
 * List all jobs (jobs command)
 */
void list_jobs(void);

/**
 * Bring job to foreground (fg command)
 * @param job_id: Job ID (0 for most recent)
 * @return: 0 on success, -1 on error
 */
int fg_job(int job_id);

/**
 * Continue job in background (bg command)
 * @param job_id: Job ID (0 for most recent)
 * @return: 0 on success, -1 on error
 */
int bg_job(int job_id);

/**
 * Check for completed jobs and update states
 */
void check_jobs(void);

/**
 * Free all job resources
 */
void free_jobs(void);

#endif // JOBS_H
