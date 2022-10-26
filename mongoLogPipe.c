#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libconfig.h>
#include <mongoc/mongoc.h>

void print_usage() {
	printf("\
Usage:   mongoLogPipe [OPTIONS]\n\
\n\
Options:\n\
  -f config		 Config file path \n\
  -s server      Full DSN for the MongoDB server with username and password\n\
  -d database    Database name\n\
  -c collection  Name of collection to write the logs to\n\
  -t tag         Optional. Value for a tag field in each document.\n\
                 Allows multiple processes to write to the same collection while\n\
                 still maintaining the information which record came from where.\n\
  -h             Print usage information\n\
\n\
You must either specify a config file with '-f config' or provide server dsb, database\n\
name collection name with the respective options\n\
");
}

int main (int argc, char *argv[]) {
	char * dbdsn = NULL;
	char * database_name = NULL;
	char * collection_name = NULL;
	char * tag = NULL;
	char * config_file_name = NULL;
	config_t config_file;

	int opt;
	while ((opt = getopt(argc, argv, "hf:s:d:c:t:")) != 1) {
		switch (opt) {
			case 'h':
				print_usage();
				exit(EXIT_SUCCESS);
				break;
			case 'f':
				config_file_name = optarg;
				break;
			case 's':
				dbdsn = optarg;
				break;
			case 'd':
				database_name = optarg;
				break;
			case 'c':
				collection_name = optarg;
				break;
			case 't':
				tag = optarg;
				break;
			case -1:
				break;
			default:
				print_usage();
				exit(EXIT_FAILURE);
		}
		if (opt == -1) {
			break;
		}
	}

	if (config_file_name != NULL) {
		printf("Reading config file %s\n", config_file_name);
		config_init(&config_file);
		char * tmp;
		int error = 0;
		if (!config_read_file(&config_file, config_file_name)) {
			fprintf(stderr, "%s:%d - %s\n", config_error_file(&config_file), config_error_line(&config_file), config_error_text(&config_file));
			error = 1;
		}
		if(!error && !config_lookup_string(&config_file, "dsn", (const char **)&tmp)) {
			fprintf(stderr, "No DSN specified in config file");
			error = 1;
		}
		dbdsn = strdup(tmp);
		if(!error && !config_lookup_string(&config_file, "db", (const char **)&tmp)) {
			fprintf(stderr, "No db specified in config file");
			error =1;
		}
		database_name = strdup(tmp);
		if(!error && !config_lookup_string(&config_file, "collection", (const char **)&tmp)) {
			fprintf(stderr, "No collection specified in config file");
			error =1;
		}
		collection_name = strdup(tmp);
		config_destroy(&config_file);
		if (error > 0) {
			exit(EXIT_FAILURE);
		} else {
			printf("Config file read\n");
		}
	} else {
		if (dbdsn == NULL) {
			fprintf(stderr, "No DSN specified\n");
			print_usage();
			exit(EXIT_FAILURE);
		}
		if (database_name == NULL) {
			fprintf(stderr, "No database name specified");
			print_usage();
			exit(EXIT_FAILURE);
		}
		if (collection_name == NULL) {
			fprintf(stderr, "No collection name specified\n");
			print_usage();
			exit(EXIT_FAILURE);
		}
	}

	mongoc_database_t *database;
	mongoc_client_t *client;
	mongoc_collection_t *collection;

	mongoc_init ();

	//printf("Using DSN %s\n", dbdsn);

	client = mongoc_client_new(dbdsn);
	if (client == NULL) {
		fprintf(stderr, "Could not connect database server\n");
		goto EXIT;
	}
	database = mongoc_client_get_database (client, database_name);
	if (database == NULL) {
		fprintf(stderr, "Could not load database");
		goto EXIT;
	}

	collection = mongoc_database_get_collection (database, collection_name);
	if (collection == NULL) {
		fprintf(stderr, "Could not load collection");
		goto EXIT;
	}

	ssize_t nread;
	size_t len = 0;
	char *line = NULL;

	bson_error_t error;
	bson_oid_t oid;
	bson_t *doc;
	while (!feof(stdin)) {
		if ((nread = getline(&line, &len, stdin)) > 1) {
			doc = bson_new ();
			bson_oid_init (&oid, NULL);
			bson_append_oid (doc, "_id", -1, &oid);
			bson_append_utf8 (doc, "message", -1, line, nread-1);	// substract 1 from the length for the \n
			bson_append_now_utc(doc, "datetime", -1);
			if (tag != NULL) {
				bson_append_utf8(doc, "tag", -1, tag, -1);
			}

			if (!mongoc_collection_insert_one (collection, doc, NULL, NULL, &error)) {
				// fprintf(stderr, "%s", error.message);
				break;
			}
			bson_destroy(doc);
		}
	}
	if (line != NULL) {
		free(line);
	}

EXIT:
	mongoc_database_destroy (database);
	mongoc_client_destroy (client);
	mongoc_cleanup ();
	return 0;
}
