#define _GNU_SOURCE  // asprintf is part of stdio.h, but need to add
                     // #define _GNU_SOURCE at the top of the file otherwise it gives an error

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

// Global variable
char active_file_path[120],  // stores the currently active file path
     file_path[120],         // stores 'active_file.txt' path
     *files_dir_path,        // stores todos files directory path
     *use_file = NULL;       // stores the file name to use instead of currently active file


/* Opens a file on any mode
 * this will return a file pointer which we can use further
 */
FILE * open_file(char file_name[], const char *mode){
    FILE *file_pointer = fopen(file_name, mode);
    return file_pointer;
}

/*
 * Returns the name of currently active file
*/
void set_active_file_name(){
    files_dir_path = getenv("TODOS_TEXT_DIR");

    if (use_file) {
        strcpy(active_file_path, files_dir_path);
        strcat(active_file_path, use_file);
        goto active_file_exists;
    }

    // Make "active_file.txt" path
    strcpy(file_path, files_dir_path);
    strcat(file_path, "active_file.txt");

    FILE *file = open_file(file_path, "r");

    if (file){
        // Load the currently active file name into char array
        char active_file_name[50] = {'\0'};
        fgets(active_file_name, sizeof(active_file_name), file);

        if (active_file_name[0] == '\0') {
            printf("No file is currently selected! Using default TODO file\n\n");
            goto set_default_todo_file;
        }

        // Make the currently active file path
        strcpy(active_file_path, files_dir_path);
        strcat(active_file_path, active_file_name);

        goto active_file_exists;
    }

    printf("'active_file.txt' is missing! Using default TODO file\n\n");

    set_default_todo_file:
        stpcpy(active_file_path, files_dir_path);
        strcat(active_file_path, "TODO");
        return;

    active_file_exists:
        file = open_file(active_file_path, "r");

        if (!file) {
            printf("'%s' is not exists! Using default TODO file\n\n", active_file_path);
            goto set_default_todo_file;
        }

}

/* Slice the char array
 * for now using this function to remove the todo priority & id
 */
void slice(const char *todo, char *result, int start, int end){
    strncpy(result, todo + start, end - start);
}

/* Open the file on write mode and delete all existing content
 * then rewrite the remaining todos
 */
void write_all_todos(char todo[][1000], int size){
    FILE *write_file = open_file(active_file_path, "w");

    for (int i=0; i<size; i++)
        fputs(todo[i], write_file);

    fclose(write_file);

}

/* Opens the file in read mode and stores all todos in the all_todos char array */
void get_all_todos(char all_todos[], int size){
    FILE *file = open_file(active_file_path, "r");

    if (!file){
        printf("File Not found\n");
        exit(0);
    }

    fread(all_todos, size, 1, file);
    fclose(file);
}

/* This will convert 1D array to 2D array
 * here is a code snippet for better understanding
 * char all_todos[] = {"write an essay on cow\nplay with pet"};
 * char todos[][1000] = {{"write an essay on cow"}, {"play with pet"}};
 */
void seprate_all_todos(char all_todos[], char todos[][1000]){
    char temp[1000];
    int j, k = 0;

    for (int i=0; all_todos[i] != '\0'; i++){
        j = 0;
        while(all_todos[i] != '\n'){
            temp[j] = all_todos[i];
            j++;
            i++;
        }

        // add a new line and null character 
        temp[j] = '\n';
        temp[++j] = '\0';

        // copy temp array into todos 2d array
        strcpy(todos[k], temp);
        k++;

    }

}

/* Returns total number of todos
 * this will count all completed and pending todos
 */
int num_of_todos(char all_todos[]){
    int total_todos = 0;
    for (int i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '\n') total_todos++;
    }
    return total_todos;
}

/* Reverse the char array
 * mainly using this function to get or create todo id
 */
void reverse(char* str) {
    int len = strlen(str);
    int i, j;
    char temp;
    
    for (i = 0, j = len - 1; i < j; i++, j--) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

/* Generates id for todo
 * this will generate a unique id for each todo
 */
int generate_todo_id(char all_todos[]){
    int todos_count = num_of_todos(all_todos), todo_len, count;
    char todos[todos_count][1000], char_todo_id[4] = {'\0'};
    seprate_all_todos(all_todos, todos);

    int todo_id_count[todos_count];

    for (int i=0; i<todos_count; i++)
        todo_id_count[i] = 0;

    for (int i=0; i<todos_count; i++){
        todo_len = strlen(todos[i]) - 1, count = 0;

        while (todos[i][todo_len] != ' '){
            char_todo_id[count] = todos[i][todo_len];
            count++;
            todo_len--;
        }

        reverse(char_todo_id);

        todo_id_count[atoi(char_todo_id)-1]++;
    }

    for (int i=0; i<todos_count; i++){
        if (todo_id_count[i] == 0){
            return i+1;
        } else if (todo_id_count[i] > 1){
            printf("Duplicate id found -> %d\n", i+1);
        }
    }

    return todos_count+1;

}

/* Function to get priorities of all todos */
void get_todos_priority(char all_todos[], int priority[]){
    int i = 0, j = 0, k, l;
    for (i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '['){
            char temp[4] = {'\0'};
            l = 0;
            for (k=i+1; all_todos[k] != ']'; k++){
                temp[l] = all_todos[k];
                l++;
            }
            priority[j] = atoi(temp);
            i += l;
            j++;
        }
    }
}

/* Sort todo by priority
 * using selection sort algorithm to sort todo by priority (will update it with merge sort in future)
 */ 
void sort_by_priority(char todos[][1000], int priority[], int size){

    char temp[1000];

    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            if (priority[i] > priority[j]) {
                int temp_priority = priority[i];
                priority[i] = priority[j];
                priority[j] = temp_priority;
                strcpy(temp, todos[i]);
                strcpy(todos[i], todos[j]);
                strcpy(todos[j], temp);
            }
        }
    }
}

/* Preprocessing the todos before sorting them
 * 1) extracting all priorities from todos
 * 2) converting todos from 1D array to 2D array
 */ 
void priority_sorting(char all_todos[], char todos[][1000], int todos_count){
    int priority[todos_count];
    get_todos_priority(all_todos, priority);  // extract all priority from todos and save them to an array
    seprate_all_todos(all_todos, todos);  // store todos into 2d array
    sort_by_priority(todos, priority, todos_count);  // sort todos by priority
}

/* Removing todo priority and id */
void get_formatted_todo(const char *todo, char *empty_space, int flag){
    int i = flag ? 1 : 0;
    for (; todo[i] != '['; i++){
        if (flag)
            empty_space[i-1] = todo[i];  // for completed todo
        else
            empty_space[i] = todo[i];  // for pending todo
    }
    i = flag ? i-1 : i;
    empty_space[i++] = '\n';
    empty_space[i] = '\0';
}

/* Set default TODO file */
void update_active_todo_file(char *file_name){
    // open the file just to check if it's exists
    char _file_path[100];
    strcpy(_file_path, files_dir_path);
    strcat(_file_path, file_name);  // concatenate the file name with files_dir_path
    FILE *file = open_file(_file_path, "r");

    if (file){
        // update the file name on `active_file.txt`
        file = open_file(file_path, "w");
        fputs(file_name, file);
        printf("Now using %s as default TODO file\n", file_name);
    } else {
        printf("File '%s' is not exists\n", file_name);
    }

}

/* Creates a new todo file */
void create_todo_file(char *file_name){
    // open the file just to check if it's exists
    char _file_path[100];
    strcpy(_file_path, files_dir_path);
    strcat(_file_path, file_name);  // concatenate the file name with files_dir_path
    FILE *file = open_file(_file_path, "r");

    if (file){
        printf("'%s' is already exists\n", file_name);
    } else {
        // just open the file in write mode to create a file
        open_file(_file_path, "w");
    }

}

/*
 * Used to find if a char array is exists in double char array
*/
int in(char *value, char char_array[3][7], int size) {
    for (int i=0; i<size; i++){
        if (!strcmp(value, char_array[i])){
            return 1;
        }
    }
    return 0;
}

/*
 * Displays the currently active todo file
*/
void get_active_todo_file_name(char *file_name){
    int j = 0;

    for (int i=99; active_file_path[i] != '/'; i--){
        if (active_file_path[i] != '\0'){
            file_name[j] = active_file_path[i];
            j++;
        }
    }

    // reverse the array
    reverse(file_name);

}

/*
 * Returns a char array having all the files name in a directory
*/
void get_files_in_dir(char files_name[30][50], char *dir_path, int *count){
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);

    if (d) {
        while ((dir = readdir(d)) != NULL) {
            strcpy(files_name[*count], dir->d_name);
            *count = *count + 1;
        }
    } else {
        printf("Unable to open %s\n", dir_path);
    }

}
