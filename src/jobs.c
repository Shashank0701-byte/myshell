#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "jobs.h"
#include "error.h"

#define MAX_JOBS 100

static job_t jobs[MAX_JOBS];
static int job_count = 0;
static int next_job_id = 1;

void init_jobs(void) {
    job_count = 0;
    next_job_id = 1;
    for (int i = 0; i < MAX_JOBS; i++) {
        jobs[i].job_id = 0;
        jobs[i].pid = 0;
        jobs[i].command = NULL;
        jobs[i].state = JOB_DONE;
        jobs[i].background = 0;
    }
}

int add_job(pid_t pid, const char *command, int background) {
    if (job_count >= MAX_JOBS) {
        fprintf(stderr, "myshell: job table full\n");
        return -1;
    }
    
    // Find empty slot
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id == 0) {
            jobs[i].job_id = next_job_id++;
            jobs[i].pid = pid;
            jobs[i].command = strdup(command);
            jobs[i].state = JOB_RUNNING;
            jobs[i].background = background;
            job_count++;
            
            if (background) {
                printf("[%d] %d\n", jobs[i].job_id, (int)pid);
            }
            
            return jobs[i].job_id;
        }
    }
    
    return -1;
}

void remove_job(int job_id) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id == job_id) {
            free(jobs[i].command);
            jobs[i].job_id = 0;
            jobs[i].pid = 0;
            jobs[i].command = NULL;
            jobs[i].state = JOB_DONE;
            job_count--;
            return;
        }
    }
}

job_t *get_job(int job_id) {
    // If job_id is 0, return most recent job
    if (job_id == 0) {
        int max_id = 0;
        job_t *recent = NULL;
        for (int i = 0; i < MAX_JOBS; i++) {
            if (jobs[i].job_id > 0 && jobs[i].job_id > max_id) {
                max_id = jobs[i].job_id;
                recent = &jobs[i];
            }
        }
        return recent;
    }
    
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id == job_id) {
            return &jobs[i];
        }
    }
    return NULL;
}

job_t *get_job_by_pid(pid_t pid) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].pid == pid && jobs[i].job_id > 0) {
            return &jobs[i];
        }
    }
    return NULL;
}

void update_job_state(int job_id, job_state_t state) {
    job_t *job = get_job(job_id);
    if (job) {
        job->state = state;
    }
}

void list_jobs(void) {
    int found = 0;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id > 0) {
            const char *state_str;
            switch (jobs[i].state) {
                case JOB_RUNNING:
                    state_str = "Running";
                    break;
                case JOB_STOPPED:
                    state_str = "Stopped";
                    break;
                case JOB_DONE:
                    state_str = "Done";
                    break;
                default:
                    state_str = "Unknown";
            }
            
            printf("[%d]%c %s\t\t%s\n",
                   jobs[i].job_id,
                   (i == 0) ? '+' : ' ',  // Mark current job
                   state_str,
                   jobs[i].command);
            found = 1;
        }
    }
    
    if (!found) {
        printf("No jobs\n");
    }
}

#ifndef _WIN32
// POSIX implementation of fg/bg

int fg_job(int job_id) {
    job_t *job = get_job(job_id);
    
    if (!job) {
        fprintf(stderr, "myshell: fg: no such job\n");
        return -1;
    }
    
    printf("%s\n", job->command);
    
    // Continue the process if stopped
    if (job->state == JOB_STOPPED) {
        kill(job->pid, SIGCONT);
    }
    
    job->state = JOB_RUNNING;
    job->background = 0;
    
    // Wait for the job to complete
    int status;
    pid_t result = waitpid(job->pid, &status, WUNTRACED);
    
    if (result > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            // Job completed
            remove_job(job->job_id);
        } else if (WIFSTOPPED(status)) {
            // Job stopped (Ctrl+Z)
            job->state = JOB_STOPPED;
            printf("\n[%d]+  Stopped\t\t%s\n", job->job_id, job->command);
        }
    }
    
    return 0;
}

int bg_job(int job_id) {
    job_t *job = get_job(job_id);
    
    if (!job) {
        fprintf(stderr, "myshell: bg: no such job\n");
        return -1;
    }
    
    if (job->state != JOB_STOPPED) {
        fprintf(stderr, "myshell: bg: job already running\n");
        return -1;
    }
    
    // Continue the process in background
    kill(job->pid, SIGCONT);
    job->state = JOB_RUNNING;
    job->background = 1;
    
    printf("[%d]+ %s &\n", job->job_id, job->command);
    
    return 0;
}

void check_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id > 0 && jobs[i].state == JOB_RUNNING) {
            int status;
            pid_t result = waitpid(jobs[i].pid, &status, WNOHANG | WUNTRACED);
            
            if (result > 0) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    printf("\n[%d]+  Done\t\t%s\n", jobs[i].job_id, jobs[i].command);
                    remove_job(jobs[i].job_id);
                } else if (WIFSTOPPED(status)) {
                    jobs[i].state = JOB_STOPPED;
                    printf("\n[%d]+  Stopped\t\t%s\n", jobs[i].job_id, jobs[i].command);
                }
            }
        }
    }
}

#else
// Windows stubs (limited support)

int fg_job(int job_id) {
    fprintf(stderr, "myshell: fg: job control not supported on Windows\n");
    return -1;
}

int bg_job(int job_id) {
    fprintf(stderr, "myshell: bg: job control not supported on Windows\n");
    return -1;
}

void check_jobs(void) {
    // Windows: Check if processes are still running
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].job_id > 0 && jobs[i].state == JOB_RUNNING) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, jobs[i].pid);
            if (hProcess) {
                DWORD exitCode;
                if (GetExitCodeProcess(hProcess, &exitCode)) {
                    if (exitCode != STILL_ACTIVE) {
                        printf("\n[%d]+  Done\t\t%s\n", jobs[i].job_id, jobs[i].command);
                        remove_job(jobs[i].job_id);
                    }
                }
                CloseHandle(hProcess);
            }
        }
    }
}

#endif

void free_jobs(void) {
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].command) {
            free(jobs[i].command);
            jobs[i].command = NULL;
        }
    }
    job_count = 0;
}
