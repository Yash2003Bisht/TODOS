#define _GNU_SOURCE  // asprintf is part of stdio.h, but need to add
                     // #define _GNU_SOURCE at the top of the file otherwise it gives an error

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------------

FILE * open_file(char file_name[], const char *mode){
    FILE *file_pointer = fopen(file_name, mode);
    return file_pointer;
}

void write_all_todos(char todo[][1000], int size){
    FILE *write_file = open_file("TODO", "w");

    for (int i=0; i<size; i++){
        fputs(todo[i], write_file);
        fputc('\n', write_file);
    }

    fclose(write_file);

}

void get_all_todos(char all_todos[], int size){
    FILE *file = open_file("TODO", "r");
    fread(all_todos, size, 1, file);
    fclose(file);
}

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

int num_of_todos(char all_todos[]){
    int total_todos = 0;
    for (int i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '\n') total_todos++;
    }
    return total_todos;
}

int generate_todo_id(char all_todos[]){
    int total_todos = num_of_todos(all_todos);
    return total_todos + 1;
}

void get_todos_priority(char all_todos[], int priority[]){
    int j = 0;
    for (int i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '['){
            char temp[2];
            temp[0] = all_todos[i+1];
            priority[j] = atoi(temp);
            j++;
        }
    }
}

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

// ----------------------------------------------------------------------------------


void add_todo(char todo_priority, int todo_size, char todo[][1000]){
    FILE *file = open_file("TODO", "a");

    // since max 100 todos are allowed so, the todo id will max go to 3 digit number
    // that's why char_todo_id is created of 4 bytes
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

void del_todo(int index){
    char all_todos[100000], todo[100][1000];
    int j = 0, k = 0, size = sizeof(all_todos);

    // get all todos
    get_all_todos(all_todos, size);

    for (int i=0; all_todos[i] != '\0'; i++){
        if (index-1 == j){
            if (all_todos[i] == '*'){
                // skip this todo
                while (all_todos[i] != '\n') i++;
            } else {
                printf("Todo not marked as completed\n");
                i--;
            }
            index = -1;
        } else {
            if (all_todos[i] == '\n'){
                j++;
                k = 0;
            } else{
                todo[j][k] = all_todos[i];
                k++;
            }
        }
    }

    // open file in write mode to delete all content
    write_all_todos(todo, j);

}

void mark_as_done(int index){
    char all_todos[100000], todo[100][1000];
    int j = 0, k = 0, size = sizeof(all_todos);

    // get all todos
    get_all_todos(all_todos, size);

    for (int i=0; all_todos[i] != '\0'; i++){
        if (index-1 == j){
            if (all_todos[i] != '*'){
                todo[j][k] = '*';
                k++;
                i--;
                index = -1;
            } else {
                printf("Todo already marked as done\n");
            }
        } else if (all_todos[i] == '\n'){
            j++;
            k = 0;
        } else{
            todo[j][k] = all_todos[i];
            k++;
        }
    }

    // open file in write mode to delete all content
    write_all_todos(todo, j);

}

void genrate_report(char all_todos[]){
    char done_todos[100][1000], pending_todos[100][1000];
    int flag = 0, done_count = 0, pending_count = 0, k = 0;

    for (int i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '*'){
            flag = 1;
            i++;
        }
        
        if (flag){
            if (all_todos[i] == '\n'){
                flag = 0;
                k = 0;
                done_count++;
            } else{
                done_todos[done_count][k] = all_todos[i];
                k++;
            }
        } else {
            if (all_todos[i] == '\n'){
                k = 0;
                pending_count++;
            } else {
                pending_todos[pending_count][k] = all_todos[i];
                k++;
            }
        }
    }

    printf("\nCompleted Todos:\n");
    if (done_count > 0){
        k = 1;
        for (int i=0; i<done_count; i++){
            printf("%d) %s\n", k, done_todos[i]);
            k++;
        }
    } else {
        printf("Nothing to show here\n");
    }

    printf("\nPending Todos:\n");
    if (pending_count > 0){
        k = 1;
        for (int i=0; i<pending_count; i++){
            printf("%d) %s\n", k, pending_todos[i]);
            k++;
        }
    } else {
        printf("Nothing to show here\n");
    }
    
    printf("\n");

}

int main(int argc, char const *argv[]){

    char executor_command[100];
    
    if (argc > 1)
        strcpy(executor_command, argv[1]);
    
    else {
        printf("Command not passed\n");
        return 0;
    }

    /*
        --------------- EXECUTOR COMMANDS ---------------
    */

    if (!strcmp(executor_command, "add")){
        int size = argc-3;
        char todo[size][1000], priority = argv[2][0];

        // copy the todo to a new char array
        for (int i=0; i < size; i++)
            strcpy(todo[i], argv[i+3]);        

        add_todo(priority, size, todo);

    }

    else if (!strcmp(executor_command, "del")){
        int todo_index = atoi(argv[2]);
        del_todo(todo_index);
    }

    else if (!strcmp(executor_command, "done")){
        int todo_index = atoi(argv[2]);
        mark_as_done(todo_index);
    }

    else if (!strcmp(executor_command, "report")){
        char all_todos[100000];
        int size = sizeof(all_todos);
        get_all_todos(all_todos, size);
        genrate_report(all_todos);
    }

    else if (!strcmp(executor_command, "ls")){
        char all_todos[100000], todos[100][1000];
        int size = sizeof(all_todos), * priority, count = 1, flag = 1, todos_count;
        get_all_todos(all_todos, size);

        todos_count = num_of_todos(all_todos);
        priority = (int *) malloc(sizeof(int) * todos_count);
        get_todos_priority(all_todos, priority);  // extract all priority from todos and save them to an array
        seprate_all_todos(all_todos, todos);  // store todos into 2d array
        sort_by_priority(todos, priority, todos_count);  // sort todos by priority

        for (int i=0; i<todos_count; i++){
            if (todos[i][0] == '*') continue;  // skip this todo
            else {
                printf("%d) %s\n", count, todos[i]);
                count++;
            }
        }
    }

    else if (!strcmp(executor_command, "help")){
        char * help = "Usage :-\n \
$ todo add 2 hello world    # Add a todo with priority 2\n \
$ todo ls                   # Show all incompleted todos sorted by priority in ascending order\n \
$ todo del INDEX            # Delete a incomplete todo with the given index\n \
$ todo done INDEX           # Mark the incomplete todo with the given index as complete\n \
$ todo report               # Statistics\n \
$ todo help                 # Show usage\n";
        
        printf("%s", help);

    }

    return 0;
}
