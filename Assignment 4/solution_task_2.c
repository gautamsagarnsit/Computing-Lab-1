/*
Task 2.
Maximise the course count while minimising the clash count.

To execute this program, run the following commands.
    sudo apt install glpk-utils libglpk-dev
    gcc -std=c11 -Wall -Wextra -o 21CS60R18_ass_4_task_2 21CS60R18_ass_4_task_2.c -lglpk -lm
    ./21CS60R18_ass_4_task_2 <input file>
*/

#include <glpk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     solve_LP2_initial
//
// Maximise the course count. This is just a rehash of the problem solved in
// task 1.
//
// Args:
//     students: int (number of students)
//     courses: int (number of courses)
//     preferences: int ** (bitmap of students' course preferences)
//     max_courses_per_student: int (course-induced constraint)
//     max_students_per_course: int * (student-induced constraint)
//     st_cs: int ** (bitmap to store students' course allocations)
//
// Returns:
//     double (number of courses allocated)
///////////////////////////////////////////////////////////////////////////////
double solve_LP2_initial(int students,
                         int courses,
                         int **preferences,
                         int max_courses_per_student,
                         int *max_students_per_course,
                         int **st_cs)
{
    // Create a maximisation problem.
    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MAX);

    // Define the objective function. Constrain the variables. There are as
    // many columns as there are variables.
    glp_add_cols(lp, students * courses);
    for(int i = 1; i <= students * courses; ++i)
    {
        glp_set_col_bnds(lp, i, GLP_DB, 0, 1);
        glp_set_obj_coef(lp, i, preferences[(i - 1) / courses][(i - 1) % courses]);
    }

    // Constrain functions of the variables. There are as many rows as there
    // are functions.
    glp_add_rows(lp, students + courses);
    for(int i = 1; i <= courses; ++i)
    {
        // Limit the number of students allotted a course.
        glp_set_row_bnds(lp, i, GLP_UP, 0, max_students_per_course[i - 1]);
    }
    for(int i = courses + 1; i <= courses + students; ++i)
    {
        // Limit the number of courses allotted to a student.
        glp_set_row_bnds(lp, i, GLP_UP, 0, max_courses_per_student);
    }

    // Allocate sufficient memory to store all non-zero constraint
    // coefficients.
    int *row_ids = malloc((2 * students * courses + 1) * sizeof *row_ids);
    int *col_ids = malloc((2 * students * courses + 1) * sizeof *col_ids);
    double *mat_val = malloc((2 * students * courses + 1) * sizeof *mat_val);
    int constraint = 0;

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

    glp_load_matrix(lp, constraint, row_ids, col_ids, mat_val);

    glp_simplex(lp, NULL);

    // Enforce integrality constraints on such misbehaving variables.
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
        obj = retrieve_obj(lp);

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

                double v = retrieve_col(lp, i * courses + j + 1);
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
                    glp_set_col_kind(lp, i * courses + j + 1, GLP_BV);
                    satisfactory = 0;
                    break;
                }
            }
        }
        if(satisfactory)
        {
            break;
        }

        glp_simplex(lp, NULL);
        glp_intopt(lp, NULL);
    }

    printf("Total %.0lf courses allocated.\n", obj);

    free(row_ids);
    free(col_ids);
    free(mat_val);
    glp_delete_prob(lp);

    return obj;
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     solve_LP1
//
// Minimise the slot clashes, given the students' course allotments.
//
// Args:
//     students: int (number of students)
//     courses: int (number of courses)
//     slots: int (number of slots)
//     st_cs: int ** (bitmap of students' course allocations)
//     cs_sl: int ** (bitmap to store courses' slot allocations)
//
// Returns:
//     double (number of courses clashes)
///////////////////////////////////////////////////////////////////////////////

double solve_LP1(int students, int courses, int slots, int **st_cs, int **cs_sl)
{
    // Create a minimisation problem.
    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);

    // Define the objective function. Constrain the variables. There are as
    // many columns as there are variables.
    glp_add_cols(lp, courses * slots + courses * courses * slots);

    // The first `courses * slots' variables indicate course-slot allocation.
    // They do not appear in the objective function.
    for(int i = 1; i <= courses * slots; ++i)
    {
        glp_set_col_bnds(lp, i, GLP_DB, 0, 1);
        glp_set_obj_coef(lp, i, 0);
    }

    // The next `courses * courses * slots' variables indicate
    // course-course-slot clashes.
    for(int i = courses * slots + 1; i <= courses * slots + courses * courses * slots; ++i)
    {
        glp_set_col_bnds(lp, i, GLP_DB, 0, 1);

        // From the 1D locator `i', extract 3D locators which we need to find
        // and set the coefficient.
        int j = i - courses * slots - 1;
        int middle = j / slots % courses;
        int high = j / slots / courses;
        int coef = 0;
        for(int k = 0; k < students; ++k)
        {
            coef += st_cs[k][middle] * st_cs[k][high];
        }
        glp_set_obj_coef(lp, i, coef);
    }

    // Constrain functions of the variables. There are as many rows as there
    // are functions.
    glp_add_rows(lp, courses + courses * courses * slots);
    for(int i = 1; i <= courses; ++i)
    {
        // Limit the courses' slot allocation.
        glp_set_row_bnds(lp, i, GLP_FX, 1, 1);
    }
    for(int i = courses + 1; i <= courses + courses * courses * slots; ++i)
    {
        // Link course-slot allocation to course-course-slot clashes.
        glp_set_row_bnds(lp, i, GLP_UP, 0, 1);
    }

    // Allocate sufficient memory to store all non-zero constraint
    // coefficients.
    int *row_ids = malloc((courses * slots + courses * courses * slots * 3 + 1) * sizeof *row_ids);
    int *col_ids = malloc((courses * slots + courses * courses * slots * 3 + 1) * sizeof *col_ids);
    double *mat_val = malloc((courses * slots + courses * courses * slots * 3 + 1) * sizeof *mat_val);
    int constraint = 0;

    // In the first `courses' rows, specific course-slot coefficients must be 1.
    for(int i = 1; i <= courses; ++i)
    {
        for(int j = 1; j <= slots; ++j)
        {
            ++constraint;
            row_ids[constraint] = i;
            col_ids[constraint] = (i - 1) * slots + j;
            mat_val[constraint] = 1;
        }
    }

    // In the next `courses * courses * slots' rows, set the coefficients of
    // three of the variables.
    for(int i = courses + 1; i <= courses + courses * courses * slots; ++i)
    {
        // From the 1D locator `i', extract 3D locators which we need to find
        // and set the coefficients.
        int j = i - courses - 1;
        int low = j % slots;
        int middle = j / slots % courses;
        int high = j / slots / courses;
        if(high == middle)
        {
            continue;
        }

        // If a course is allotted a certain slot ...
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = high * slots + low + 1;
        mat_val[constraint] = 1;

        // ... and another course is allotted the same slot ...
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = middle * slots + low + 1;
        mat_val[constraint] = 1;

        // ... then the newly constructed variable must link them and the row
        // constraints previously defined.
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = j + courses * slots + 1;
        mat_val[constraint] = -1;
    }

    glp_load_matrix(lp, constraint, row_ids, col_ids, mat_val);

    // How much error is permitted before we cannot reasonably approximate a
    // variable as an integer?
    int const denominator = courses > slots ? courses : slots;
    double const ltol = 1.0 / denominator;
    double const rtol = 1 - ltol;

    glp_simplex(lp, NULL);

    // Enforce integrality constraints on such misbehaving variables.
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
        obj = retrieve_obj(lp);

        // Put the solution in the provided matrix.
        int satisfactory = 1;
        for(int i = 0; i < courses; ++i)
        {
            for(int j = 0; j < slots; ++j)
            {
                double v = retrieve_col(lp, i * slots + j + 1);
                if(v < ltol)
                {
                    cs_sl[i][j] = 0;
                }
                else if(v > rtol)
                {
                    cs_sl[i][j] = 1;
                }
                else
                {
                    glp_set_col_kind(lp, i * slots + j + 1, GLP_BV);
                    satisfactory = 0;
                    break;
                }
            }
        }
        if(satisfactory)
        {
            break;
        }

        glp_simplex(lp, NULL);
        glp_intopt(lp, NULL);
    }

    printf("Total %.0lf clashes occurred.\n", obj);

    free(row_ids);
    free(col_ids);
    free(mat_val);
    glp_delete_prob(lp);

    return obj;
}

///////////////////////////////////////////////////////////////////////////////
// Function Name:
//     solve_LP2
//
// Maximise the course count while minimising the course clashes, given the
// courses' slot allocations. Give more weight to the latter, as indicated by
// the `lambda' argument.
//
// Args:
//     students: int (number of students)
//     courses: int (number of courses)
//     slots: int (number of slots)
//     preferences: int ** (bitmap of students' course preferences)
//     max_courses_per_student: int (course-induced constraint)
//     max_students_per_course: int * (student-induced constraint)
//     st_cs: int ** (bitmap to store students' course allocations)
//     cs_sl: int ** (bitmap of courses' slot allocations)
//     lambda: double (parameter to prioritise prevention of slot clashes)
//
// Returns:
//     double (number of clashes-allocations difference)
///////////////////////////////////////////////////////////////////////////////

double solve_LP2(int students,
                 int courses,
                 int slots,
                 int **preferences,
                 int max_courses_per_student,
                 int *max_students_per_course,
                 int **st_cs,
                 int **cs_sl,
                 int lambda)
{
    // Create a minimisation problem.
    glp_prob *lp = glp_create_prob();
    glp_set_obj_dir(lp, GLP_MIN);

    // Define the objective function. Constrain the variables. There are as
    // many columns as there are variables.
    glp_add_cols(lp, students * courses + students * courses * courses);

    // The first `students * courses' variables indicate student-course
    // allocation.
    for(int i = 1; i <= students * courses; ++i)
    {
        glp_set_col_bnds(lp, i, GLP_DB, 0, 1);
        glp_set_obj_coef(lp, i, -preferences[(i - 1) / courses][(i - 1) % courses]);
    }

    // The next `students * courses * courses' variables indicate
    // student-course-course clashes.
    for(int i = students * courses + 1; i <= students * courses + students * courses * courses; ++i)
    {
        glp_set_col_bnds(lp, i, GLP_DB, 0, 1);

        // From the 1D locator `i', extract 3D locators which we need to find
        // and set the coefficient.
        int j = i - students * courses - 1;
        int low = j % courses;
        int middle = j / courses % courses;
        int coef = 0;
        for(int k = 0; k < slots; ++k)
        {
            coef += cs_sl[middle][k] * cs_sl[low][k];
        }
        glp_set_obj_coef(lp, i, coef * lambda);
    }

    // Constrain functions of the variables. There are as many rows as there
    // are functions.
    glp_add_rows(lp, students + courses + students * courses * courses);
    for(int i = 1; i <= courses; ++i)
    {
        // Limit the number of students allotted a course.
        glp_set_row_bnds(lp, i, GLP_UP, 0, max_students_per_course[i - 1]);
    }
    for(int i = courses + 1; i <= courses + students; ++i)
    {
        // Limit the number of courses allotted to a student.
        glp_set_row_bnds(lp, i, GLP_UP, 0, max_courses_per_student);
    }
    for(int i = courses + students + 1; i <= courses + students + students * courses * courses; ++i)
    {
        // Link student-course allocation to student-course-course clashes.
        glp_set_row_bnds(lp, i, GLP_UP, 0, 1);
    }

    // Allocate sufficient memory to store all non-zero constraint
    // coefficients.
    int *row_ids = malloc((2 * courses * students + 3 * students * courses * courses + 1) * sizeof *row_ids);
    int *col_ids = malloc((2 * courses * students + 3 * students * courses * courses + 1) * sizeof *col_ids);
    double *mat_val = malloc((2 * courses * students + 3 * students * courses * courses + 1) * sizeof *mat_val);
    int constraint = 0;

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

    // In the next `students * courses * courses' rows, set the coefficients of
    // three of the variables.
    for(int i = courses + students + 1; i <= courses + students + students * courses * courses; ++i)
    {
        // From the 1D locator `i', extract 3D locators which we need to find
        // and set the coefficients.
        int j = i - courses - students - 1;
        int low = j % courses;
        int middle = j / courses % courses;
        int high = j / courses / courses;
        if(middle == low)
        {
            continue;
        }

        // Is there a clash?
        int has_clash = 0;
        for(int k = 0; k < slots; ++k)
        {
            if(cs_sl[middle][k] && cs_sl[low][k])
            {
                has_clash = 1;
                break;
            }
        }
        if(!has_clash)
        {
            continue;
        }

        // If a student has taken a course ...
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = high * courses + middle + 1;
        mat_val[constraint] = 1;

        // ... and another course ...
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = high * courses + low + 1;
        mat_val[constraint] = 1;

        // ... then the newly constructed variable must link them and the row
        // constraints previously defined.
        ++constraint;
        row_ids[constraint] = i;
        col_ids[constraint] = j + students * courses + 1;
        mat_val[constraint] = -1;
    }

    glp_load_matrix(lp, constraint, row_ids, col_ids, mat_val);

    glp_simplex(lp, NULL);

    // Enforce integrality constraints on such misbehaving variables.
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
        obj = retrieve_obj(lp);

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

                double v = retrieve_col(lp, i * courses + j + 1);
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
                    glp_set_col_kind(lp, i * courses + j + 1, GLP_BV);
                    satisfactory = 0;
                    break;
                }
            }
        }
        if(satisfactory)
        {
            break;
        }

        glp_simplex(lp, NULL);
        glp_intopt(lp, NULL);
    }

    printf("Observed %.0lf student-slot and student-course difference.\n", obj);

    free(row_ids);
    free(col_ids);
    free(mat_val);
    glp_delete_prob(lp);

    return obj;
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
    int students, courses, slots;
    fscanf(input, "%d,%d,%d", &students, &courses, &slots);
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

    // To store courses' slot allocations.
    int **cs_sl = malloc(courses * sizeof *cs_sl);
    for(int i = 0; i < courses; ++i)
    {
        cs_sl[i] = malloc(slots * sizeof **cs_sl);
    }

    // Repeat the steps mentioned in the problem statement.
    double lambda = 0, prev_lambda = 0;
    double result;
    //for(int i = 0; i < 8 * (int)sizeof(int) - 1; ++i)
    for(int i = 0; i < 10000 - 1; ++i)
    {
        // Choose the priority parameter. It must increase linearly with the
        // number of iterations.
        prev_lambda = lambda;
        lambda = 1.5 * i;
        printf("Priority parameter set to %lf ...\n", lambda);

        if(i == 0)
        {
            // Course-slot allocation not done. Fix student-course allocation.
            solve_LP2_initial(students, courses, preferences, max_courses_per_student, max_students_per_course, st_cs);
        }
        else
        {
            // Course-slot allocation done. Derive student-course allocation.
            solve_LP2(students, courses, slots,
                      preferences,
                      max_courses_per_student, max_students_per_course,
                      st_cs, cs_sl,
                      lambda);
        }

        // Student-course allocation done. Derive course-slot allocation.
        result = solve_LP1(students, courses, slots, st_cs, cs_sl);
        if(result == 0)
        {
            break;
        }
    }

    // Try to decrease the parameter.
    int finish = 0;
    for(int i = 0; i < 4; ++i)
    {
        // Use the arithmetic mean of the current and previous values. If we
        // overshot, backtrack to the previous value.
        if(result == 0)
        {
            double new_lambda = (lambda + prev_lambda) / 2.0;
            lambda = new_lambda;
        }
        else
        {
            lambda = 2 * lambda - prev_lambda;
            finish = 1;
        }
        printf("Priority parameter set to %lf ...\n", lambda);

        solve_LP2(students, courses, slots,
                  preferences,
                  max_courses_per_student, max_students_per_course,
                  st_cs, cs_sl,
                  lambda);
        result = solve_LP1(students, courses, slots, st_cs, cs_sl);
        if(finish)
        {
            break;
        }
    }

    // Display course-slot allocation.
    printf("Courses v/s slots shown below.\n");
    for(int i = 0; i < courses; ++i)
    {
        printf("%8d: ", i + 1);
        for(int j = 0; j < slots; ++j)
        {
            if(cs_sl[i][j])
            {
                printf("%d ", j + 1);
            }
        }
        printf("\n");
    }

    // Display student-course allocation.
    printf("Students v/s courses shown below.\n");
    int total_courses = 0;
    for(int i = 0; i < students; ++i)
    {
        printf("%8d: ", i + 1);
        for(int j = 0; j < courses; ++j)
        {
            if(st_cs[i][j])
            {
                total_courses += 1;
                printf("%d ", j + 1);
            }
        }
        printf("\n");
    }
    printf("Total %d courses allocated.\n", total_courses);

    for(int i = 0; i < students; ++i)
    {
        free(preferences[i]);
        free(st_cs[i]);
    }
    free(preferences);
    free(st_cs);
    for(int i = 0; i < courses; ++i)
    {
        free(cs_sl[i]);
    }
    free(cs_sl);
    free(max_students_per_course);

    return 0;
}
