#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define FILE_NAME "todo.txt"
#define MAX_LINE 256

static void usage(void) {
    printf("Usage:\n");
    printf("  todo add <task>     Add a new task\n");
    printf("  todo list           List all tasks\n");
    printf("  todo done <n>       Mark task n as done\n");
    printf("  todo del <n>        Delete task n\n");
    printf("  todo clear          Delete all tasks\n");
}

static void trim(char *s) {
    char *start = s;
    char *end;
    while (isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);
    end = s + strlen(s);
    while (end > s && isspace((unsigned char)end[-1])) *--end = '\0';
}

static char *read_line(FILE *fp) {
    static char buf[MAX_LINE];
    if (!fgets(buf, sizeof buf, fp)) return NULL;
    buf[strcspn(buf, "\r\n")] = '\0';
    return buf;
}

static int task_count(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    int n = 0;
    char *line;
    if (!fp) return 0;
    while ((line = read_line(fp))) {
        if (line[0] == '[') n++;
    }
    fclose(fp);
    return n;
}

static int cmd_add(const char *desc) {
    FILE *fp = fopen(FILE_NAME, "a");
    if (!fp) {
        fprintf(stderr, "Error: cannot open %s\n", FILE_NAME);
        return 1;
    }
    fprintf(fp, "[ ] %s\n", desc);
    fclose(fp);
    printf("Added: %s\n", desc);
    return 0;
}

static int cmd_list(void) {
    FILE *fp = fopen(FILE_NAME, "r");
    char *line;
    int n = 0;
    if (!fp) {
        printf("No tasks yet. Add one with: todo add \"my task\"\n");
        return 0;
    }
    while ((line = read_line(fp))) {
        if (line[0] != '[') continue;
        n++;
        if (line[1] == 'x')
            printf("%2d. [x] %s\n", n, line + 4);
        else
            printf("%2d. [ ] %s\n", n, line + 4);
    }
    fclose(fp);
    if (n == 0)
        printf("No tasks yet. Add one with: todo add \"my task\"\n");
    return 0;
}

static int rewrite_excluding(int target, int mark_done) {
    FILE *in = fopen(FILE_NAME, "r");
    FILE *out = fopen(FILE_NAME ".tmp", "w");
    char *line;
    int n = 0;
    if (!in || !out) {
        if (in) fclose(in);
        if (out) fclose(out);
        return -1;
    }
    while ((line = read_line(in))) {
        if (line[0] != '[') continue;
        n++;
        if (n == target) {
            if (mark_done)
                fprintf(out, "[x] %s\n", line + 4);
            continue; /* delete: skip writing */
        }
        fprintf(out, "%s\n", line);
    }
    fclose(in);
    fclose(out);
    /* rename() cannot overwrite an existing file on Windows */
    remove(FILE_NAME);
    if (rename(FILE_NAME ".tmp", FILE_NAME) != 0) {
        remove(FILE_NAME ".tmp");
        return -1;
    }
    return n;
}

static int parse_index(const char *arg, int *idx) {
    char *end;
    long v = strtol(arg, &end, 10);
    if (*end != '\0' || v < 1 || v > task_count()) return -1;
    *idx = (int)v;
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        usage();
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Error: missing task description\n");
            return 1;
        }
        /* join all remaining args so `todo add buy milk` works */
        char desc[MAX_LINE] = "";
        for (int i = 2; i < argc; i++) {
            if (i > 2) strncat(desc, " ", sizeof(desc) - strlen(desc) - 1);
            strncat(desc, argv[i], sizeof(desc) - strlen(desc) - 1);
        }
        trim(desc);
        if (!*desc) {
            fprintf(stderr, "Error: empty task description\n");
            return 1;
        }
        return cmd_add(desc);
    }

    if (strcmp(argv[1], "list") == 0 || strcmp(argv[1], "ls") == 0)
        return cmd_list();

    if (strcmp(argv[1], "done") == 0 || strcmp(argv[1], "del") == 0 ||
        strcmp(argv[1], "clear") == 0) {
        int mark_done = strcmp(argv[1], "done") == 0;
        if (strcmp(argv[1], "clear") == 0) {
            FILE *fp = fopen(FILE_NAME, "w");
            if (fp) fclose(fp);
            printf("All tasks cleared.\n");
            return 0;
        }
        if (argc < 3) {
            fprintf(stderr, "Error: missing task number\n");
            return 1;
        }
        int idx;
        if (parse_index(argv[2], &idx) != 0) {
            fprintf(stderr, "Error: invalid task number '%s'\n", argv[2]);
            return 1;
        }
        int total = rewrite_excluding(idx, mark_done);
        if (total < 0) {
            fprintf(stderr, "Error: could not update %s\n", FILE_NAME);
            return 1;
        }
        printf("%s task %d.\n", mark_done ? "Completed" : "Deleted", idx);
        return 0;
    }

    fprintf(stderr, "Error: unknown command '%s'\n", argv[1]);
    usage();
    return 1;
}
