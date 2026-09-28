*
 * Campus Attendance Assistant - Student Records (C version)
 * - Stores 15 students with attendance in 4 subjects
 * - Fetches one student by roll number and shows full attendance details
 * - Exam eligibility: eligible if and only if overall attendance >= 75%
 *
 * Compile:  gcc attendance_students.c -o attendance
 * Run:      ./attendance      (Windows: attendance.exe)
 */
#include <stdio.h>
#include <string.h>
 
#define NUM_STUDENTS 15
#define NUM_SUBJECTS 4
#define NAME_LEN 50
#define REQUIRED 75          /* minimum attendance % for exam eligibility */
 
typedef struct {
    int roll;
    char name[NAME_LEN];
    int attended[NUM_SUBJECTS];
} Student;
 
/* Subject names and total classes held so far (same for every student) */
static const char *SUBJECT[NUM_SUBJECTS] = {"Mathematics", "Physics", "Chemistry", "English"};
static const int TOTAL[NUM_SUBJECTS]     = {42, 40, 38, 30};
 
/* ---------- student database (15 students) ---------- */
static const Student students[NUM_STUDENTS] = {
    {101, "Aarav Nair",       {38, 36, 35, 27}},
    {102, "Meera Menon",      {34, 28, 36, 20}},
    {103, "Rahul Varma",      {30, 26, 28, 22}},
    {104, "Sneha Pillai",     {40, 38, 37, 29}},
    {105, "Arjun Das",        {32, 30, 29, 22}},
    {106, "Fathima Rahman",   {36, 33, 31, 25}},
    {107, "Vishnu Kumar",     {25, 22, 24, 18}},
    {108, "Anjali Thomas",    {39, 35, 34, 26}},
    {109, "Nikhil Joseph",    {31, 27, 30, 23}},
    {110, "Diya Sharma",      {41, 39, 36, 29}},
    {111, "Kiran Mohan",      {28, 31, 33, 21}},
    {112, "Lakshmi Iyer",     {35, 32, 32, 24}},
    {113, "Adithya Raj",      {22, 25, 20, 15}},
    {114, "Hana Basheer",     {37, 34, 35, 26}},
    {115, "Jibin George",     {33, 29, 31, 24}}
};
 
/* ---------- input helpers ---------- */
static void read_line(char *buf, int size) {
    if (fgets(buf, size, stdin) == NULL) { buf[0] = '\0'; return; }
    buf[strcspn(buf, "\n")] = '\0';
}
 
static int read_int(const char *prompt, int min, int max) {
    char line[64];
    int v;
    while (1) {
        printf("%s", prompt);
        read_line(line, sizeof line);
        if (sscanf(line, "%d", &v) == 1 && v >= min && v <= max) return v;
        printf("  Please enter a number between %d and %d.\n", min, max);
    }
}
 
/* ---------- attendance math (integer only, no rounding errors) ---------- */
static int meets(int a, int t)   { return a * 100 >= REQUIRED * t; }
static int is_safe(int a, int t) { return a * 100 >= (REQUIRED + 5) * t; }
 
/* Classes that can still be missed while staying at or above the requirement */
static int can_skip(int a, int t) {
    int n = 0;
    while (n < 1000 && a * 100 >= REQUIRED * (t + n + 1)) n++;
    return n;
}
 
/* Consecutive classes to attend to reach the requirement */
static int classes_needed(int a, int t) {
    int n = 0;
    while (n < 100000 && (a + n) * 100 < REQUIRED * (t + n)) n++;
    return n;
}
 
static const char *status_text(int a, int t) {
    if (is_safe(a, t)) return "Safe";
    if (meets(a, t))   return "Borderline";
    return "Shortage";
}
 
static void draw_bar(double pct) {
    int i, filled = (int)(pct / 5.0);          /* 20-char bar */
    if (filled > 20) filled = 20;
    printf("[");
    for (i = 0; i < 20; i++) putchar(i < filled ? '#' : '-');
    printf("]");
}
 
static void totals(const Student *s, int *A, int *T) {
    int i;
    *A = 0; *T = 0;
    for (i = 0; i < NUM_SUBJECTS; i++) { *A += s->attended[i]; *T += TOTAL[i]; }
}
 
/* ---------- list all students ---------- */
static void list_students(void) {
    int i, A, T;
    printf("\n%-6s %-18s %-9s %s\n", "Roll", "Name", "Overall", "Exam eligibility");
    printf("------------------------------------------------------\n");
    for (i = 0; i < NUM_STUDENTS; i++) {
        totals(&students[i], &A, &T);
        printf("%-6d %-18s %6.1f%%   %s\n", students[i].roll, students[i].name,
               (double)A / T * 100.0, meets(A, T) ? "ELIGIBLE" : "NOT ELIGIBLE");
    }
}
 
/* ---------- full report for one student ---------- */
static void show_student(const Student *s) {
    int i, A, T, low_count = 0, safe_count = 0;
    totals(s, &A, &T);
 
    for (i = 0; i < NUM_SUBJECTS; i++) {
        if (!meets(s->attended[i], TOTAL[i])) low_count++;
        if (is_safe(s->attended[i], TOTAL[i])) safe_count++;
    }
 
    printf("\n=====================================================\n");
    printf("  STUDENT ATTENDANCE REPORT\n");
    printf("=====================================================\n");
    printf("Roll No : %d\n", s->roll);
    printf("Name    : %s\n", s->name);
 
    /* Subject-wise table */
    printf("\n%-13s %-8s %-7s %-22s %-11s %s\n",
           "Subject", "Att/Tot", "%", "Progress", "Status", "Note");
    for (i = 0; i < NUM_SUBJECTS; i++) {
        int a = s->attended[i], t = TOTAL[i];
        double p = (double)a / t * 100.0;
        char att[24], note[40];
        sprintf(att, "%d/%d", a, t);
        if (meets(a, t)) sprintf(note, "can miss %d more", can_skip(a, t));
        else             sprintf(note, "attend next %d", classes_needed(a, t));
        printf("%-13s %-8s %5.1f%%  ", SUBJECT[i], att, p);
        draw_bar(p);
        printf("  %-11s %s\n", status_text(a, t), note);
    }
 
    /* Overall + eligibility */
    double overall = (double)A / T * 100.0;
    printf("\n-----------------------------------------------------\n");
    printf("Total classes held     : %d\n", T);
    printf("Total classes attended : %d\n", A);
    printf("Overall attendance     : %.2f%%  (required: %d%%)\n", overall, REQUIRED);
    printf("Exam eligibility       : %s\n", meets(A, T) ? "ELIGIBLE" : "NOT ELIGIBLE");
    printf("-----------------------------------------------------\n");
 
    /* Summary */
    printf("\nSUMMARY\n");
    printf("%s attended %d of %d classes (%.1f%%). ", s->name, A, T, overall);
    if (meets(A, T)) printf("Eligible to write the exam.\n");
    else             printf("Not eligible: %.1f%% is below the required %d%%.\n", overall, REQUIRED);
 
    if (low_count > 0) {
        printf("Below %d%% in:", REQUIRED);
        int first = 1;
        for (i = 0; i < NUM_SUBJECTS; i++)
            if (!meets(s->attended[i], TOTAL[i])) { printf("%s %s", first ? "" : ",", SUBJECT[i]); first = 0; }
        printf(".\n");
    }
 
    /* Suggestions */
    printf("\nSUGGESTIONS\n");
    int tip = 1;
 
    if (!meets(A, T)) {
        printf("%d. To become eligible, attend the next %d classes (across subjects) without a miss.\n",
               tip++, classes_needed(A, T));
    } else if (!is_safe(A, T)) {
        int s2 = can_skip(A, T);
        if (s2 == 0)
            printf("%d. Eligibility is right at the limit. Do not miss any more classes.\n", tip++);
        else
            printf("%d. Eligibility is close to the limit. You can miss only %d more class%s overall.\n",
                   tip++, s2, s2 == 1 ? "" : "es");
    } else {
        printf("%d. Overall attendance is healthy. Keep it above %d%% until the exams.\n", tip++, REQUIRED + 5);
    }
 
    for (i = 0; i < NUM_SUBJECTS; i++) {
        int a = s->attended[i], t = TOTAL[i];
        double p = (double)a / t * 100.0;
        if (!meets(a, t)) {
            printf("%d. %s is at %.1f%%. Attend the next %d classes without a miss to reach %d%%.",
                   tip++, SUBJECT[i], p, classes_needed(a, t), REQUIRED);
            if (p < REQUIRED - 15) printf(" This is a serious gap; meet your mentor this week.");
            printf("\n");
        } else if (!is_safe(a, t)) {
            int k = can_skip(a, t);
            printf("%d. %s is close to the limit. You can miss only %d more class%s.\n",
                   tip++, SUBJECT[i], k, k == 1 ? "" : "es");
        }
    }
 
    if (safe_count > 0) {
        printf("%d. Keep the streak in", tip++);
        int first = 1;
        for (i = 0; i < NUM_SUBJECTS; i++)
            if (is_safe(s->attended[i], TOTAL[i])) { printf("%s %s", first ? "" : ",", SUBJECT[i]); first = 0; }
        printf(". Your buffer there is healthy.\n");
    }
    if (low_count > 0)
        printf("%d. Prioritize early-morning and lab sessions in weak subjects, and set a reminder the night before.\n", tip++);
    printf("\n");
}
 
int main(void) {
    printf("=====================================\n");
    printf("   CAMPUS ATTENDANCE ASSISTANT\n");
    printf("=====================================\n");
 
    while (1) {
        printf("\nMENU\n");
        printf("  1. List all students\n");
        printf("  2. View a student's attendance and exam eligibility\n");
        printf("  3. Exit\n");
        int choice = read_int("Choose an option: ", 1, 3);
 
        if (choice == 1) {
            list_students();
        } else if (choice == 2) {
            int roll = read_int("Enter roll number (101-115): ", 1, 999999);
            int i, found = -1;
            for (i = 0; i < NUM_STUDENTS; i++)
                if (students[i].roll == roll) { found = i; break; }
            if (found < 0) printf("No student found with roll number %d.\n", roll);
            else show_student(&students[found]);
        } else {
            printf("Goodbye.\n");
            break;
        }
    }
    return 0;
}