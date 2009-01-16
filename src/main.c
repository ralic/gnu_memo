/*
 * Copyright (C) 2008, 2009 Jan Stępień
 *
 * This file is part of Memo.
 *
 * Memo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Memo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Memo.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libmemo.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "macros.h"
#include "xmalloc.h"
#include "messages.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#error "Error: config.h is required, but HAVE_CONFIG_H isn't defined."
#endif

char *program_name;
const char usage[] = "usage: %s [COMMAND [ARGS]]\n";

typedef struct {
	const char *option_name;
	void (*action)(int, const char*[]);
} action;

int
open_database(memo_database *db) {
	char *filename, memo_db_file[] = "/.memo/db", *homedir;
	int homedir_len;
	homedir = getenv("HOME");
	homedir_len = strlen(homedir);
	filename = xmalloc(sizeof(char)*(homedir_len+ARRAY_SIZE(memo_db_file)));
	strcat(filename, homedir);
	strcat(filename + homedir_len, memo_db_file);
	if (access(filename, R_OK | W_OK) == -1 && errno != ENOENT) {
		perror("Failed to open db file for r/w");
		exit(1);
	}
	if (memo_database_load(db, filename) < 0)
		exit(1);
	return 0;
}

void
add_pair(int argc, const char *argv[]) {
	memo_database db;
	memo_word *word[2];
	int i;
	if (argc != 2 || strlen(argv[0]) < 1 || strlen(argv[1]) < 1)
		die("usage: %s --add-pair FIRST_ITEM SECOND_ITEM\n", program_name);
	open_database(&db);
	for (i = 0; i < 2; i++)
		if ((word[i] = memo_word_find_by_value(db, argv[i])) == NULL)
			if ((word[i] = memo_word_new(db)) == NULL ||
					memo_word_set_value(word[i], argv[i]) ||
					memo_word_save(word[i]))
				die("Cannot create a new word '%s'\n", argv[i]);
	if (memo_word_add_translation(word[0], word[1]))
		die("Cannot create the translation\n");
	memo_database_close(db);
}

void
print_usage() {
	printf(usage, program_name);
	exit(0);
}

void
print_help() {
	const char help_message[] = \
		"Possible commands are:\n"\
		"  --help       Prints this message\n"\
		"  --usage      Displays brief usage information\n"\
		"  --version    Displays version information\n"\
		"  --add-pair   Adds a new pair of phrases to the database\n"\
		"";
	printf(usage, program_name);
	printf(help_message);
	exit(0);
}

void
print_version() {
	const char version[] = PACKAGE_NAME" version "PACKAGE_VERSION"\n"\
		"Copyright (C) 2008, 2009 Jan Stępień\n";
	printf(version, program_name);
	exit(0);
}

int
parse_args(int argc, const char *argv[], action actions[]) {
	int i;
	for (i = 0; actions[i].option_name != NULL; i++) {
		if (strcmp(argv[0], actions[i].option_name) == 0) {
			actions[i].action(argc-1, argv+1);
			return 0;
		}
	}
	return -1;
}

int
parse_main_args(int argc, const char *argv[]) {
	action actions[] = {
		{"--help", &print_help},
		{"--usage", &print_usage},
		{"--version", &print_version},
		{"--add-pair", &add_pair},
		{NULL, NULL}
	};
	program_name = (char*) argv[0];
	if (argc == 1)
		print_usage();
	if (parse_args(argc-1, argv+1, actions) == -1) {
		fprintf(stderr, "Unknown argument: %s\n", argv[1]);
		exit(1);
	}
}

int
main (int argc, const char *argv[]) {
	parse_main_args(argc, argv);
	return 0;
}
