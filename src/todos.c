#include "utils.c"

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

void list_all_todos_files(){
    int total_files;
    char files[30][50];
    get_files_in_dir(files, files_dir_path, &total_files);

    for (int i=0; i<total_files; i++){
        // skip files starts with '.'
        if (files[i][0] == '.')
            continue;
        printf("%s\n", files[i]);
    }

}
