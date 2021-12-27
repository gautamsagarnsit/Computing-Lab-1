/*
Task 1.
Maximise the course count.

To execute this program, run the following commands.
    sudo apt install glpk-utils libglpk-dev
    gcc -std=c11 -Wall -Wextra -o 21CS60R18_ass_4_task_1 21CS60R18_ass_4_task_1.c -lglpk -lm
    ./21CS60R18_ass_4_task_1 <input file>
*/

#include <glpk.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     add_column_info
//
// Define variable constraints and the objective function.
//
// Args:
//     maximise_courses: glp_prob * (GLPK problem object)
//     students: int (number of students)
//     courses: int (number of courses)
//     preferences: int ** (bitmap of students' course preferences)
//
// Returns
//     nothing
///////////////////////////////////////////////////////////////////////////////

void add_column_info(glp_prob *maximise_courses, 
                    int students, 
                    int courses, 
                    int **preferences) //students course preferences
{
    // Number of columns equals number of variables.
    glp_add_cols(maximise_courses, students * courses);
    for(int i = 1; i <= students * courses; ++i)
    {
        glp_set_col_bnds(maximise_courses, i, GLP_DB, 0, 1);
        glp_set_obj_coef(maximise_courses, i, preferences[(i - 1) / courses][(i - 1) % courses]);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     add_row_info
//
// Define function constraints.
//
// Args:
//     maximise_courses: glp_prob * (GLPK problem object)
//     students: int (number of students)
//     courses: int (number of courses)
//     max_courses_per_student: int (course-induced constraint)
//     max_students_per_course: int * (student-induced constraint)
//
// Returns
//     nothing
///////////////////////////////////////////////////////////////////////////////

void add_row_info(glp_prob *maximise_courses,
                  int students,
                  int courses,
                  int max_courses_per_student, //(course-induced constraint)
                  int *max_students_per_course) // (student-induced constraint)
{
    // Number of rows equals number of constraints.
    glp_add_rows(maximise_courses, students + courses);
    for(int i = 1; i <= courses; ++i)
    {
        // Limit the number of students allotted a course.
        glp_set_row_bnds(maximise_courses, i, GLP_UP, 0, max_students_per_course[i - 1]);
    }
    for(int i = courses + 1; i <= courses + students; ++i)
    {
        // Limit the number of courses allotted to a student.
        glp_set_row_bnds(maximise_courses, i, GLP_UP, 0, max_courses_per_student);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Function Name
//     add_constraint_mat
//
// Pass all non-zero constraint coefficients.
//
// Args:
//     maximise_courses: glp_prob * (GLPK problem object)
//     students: int (number of students)
//     courses: int (number of courses)
//     pia: int ** (pointer to return the row indices to the caller)
//     pja: int ** (pointer to return the column indices to the caller)
//     par: double ** (pointer to return the matrix values to the caller)
//
// Returns:
//     nothing
///////////////////////////////////////////////////////////////////////////////

void add_constraint_mat(glp_prob *maximise_courses, 
                        int students, //(number of students)
                        int courses, // (number of courses)
                        int **pia, // (pointer to return the row indices to the caller)
                        int **pja, // (pointer to return the column indices to the caller)
                        double **par) // (pointer to return the matrix values to the caller)
{
    // Allocate sufficient memory to prepare the constraint matrix.
    int *row_ids = malloc((2 * students * courses + 1) * sizeof *row_ids);
    int *col_ids = malloc((2 * students * courses + 1) * sizeof *col_ids);
    double *mat_val = malloc((2 * students * courses + 1) * sizeof *mat_val);
    int constraint = 0;

    // A way to give back the pointers to the caller so that it may free the
    // memory.
    *pia = row_ids;
    *pja = col_ids;
    *par = mat_val;

    // In the first `courses' rows, set the element corresponding to a
    // particular course for all students.
    for(int i = 1; i <= courses; ++i)
    {
        for(int j = 1; j <= students; ++j)
        {
            ++constraint;
            row_ids[constraint] = i;
            col_ids[constraint] = (j - 1) * courses + i;
            mat_val[constraint] = 1;
        }
    }

    // In the next `students' rows, set the element corresponding to a
    // particular student for all courses.
    for(int i = courses + 1; i <= courses + students; ++i)
    {
        for(int j = 1; j <= courses; ++j)
        {
            ++constraint;
            row_ids[constraint] = i;
            col_ids[constraint] = (i - courses - 1) * courses + j;
            mat_val[constraint] = 1;
        }
    }

    glp_load_matrix(maximise_courses, constraint, row_ids, col_ids, mat_val);
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     solve_and_print
//
// Solve the linear programming problem and print it.
//
// Args:
//     maximise_courses: glp_prob * (GLPK problem object)
//     students: int (number of students)
//     courses: int (number of courses)
//     st_cs: int ** (bitmap to store students' course allocations)
//     max_courses_per_student: int (course-induced constraint)
//     max_students_per_course: int * (student-induced constraint)
//
// Returns
//     nothing
///////////////////////////////////////////////////////////////////////////////

void solve_and_print(glp_prob *maximise_courses,
                     int students,
                     int courses,
                     int **st_cs, //(bitmap to store students' course allocations)
                     int max_courses_per_student,
                     int *max_students_per_course)
{
    glp_simplex(maximise_courses, NULL);

    // Enforce integrality constraints on misbehaving variables.
    double obj;
    for(int iterations = 0;; ++iterations)
    {
        // If we have only the simplex solution (the MIP solution becomes
        // available after one iteration), use it. We will run the MIP solver
        // only if necessary, thereby saving a lot of time.
        double (*retrieve_col)(glp_prob *, int);
        double (*retrieve_obj)(glp_prob *);
        if(iterations == 0)
        {
            retrieve_col = glp_get_col_prim;
            retrieve_obj = glp_get_obj_val;
        }
        else
        {
            retrieve_col = glp_mip_col_val;
            retrieve_obj = glp_mip_obj_val;
        }
        obj = retrieve_obj(maximise_courses);

        // Put the solution in the provided matrix.
        int satisfactory = 1;
        for(int i = 0; i < students; ++i)
        {
            for(int j = 0; j < courses; ++j)
            {
                // How much error is permitted before we cannot reasonably
                // approximate a variable as an integer?
                int const denominator = max_courses_per_student > max_students_per_course[j] ?
                                        max_courses_per_student : max_students_per_course[j];
                double const ltol = 1.0 / denominator;
                double const rtol = 1 - ltol;

                double v = retrieve_col(maximise_courses, i * courses + j + 1);
                if(v < ltol)
                {
                    st_cs[i][j] = 0;
                }
                else if(v > rtol)
                {
                    st_cs[i][j] = 1;
                }
                else
                {
                    glp_set_col_kind(maximise_courses, i * courses + j + 1, GLP_BV);
                    satisfactory = 0;
                }
            }
        }

        // Display the solution.
        if(satisfactory)
        {
            printf("Total %.0lf courses allocated. Each row lists the courses allocated to a student.\n", obj);
            for(int i = 0; i < students; ++i)
            {
                printf("%8d: ", i + 1);
                for(int j = 0; j < courses; ++j)
                {
                    if(st_cs[i][j])
                    {
                        printf("%d ", j + 1);
                    }
                }
                printf("\n");
            }
            return;
        }

        glp_simplex(maximise_courses, NULL);
        glp_intopt(maximise_courses, NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Main function.
///////////////////////////////////////////////////////////////////////////////
int main(int const argc, char const *argv[])
{
    if(argc < 2)
    {
        printf("Usage:\n");
        printf("\t%s <input file>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");

    // Read bounds.
    int students, courses;
    fscanf(input, "%d,%d", &students, &courses);
    int max_courses_per_student;
    fscanf(input, "%d", &max_courses_per_student);
    int *max_students_per_course = malloc(courses * sizeof *max_students_per_course);
    for(int i = 0; i < courses; ++i)
    {
        if(i == courses - 1)
        {
            fscanf(input, "%d", max_students_per_course + i);
        }
        else
        {
            fscanf(input, "%d,", max_students_per_course + i);
        }
    }

    // Read preferences. Convert 1-based indices to 0-based before using them.
    int **preferences = malloc(students * sizeof *preferences);
    for(int i = 0; i < students; ++i)
    {
        preferences[i] = calloc(courses, sizeof **preferences);
    }
    int given_prefs;
    fscanf(input, "%d", &given_prefs);
    for(int i = 0; i < given_prefs; ++i)
    {
        int s, c;
        fscanf(input, "%d,%d", &s, &c);
        preferences[s - 1][c - 1] = 1;
    }

    fclose(input);

    // To store students' course allocations.
    int **st_cs = malloc(students * sizeof *st_cs);
    for(int i = 0; i < students; ++i)
    {
        st_cs[i] = malloc(courses * sizeof **st_cs);
    }

    // Create a maximisation problem.
    glp_prob *maximise_courses = glp_create_prob();
    glp_set_obj_dir(maximise_courses, GLP_MAX);

    // Solve it.
    add_column_info(maximise_courses, students, courses, preferences);
    add_row_info(maximise_courses, students, courses, max_courses_per_student, max_students_per_course);
    int *ia, *ja;
    double *ar;
    add_constraint_mat(maximise_courses, students, courses, &ia, &ja, &ar);
    solve_and_print(maximise_courses, students, courses, st_cs, max_courses_per_student, max_students_per_course);

    for(int i = 0; i < students; ++i)
    {
        free(preferences[i]);
        free(st_cs[i]);
    }
    free(preferences);
    free(st_cs);
    free(max_students_per_course);
    free(ia);
    free(ja);
    free(ar);
    glp_delete_prob(maximise_courses);

    return 0;
}
