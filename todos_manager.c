#define _GNU_SOURCE  // asprintf is part of stdio.h, but need to add
                     // #define _GNU_SOURCE at the top of the file otherwise it gives an error

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


FILE * open_file(char file_name[], const char *mode){
    FILE *file_pointer = fopen(file_name, mode);
    return file_pointer;
}

void get_all_todos(char all_todos[], int size){
    FILE *file = open_file("TODO", "r");
    fread(all_todos, size, 1, file);
    fclose(file);
}

void write_all_todos(char todo[][1000], int size){
    FILE *write_file = open_file("TODO", "w");

    for (int i=0; i<size; i++){
        fputs(todo[i], write_file);
        fputc('\n', write_file);
    }

    fclose(write_file);

}

void add_todo(char todo_priority, int todo_size, char todo[][1000]){
    FILE *file = open_file("TODO", "a");
    char * priority_format;

    for (int i=0; i<todo_size; i++){
        fputs(todo[i], file);
        
        // add a space
        if (i < todo_size-1)
            fputc(' ', file);
        
        // add todo priority
        else{
            asprintf(&priority_format, " [%c]", todo_priority);
            fputs(priority_format, file);
        }

    }
    
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
    int flag = 0, d = 0, p = 0, k = 0;

    for (int i=0; all_todos[i] != '\0'; i++){
        if (all_todos[i] == '*'){
            flag = 1;
            i++;
        }
        
        if (flag){
            if (all_todos[i] == '\n'){
                flag = 0;
                k = 0;
                d++;
            } else{
                done_todos[d][k] = all_todos[i];
                k++;
            }
        } else {
            if (all_todos[i] == '\n'){
                k = 0;
                p++;
            } else {
                pending_todos[p][k] = all_todos[i];
                k++;
            }
        }
    }

    printf("\nCompleted Todos:\n");
    if (d > 0){
        for (int i=0; i<d; i++)
            printf("%s\n", done_todos[i]);
    } else {
        printf("Nothing to show here\n");
    }

    printf("\nPending Todos:\n");
    if (p > 0){
        for (int i=0; i<p; i++)
            printf("%s\n", pending_todos[i]);
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
        char all_todos[100000];
        int size = sizeof(all_todos);
        get_all_todos(all_todos, size);

        for (int i=0; all_todos[i] != '\0'; i++){

            if (all_todos[i] == '*'){
                // skip this todo
                while (all_todos[i] != '\n') i++;
                i++;
            }
            printf("%c", all_todos[i]);
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
