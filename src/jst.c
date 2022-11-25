/* jst -- just in time text jusitfier
    ________   ___        ___  ___   _____ ______    ________   ___   ________    ________
   |\   __  \ |\  \      |\  \|\  \ |\   _ \  _   \ |\   __  \ |\  \ |\   ___  \ |\   ____\
   \ \  \|\  \\ \  \     \ \  \\\  \\ \  \\\__\ \  \\ \  \|\ /_\ \  \\ \  \\ \  \\ \  \___|
    \ \   ____\\ \  \     \ \  \\\  \\ \  \\|__| \  \\ \   __  \\ \  \\ \  \\ \  \\ \  \  ___
     \ \  \___| \ \  \____ \ \  \\\  \\ \  \    \ \  \\ \  \|\  \\ \  \\ \  \\ \  \\ \  \|\  \
      \ \__\     \ \_______\\ \_______\\ \__\    \ \__\\ \_______\\ \__\\ \__\\ \__\\ \_______\
       \|__|      \|_______| \|_______| \|__|     \|__| \|_______| \|__| \|__| \|__| \|_______|
    ___  ___   _________   ___   ___        ________
   |\  \|\  \ |\___   ___\|\  \ |\  \      |\   ____\
   \ \  \\\  \\|___ \  \_|\ \  \\ \  \     \ \  \___|_
    \ \  \\\  \    \ \  \  \ \  \\ \  \     \ \_____  \
     \ \  \\\  \    \ \  \  \ \  \\ \  \____ \|____|\  \
      \ \_______\    \ \__\  \ \__\\ \_______\ ____\_\  \
       \|_______|     \|__|   \|__| \|_______||\_________\
                                              \|_________|

   Plumbing utilities for GNU/Linux.
   https://github.com/fontseca/plumbing-utils

   Copyright (C) 2022 by Jeremy Fonseca <fonseca.dev@outlook.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

/* The official name of this program.  */
#define PROGNAME "jst"

/* Defines an internal type.  */
#define INTERNAL(X) __##X##__internal

/* typedef of an internal file.  */
typedef struct INTERNAL(__file) file_t;

/* typedef of an internal reader.  */
typedef struct INTERNAL(__reader) reader_t;

/* Programn command line options.  */
static const struct option longopts[] =
{
  { "output", required_argument, NULL, 'o' },
  { "help", no_argument, NULL, 'h' },
  { "version", no_argument, NULL, 'v' },
  { (char *)NULL, 0, NULL, 0 }
};

/* Default input buffer length. */
#define DEFAULT_BUFFER_SIZE 256

/* The input buffer for interactive mode.  */
static char *in_buffer = (char *)NULL;

/* Abstracts a file.  */
struct INTERNAL(__file)
{
  /* Previous file.  */
  file_t *prev;

  /* The file name.  */
  const char *name;

  /* Path to the current file.  Appropriate text when
     running in interactive mode.  */
  const char *path;

  /* The actual file content.  */
  char *buffer;

  /* The improved buffer based on the original buffer.  */
  char *altered_buffer;
};

/* Allocates and initializes a new file located at `path'.  */
static file_t *
make_file(const char *const path)
{
  file_t *const pfile = (file_t *) malloc(sizeof(file_t));

  pfile->buffer = (char *)NULL;
  pfile->altered_buffer = (char *)NULL;
  pfile->name = path;

  return pfile;
}

/* Writes into memory the content of a file.  */
static void
file_set_buffer(file_t *const pfile, const char *const buffer)
{
  if (NULL != pfile->buffer)
  {
    return;
  }

  pfile->buffer = strdup(buffer);

  /* Allocate double room to add enough `\n' when
     `file_transform_buffer(...)::max_characters' is reached.  */

  pfile->altered_buffer = (char *)malloc(1 + strlen(pfile->buffer) * 2);
}

/* Transforms the current file buffer into the expected output. `max_characters'
   is the maximun number of characters allowed in a line. */
static void
file_transform_buffer(file_t *const pfile, const uint32_t max_characters)
{
  if (NULL == pfile->buffer
            || NULL == pfile->altered_buffer)
  {
    return;
  }

  char *old = pfile->buffer;
  char *new = pfile->altered_buffer;

  /* Number of characters processed in the current line.  */
  uint32_t nchars = 0;

  next: for (;;)
  {
    if ((nchars) >= max_characters)
    {
      *new++ = '\n', nchars = 0;
      continue;
    }

    switch (*old)
    {

      /* If we have more than one `\n' or `\r', then it means that we have a
         division between paragraphs. So, copy all the contiguous `\n' and
         `\r'.  */

      case '\n': case '\r':
        if ('\n' == *(old + 1)
                  || '\r' == *(old + 1))
        {
          do
          {
            *new++ = *old++;
            if ('\n' ^ *old
                      && '\r' ^ *old)
            {
              nchars = 0; /* Starts at line of the next paragraph.  */
              goto next;
            }
          } while ('\n' ^ *old
                        || '\r' ^ *old);
        }

        /* Not the same when we have only one `\n' or `\r'. In this scenario
           we MUST be in a EOL.  */

        *new = ' ';
        break;

      default:
        *new = *old;
    }

    /* (The proposition `nchars = 0' avoids doing `new - 1' when starting
       line.)  */

    if (nchars && (' ' == *new
                        || '\t' == *new))
    {
      if (' ' == *(new - 1)
                || '\t' == *(new - 1))
      {
        ++old;
        continue;
      }
    }

    if (*old == '\0')
    {
      return;
    }

    ++new, ++old, ++nchars;
  }
}

/* Displays the processed content of a file.  */
static void
file_display_justified_buffer(file_t *const pfile, FILE *const outfile)
{
  if (NULL == pfile->altered_buffer)
  {
    return;
  }

  fprintf(outfile, "\n\n<start: %s>\n\n%s\n\n<end: %s>\n\n",
            pfile->name, pfile->altered_buffer, pfile->name);
}

/* Destroys a file.  */
static void
file_destroy(file_t *const pfile)
{
  free(pfile->buffer);
  free(pfile->altered_buffer);
  free(pfile);
}

/* Abstracts a stack of files.  */
struct INTERNAL(__reader)
{
  /* The top of the stack.  */
  file_t *top;

  /* The count of nodes in the structure.  */
  uint32_t n_files;

  /* The maximum number of characters in a line.  */
  uint32_t max;

  /* Where the output goes.  (Standard output by default.)  */
  FILE *outfile;
};

/* Allocates and initializes a new reader. `max' is the maximum number
   of characters that the justified text of the nodes in the data
   structure have.  `outfile' is where the output of all the processed
   files goes; stdout is selected by default.  */
static reader_t *
make_reader(const uint32_t max, FILE *const outfile)
{
  reader_t *const reader = (reader_t *)malloc(sizeof(reader_t));

  reader->max = max;

  /* Set up output file.  */

  reader->outfile = NULL == outfile ? stdout : outfile;

  reader->n_files = 0;
  reader->top = (file_t *)NULL;

  return reader;
}

/* Pushes a node on the stack.  */
static void
reader_push(reader_t *const preader, file_t *const pfile)
{
  /* When first node.  */

  if (NULL == preader->top)
  {
    preader->top = pfile;
    preader->top->prev = NULL;
  }
  else
  {
    pfile->prev = preader->top;
    preader->top = pfile;
  }

  ++preader->n_files;
}

/* Performs the text transformation on each node in reverse order.
   The function `callback' gets invoked node by node to make the
   expected action.  */
static void
reader_process_files(reader_t *const preader, file_t *const top,
                      void (*const callback)(file_t *, const uint32_t))
{
  if (NULL == top)
  {
    return;
  }

  reader_process_files(preader, top->prev, callback);

  callback(top, preader->max);
}

/* Executes the function `callback' for each node.  */
static void
reader_traverse_nodes(reader_t *const preader, file_t *const top,
                        void (*const callback)(void *, ...))
{
  if (NULL == top)
  {
    return;
  }

  reader_traverse_nodes(preader, top->prev, callback);

  callback(top, preader->outfile);
}

/* Destroys a reader and its file nodes.  */
static void
reader_destroy(reader_t *const preader, file_t *const top,
                    void (*const file_destroyer)(file_t *const))
{
  if (NULL == top)
  {
    return;
  }

  reader_destroy(preader, top->prev, file_destroyer);

  file_destroyer(top);

  if (!--preader->n_files)
  {
    free(preader);
  }
}

/* Emits usage.  */
void
usage(const uint32_t status)
{
  if (EXIT_SUCCESS ^ status)
  {
    fprintf(stderr, "Try '%s --help' for more information.\n", PROGNAME);
  }
  else
  {
    printf("\
Usage: %s [OPTION]... [FILE]...\n\
\n", PROGNAME);

    fputs("\
Just in time plain text justifier.\n\
\n", stdout);

    fputs("\
  -o, --output=outfile         send output to `outfile'\n\
  -h, --help                   display this information\n\
  -v, --version                display current version\n\
\n", stdout);

    fputs("\
Copyright (C) 2022 by Jeremy Fonseca <fonseca.dev@outlook.com>\n", stdout);
  }
 
  exit(status);
}

/* Handles possible signals.  */
static void
signals_handler(const int32_t signum)
{
  switch(signum)
  {
    case SIGTERM: case SIGINT:
      if (NULL != in_buffer)
      {
        free(in_buffer);
      }

      exit(EXIT_SUCCESS);
  }
}

int32_t
main(int32_t argc, char **argv)
{
  FILE *outfile = stdout; /* Default output file.  */
  uint32_t MAX_CHARS = 86; /* Default maximum characters in a line.  */
  uint8_t interactive = 1;
  char c;
  
  /* Files to trasform.  */

  char **files = (char**)NULL;

  while (c = getopt_long(argc, argv, "o:vh", longopts, NULL),
          c ^ -1)
  {
    switch (c)
    {
      case 'o':
        interactive = 0;
        break;
      case 'h':
        usage(EXIT_SUCCESS);
        break;
      default:
        usage(EXIT_FAILURE);
    }
  }
  
  if (interactive)
  {
    /* Only signals should quiet the next loop.  */
   
    signal(SIGTERM, signals_handler);
    signal(SIGINT, signals_handler);

    for (;;)
    {
      /* Flush stdin buffer.  */
      clearerr(stdin);

      /* Number of read bytes.  */
      uint64_t n_read = 0;

      /* Current read byte.  */
      char ch;

      in_buffer = (char *)malloc(DEFAULT_BUFFER_SIZE);

      while (ch = fgetc(stdin), !feof(stdin))
      {

        /* `strcat' might be used here, but it gives some problems for
            very long inputs. */

        *(in_buffer + n_read++) = ch;

        if (0 == n_read % DEFAULT_BUFFER_SIZE)
        {
          in_buffer = (char *)realloc(in_buffer, n_read * 2);
        }
      }

      *(in_buffer + n_read) = '\0';

      file_t *const interactive_file = make_file("interactive");

      file_set_buffer(interactive_file, in_buffer);

      free(in_buffer);

      file_transform_buffer(interactive_file, MAX_CHARS);
      file_display_justified_buffer(interactive_file, outfile);
      file_destroy(interactive_file);
    }
  }

  return EXIT_SUCCESS;
}
