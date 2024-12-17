/**
 * Copyright 2024 Enveng Group - Simon French-Bluhm and Adrian Gallo.
 * SPDX-License-Identifier: 	AGPL-3.0-or-later
 */
/* src/shell.c */
#include "../include/shell.h"
#include "../include/app_error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define SHELL_PROMPT "$ "

static char command_buffer[MAX_CMD_LEN];
static char *args[MAX_ARGS + 1]; /* +1 for NULL terminator */

static int parseCommand(const char *cmd);
static int executeBuiltin(const char *cmd);

int
shellInit(void)
{
    /* Clear command buffer and args array */
    memset(command_buffer, 0, sizeof(command_buffer));
    memset(args, 0, sizeof(args));

    return 0;
}

void
shellPrompt(void)
{
    size_t len;

    /* Display prompt and get command */
    printf("%s", SHELL_PROMPT);
    fflush(stdout);

    if (fgets(command_buffer, sizeof(command_buffer), stdin) == NULL)
    {
        if (feof(stdin))
        {
            printf("\n");
            exit(EXIT_SUCCESS);
        }
        return;
    }

    /* Remove trailing newline */
    len = strlen(command_buffer);
    if (len > 0 && command_buffer[len - 1] == '\n')
    {
        command_buffer[len - 1] = '\0';
    }

    /* Execute command */
    shellExecuteCommand(command_buffer);
}

int
shellExecuteCommand(const char *command)
{
    pid_t pid;
    int status;

    if (command == NULL || *command == '\0')
    {
        return CMD_SUCCESS;
    }

    /* Parse command into arguments */
    if (parseCommand(command) != 0)
    {
        return CMD_ERROR;
    }

    /* Check for built-in commands first */
    if (executeBuiltin(args[0]) == CMD_SUCCESS)
    {
        return CMD_SUCCESS;
    }

    /* Fork and execute external command */
    pid = fork();
    if (pid == -1)
    {
        errorLog(ERROR_CRITICAL, "Fork failed");
        return CMD_ERROR;
    }

    if (pid == 0)
    {
        /* Child process */
        execvp(args[0], args);
        /* If execvp returns, command was not found */
        fprintf(stderr, "Command not found: %s\n", args[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        /* Parent process */
        if (waitpid(pid, &status, 0) == -1)
        {
            errorLog(ERROR_WARNING, "Wait failed");
            return CMD_ERROR;
        }
        /* Check if child process exited normally */
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            return CMD_NOT_FOUND;
        }
    }

    return CMD_SUCCESS;
}

void
shellShutdown(void)
{
    /* Clear sensitive data */
    memset(command_buffer, 0, sizeof(command_buffer));
    memset(args, 0, sizeof(args));
}

static int
parseCommand(const char *cmd)
{
    char temp[MAX_CMD_LEN];
    char *token;
    char *saveptr;
    size_t cmd_len;
    size_t arg_count;

    /* Input validation */
    if (cmd == NULL) {
        return -1;
    }

    /* Get command length and check bounds */
    cmd_len = strlen(cmd);
    if (cmd_len >= MAX_CMD_LEN) {
        return -1;
    }

    /* Make safe copy of command */
    memcpy(temp, cmd, cmd_len);
    temp[cmd_len] = '\0';

    /* Reset args array */
    memset(args, 0, sizeof(args));
    arg_count = 0;

    /* Parse command into arguments */
    token = strtok_r(temp, " \t", &saveptr);
    while (token != NULL && arg_count < MAX_ARGS) {
        /* Copy token to args array */
        args[arg_count] = token;
        arg_count++;
        token = strtok_r(NULL, " \t", &saveptr);
    }

    /* Ensure args array is NULL-terminated */
    args[arg_count] = NULL;

    return 0;
}

static int
executeBuiltin(const char *cmd)
{
    if (strcmp(cmd, "exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(cmd, "cd") == 0)
    {
        if (args[1] == NULL)
        {
            fprintf(stderr, "cd: missing argument\n");
            return CMD_ERROR;
        }
        if (chdir(args[1]) != 0)
        {
            perror("cd");
            return CMD_ERROR;
        }
        return CMD_SUCCESS;
    }

    return CMD_NOT_FOUND;
}
