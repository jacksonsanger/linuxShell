#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1024

//handler for child processes ending
void handler(int sig) {
    int status;
    waitpid(-1, &status, WNOHANG); //-1 param means any child process, and WNOHANG allows to return immediately without waiting to finish
}

void print_prompt() {
    char cwd[BUFFER_SIZE];
    getcwd(cwd, sizeof(cwd));
    printf("%s> ", cwd);
}

void handle_cd(char* path) {
    if (chdir(path) != 0) {
        perror("chdir() error");
    }
}

//helper function for checking for output redirection and preparing the tokens for it
int check_output_redirection(char** tokens, int* token_count, char* output_file) {
    for (int i = 0; i < *token_count; i++) {
        if (strcmp(tokens[i], ">") == 0) {
            //ensure the ">" is not the last token
            if (i + 1 < *token_count) {
                // Copy the output file name (next token) to the output_file variable
                strcpy(output_file, tokens[i + 1]);

                // Free the memory for '>' and the filename tokens
                free(tokens[i]);
                free(tokens[i + 1]);

                // Remove '>' and the filename from the tokens array. 
                //this loop starts at the token where it found the ">" character and then checks two spaces beyond to ensure there are still valid tokens to shift left
                for (int j = i; j + 2 <= *token_count; j++) {
                    tokens[j] = tokens[j + 2];
                }

                *token_count -= 2;
                return 1; // Output redirection found
            }
            else {
                printf("Output file argument required.\n");
                return 0;
            }
        }
    }
    return 0; // No output redirection found
}

int check_input_redirection(char** tokens, int* token_count, char* input_file) {
    for (int i = 0; i < *token_count; i++) {
        if (strcmp(tokens[i], "<") == 0) {
            //ensure the "<" is not the last token
            if (i + 1 < *token_count) {
                // Copy the input file name to the input_file variable
                strcpy(input_file, tokens[i + 1]);

                // Free the memory for '>' and the filename tokens
                free(tokens[i]);
                free(tokens[i + 1]);

                // Remove '<' and the filename from the tokens array
                for (int j = i; j + 2 <= *token_count; j++) {
                    tokens[j] = tokens[j + 2];
                }

                *token_count -= 2;
                return 1; // Input redirection found
            }
            else {
                printf("Input file argument required.\n");
                return 0;
            }
        }
    }
    return 0; // No input redirection found
}


int main() {
    //call signal to redirect when signal is sent
    signal(SIGCHLD, handler);

    //input is an array of chars (same as char*)
    char* input = (char *)malloc(BUFFER_SIZE * sizeof(char));
    //allocate memory for input and output file args
    char* input_file = (char*)malloc(BUFFER_SIZE * sizeof(char));
    char* output_file = (char*)malloc(BUFFER_SIZE * sizeof(char));
    
    while (1) {
        // Step 1: Display the current working directory followed by >
        print_prompt();

        //use fgets, will store user input in "input"
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break; // Exit on EOF or error
        }
        
        // Step 3: Tokenize the input string
        int token_count = 0;
        //tokens is an array of strings (char**)
        char** tokens = (char **)malloc(BUFFER_SIZE * sizeof(char*));
        char* token;
        //get the first token and store it
        //strtok returns a pointer to the original string, but with a null terminator after the first token
        token = strtok(input, " \t\n");
        while (token != NULL) {
            // Allocate memory for each token
            tokens[token_count] = (char*)malloc(strlen(token) + 1);
            //copy the token into the allocated memory
            strcpy(tokens[token_count], token);
            token_count++;
            //set string to null in next call to continue tokenizing same string
            token = strtok(NULL, " \t\n");
        }
        tokens[token_count] = NULL; // Last element should be NULL for execvp to be used


        // Check for background process ('&' at the end of tokens)
        int run_in_background = 0;
        if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
            run_in_background = 1;
            free(tokens[token_count - 1]); //free the memory allocated for that token
            tokens[token_count - 1] = NULL; //set to null for execvp
            token_count--; // Adjust token count
        }
        
        //Check for built-in commands: exit and cd
        if (tokens[0] == NULL) {
            // No command entered, free memory allocated for tokens and reprompt
            free(tokens);
            continue; 
        } 
        //process exit
        else if (strcmp(tokens[0], "exit") == 0) {
             // Free all allocated memory before breaking out of the loop
            for (int i = 0; i < token_count; i++) {
                free(tokens[i]);
            }
            free(tokens);
            break; // Exit the shell
        } 
        //process cd
        else if (strcmp(tokens[0], "cd") == 0) {
            if (tokens[1] != NULL) {
                handle_cd(tokens[1]);
            } 
            else {
                printf("cd: expected argument\n");
            }
        } //process programs
        else {

            //check input and output redirect
            int redirect_input = check_input_redirection(tokens, &token_count, input_file);
            int redirect_output = check_output_redirection(tokens, &token_count, output_file);

            int pid = fork();  // Fork a new process

            if (pid == 0) {
                //we are the child process

                if (redirect_input) {
                    freopen(input_file, "r", stdin);
                }

                if (redirect_output) {
                    freopen(output_file, "w", stdout);
                }

                execvp(tokens[0], tokens);
                //this code below won't execute unless execvp fails
                printf("execvp failed, invalid program name\n");
                exit(EXIT_FAILURE); // Terminates the child process
            }
            else { //(pid greater then 0)
                if (!run_in_background) {
                    // means we are parent process
                    int status;
                    waitpid(pid, &status, 0); // Wait for the child process to finish if run in foreground
                }
                else {
                    printf("[Background process started with PID %d]\n", pid);
                }
            }
        }

        // Free the memory allocated for each token, and the token list on every iteration
        for (int i = 0; i < token_count; i++) {
            free(tokens[i]);
        }
        free(tokens);
    }

    //free all leftover memory
    free(input);
    free(input_file);
    free(output_file);
    
    return 0;
}
