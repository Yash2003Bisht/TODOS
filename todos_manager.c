#define _GNU_SOURCE  // asprintf is part of stdio.h, but need to add
                     // #define _GNU_SOURCE at the top of the file otherwise it gives an error

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Global variable
char active_file_path[120],  // stores the currently active file path
     file_path[120],         // stores 'active_file.txt' path
     *files_dir_path;        // stores todos files directory path

// ----------------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------------


void add_todo(char todo_priority, int todo_size, char todo[][1000]){
    FILE *file = open_file(active_file_path, "a");

    // since max 100 todos are allowed so, the todo id will max go to 3 digit number
    // that's why char_todo_id is created of 4 bytes(1 additional byte for the null character)
    char * priority_format, all_todos[100000], char_todo_id[4];
    int size = sizeof(all_todos), todo_id;

    for (int i=0; i<todo_size; i++){
        fputs(todo[i], file);

        // add a space
        if (i < todo_size-1)
            fputc(' ', file);

    }

    // add todo priority
    asprintf(&priority_format, " [%c] - ", todo_priority);
    fputs(priority_format, file);

    // add todo id
    get_all_todos(all_todos, size);
    todo_id = generate_todo_id(all_todos);
    sprintf(char_todo_id, "%d", todo_id);
    fputs(char_todo_id, file);
    
    // add a new line at the end
    fputc('\n', file);

    // close the file
    fclose(file);
}

void del_todo(int todo_id, int force_del, char all_todos[]){
    int todos_count = num_of_todos(all_todos), match_todo_id, todo_len, flag = 0, count;
    char todos[todos_count][1000], char_todo_id[4], todo[1000];
    seprate_all_todos(all_todos, todos);

    for (int i=0; i<todos_count; i++){
        todo_len = strlen(todos[i]) - 1, count = 0;

        while (todos[i][todo_len] != ' '){
            char_todo_id[count] = todos[i][todo_len];
            count++;
            todo_len--;
        }

        reverse(char_todo_id);
        
        match_todo_id = atoi(char_todo_id);

        if (todo_id == match_todo_id){
            if (todos[i][0] == '*' || force_del == 1){

                if (todos[i][0] == '*')
                    get_formatted_todo(todos[i], todo, 1);
                else
                    get_formatted_todo(todos[i], todo, 0);

                for (int j=i; j<todos_count-1; j++)
                    strcpy(todos[j], todos[j+1]);
                printf("(DELETED) Todo -> %s", todo);
                flag = 1;
            } else {
                flag = -1;
                printf("Todo is not marked as completed\n");
            }
            break;
        }
    }

    if (flag == 1)
        write_all_todos(todos, todos_count - 1);
    else if (flag == 0)
        printf("Todo id not found\n");

}

void del_all_todos(char all_todos[]){
    int todos_count = num_of_todos(all_todos), length, pending_todos_count = 0;
    char todos[todos_count][1000], pending_todos[todos_count][1000];
    seprate_all_todos(all_todos, todos);

    for (int i=0; i<todos_count; i++){
        if (todos[i][0] != '*'){
            strcpy(pending_todos[pending_todos_count], todos[i]);  // copy pending todo to pending_todos char array
            pending_todos_count++;
        }
    }

    write_all_todos(pending_todos, pending_todos_count);
}

void mark_todo(int todo_id, char all_todos[], const char operation[]){
    int todos_count = num_of_todos(all_todos), match_todo_id, todo_len, flag = 0, count;
    char todos[todos_count][1000], char_todo_id[4], todo[1000];
    seprate_all_todos(all_todos, todos);

    for (int i=0; i<todos_count; i++){
        todo_len = strlen(todos[i]) - 1, count = 0;

        while (todos[i][todo_len] != ' '){
            char_todo_id[count] = todos[i][todo_len];
            count++;
            todo_len--;
        }

        reverse(char_todo_id);
        
        match_todo_id = atoi(char_todo_id);

        if (todo_id == match_todo_id){
            if (!strcmp(operation, "--undone")){
                if (todos[i][0] != '*'){
                    printf("Todo is not marked as done\n");
                    flag = -1;
                } else {
                    strcpy(todo, todos[i] + 1);
                    strcpy(todos[i], todo);
                    printf("Todo mark as undone\n");
                    flag = 1;
                }
            } else {
                if (todos[i][0] == '*'){
                    printf("Todo is already marked as done\n");
                    flag = -1;
                } else {
                    todo[0] = '*';
                    strcpy(todo + 1, todos[i]);
                    strcpy(todos[i], todo);
                    printf("Todo mark as done\n");
                    flag = 1;
                }
            }
            break;
        }
    }

    // open file in write mode to delete all content
    if (flag == 1)
        write_all_todos(todos, todos_count);
    else if (flag == 0)
        printf("Todo id not found\n");

}

void generate_report(char all_todos[]){
    char todos[100][1000], todo[1000];
    int count = 1, todos_count = num_of_todos(all_todos);

    // sort todos by priority
    priority_sorting(all_todos, todos, todos_count);

    printf("---------- Completed ----------\n");
    for (int i=0; i<todos_count; i++){
        if (todos[i][0] == '*'){
            get_formatted_todo(todos[i], todo, 1);
            printf("%d) %s", count, todo);
            count++;
        }
    }

    printf("\n");

    printf("---------- Pending ----------\n");
    count = 1;
    for (int i=0; i<todos_count; i++){
        if (todos[i][0] != '*'){
            get_formatted_todo(todos[i], todo, 0);
            printf("%d) %s", count, todo);
            count++;
        }
    }

    printf("\n");

}

void list_pending_todos(char all_todos[], int formatting_required){
    char todos[100][1000], todo[1000];
    int count = 1, flag = 1, todos_count = num_of_todos(all_todos);
    
    // sort todos by priority
    priority_sorting(all_todos, todos, todos_count);

    if (formatting_required){
        for (int i=0; i<todos_count; i++){
            if (todos[i][0] == '*') continue;  // skip this todo
            else {
                get_formatted_todo(todos[i], todo, 0);
                printf("%d) %s\n", count, todo);
                count++;
            }
        }
    } else {
        for (int i=0; i<todos_count; i++){
            if (todos[i][0] == '*') continue;  // skip this todo
            else {
                printf("%d) %s\n", count, todos[i]);
                count++;
            }
        }
    }
}

void list_completed_todos(char all_todos[], int formatting_required){
    char todos[100][1000], todo[1000];
    int count = 1, flag = 1, todos_count = num_of_todos(all_todos);
    
    // sort todos by priority
    priority_sorting(all_todos, todos, todos_count);

    if (formatting_required){
        for (int i=0; i<todos_count; i++){
            if (todos[i][0] == '*'){
                get_formatted_todo(todos[i], todo, 1);
                printf("%d) %s\n", count, todo);
                count++;
            }
        }
    } else {
        for (int i=0; i<todos_count; i++){
            if (todos[i][0] == '*'){
                printf("%d) %s\n", count, todos[i]);
                count++;
            }
        }
    }
}

int main(int argc, char const *argv[]){

    char base_command[10], all_todos[100000],
         // Stores commands that do not require the 'get_all_todos' function to be run
         preload_not_required_commands[3][7] = {"add", "use", "create"};
    int start = 0, size = sizeof(all_todos), todo_id, command_match;

    if (argc > 1){
        strcpy(base_command, argv[1]);
        command_match = in(base_command, preload_not_required_commands, 3);

        // Set the currently active file name
        set_active_file_name();

        if (!command_match){
            // Get all the todos
            get_all_todos(all_todos, size);
        }

    } else{
        printf("No command passed\n");
        return 0;
    }

    /*
        --------------- EXECUTOR COMMANDS ---------------
    */

    if (!strcmp(base_command, "add")){
        if (argc <= 3){
            printf("Priority & todo message is required\n");
        } else{
            size = argc-3;
            char todo[size][1000], priority = argv[2][0];

            // copy the todo to a new char array
            for (int i=0; i < size; i++)
                strcpy(todo[i], argv[i+3]);        

            add_todo(priority, size, todo);
            printf("Todo added\n");
        }
    }

    else if (!strcmp(base_command, "done")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else{
            todo_id = atoi(argv[2]);
            if (argc == 4)
                mark_todo(todo_id, all_todos, argv[3]);
            else
                mark_todo(todo_id, all_todos, "--done");
        }
    }

    else if (!strcmp(base_command, "del")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else if (!strcmp(argv[2], "--all")){
            char confirmation[2];

            // take a second confirmation from the user
            printf("Please confirm(y/n) ");
            scanf("%s", confirmation);

            if (!strcmp(confirmation, "y")){
                del_all_todos(all_todos);
                printf("All completed todos deleted\n");
            }


        } else{
            todo_id = atoi(argv[2]);
            int force_del;

            // check if "--force" delete flag is used
            if (argc >= 4){
                if (!strcmp(argv[3], "--force"))
                    force_del = 1;
                else
                    force_del = 0;
            }

            del_todo(todo_id, force_del, all_todos);
        }
    }

    else if (!strcmp(base_command, "report")){
        generate_report(all_todos);
    }

    else if (!strcmp(base_command, "ls")){
        if (argc <= 2){
            list_pending_todos(all_todos, 0);
        } else {
            if (!strcmp(argv[2], "--format") && argc <= 3)
                list_pending_todos(all_todos, 1);
            else if (!strcmp(argv[2], "--done") && argc <= 3)
                list_completed_todos(all_todos, 0);
            else if ((!strcmp(argv[2], "--done") && !strcmp(argv[3], "--format")) ||
                      !strcmp(argv[2], "--format") && !strcmp(argv[3], "--done"))
                list_completed_todos(all_todos, 1);
            else
                printf("Invalid flag use\n");
        }
    }

    else if (!strcmp(base_command, "use")){
        if (argc < 3) {
            printf("File name is missing\n");
        } else {
            char *file_name = (char *) argv[2];
            update_active_todo_file(file_name);
        }
    }

    else if (!strcmp(base_command, "create")){
        if (argc < 3) {
            printf("File name is missing\n");
        } else {
            char *file_name = (char *) argv[2];
            create_todo_file(file_name);
        }
    }

    else if (!strcmp(base_command, "active")){
        if (argc < 3) {
            char file_name[100];
            get_active_todo_file_name(file_name);
            printf("%s\n", file_name);
        } else if (!strcmp(argv[2], "--path")) {
            printf("%s\n", active_file_path);
        } else {
            printf("Invalid flag use\n");
        }
    }

    else if (!strcmp(base_command, "help")){
        char * help = "Usage :-\n \
$ todo add 2 hello world    # Add a todo with priority 2\n \
$ todo ls                   # Show all incompleted todos sorted by priority in ascending order\n \
$ todo ls --format          # --format for formatting\n \
$ todo ls --done            # Show all completed todos sorted by priority in ascending order\n \
$ todo del ID               # Delete a completed todo with the given ID\n \
$ todo del ID --force       # Use --force flag to delete a uncompleted todo\n \
$ todo del --all            # Delete all completed todo\n \
$ todo done ID              # Mark the incomplete todo with the given ID as complete\n \
$ todo done ID --undone     # Mark the complete todo with the given ID as incomplete\n \
$ todo report               # Statistics\n \
$ todo create FILE_NAME     # Create a new todo file\n \
$ todo use FILE_NAME        # Set the defualt TODO file\n \
$ todo help                 # Show usage\n";

        printf("%s", help);

    } else {
        printf("Invalid command\n");
    }

    return 0;
}
