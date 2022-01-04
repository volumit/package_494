#include "numbering.h"
#include "libiberty.h"
#include "string.h"
#include <stdlib.h>

#ifdef HTC_SUPPORT
#if HAVE_LIBINTL_H
#include <libintl.h>
#else
extern char *bindtextdomain (const char *, const char *);
extern char *textdomain (const char *);
#endif

static int htc_numbering = 0;

void set_error_numbering()
{
	htc_numbering = 1;
}

int is_error_numbering()
{
	return htc_numbering;
}

const char*
get_localedir (char* binpath,
	       char* bindir,
	       char* toolbindir,
	       char* localedir)
{
  char *path = make_relative_prefix (binpath, "/", "/");
  char *tooldirname = xstrdup (toolbindir);
  char *c;

  path = concat (path, "xx", (char *) 0);

  if ((path[0] == '/') || ((path[1] == ':') && (path[2] == '\\')))
    {
      /* Cut name of tooldir */
      c = tooldirname + strlen (tooldirname) - strlen ("/bin");

      *c = '\0';
      while ((*c != '/') && (*c != '\\'))
        --c;

      ++c;
      tooldirname = c;

      /* Scroll to the parent of the bin dir in the path */
      c = path + strlen (path) - strlen("/bin/xx") - 1;
      while ((*c != '/') && (*c != '\\') && (c != path))
        --c;

      ++c;

      /* Set the localedir depending on which binary is used*/

      if (strncmp (c, tooldirname, strlen (tooldirname)))
        return make_relative_prefix (path, bindir, localedir);
      else
        return make_relative_prefix (path, toolbindir, localedir);
    }
  else
    return localedir;
}

void set_language_domain(const char* domain, const char* localedir)
{
  char *language_domain = concat (domain, is_error_numbering() ? "-numbering" : "", (char *) 0);

  bindtextdomain(language_domain, localedir);
}

void check_numbering_option (int argc, char **argv)
{
  int i;

  for (i = 1; i < argc; ++i)
    if (strcmp (argv[i], "--error-numbers") == 0)
      set_error_numbering();
}

void set_locale_numbering (const char *package, const char* localedir)
{
    /* Force gettext to use "en" locale. */
  setenv("LANGUAGE", "en", 1);

  set_language_domain (package, localedir);
  set_language_domain ("bfd", localedir);
  set_language_domain ("opcodes", localedir);
  textdomain (concat (package, is_error_numbering() ? "-numbering" : "", (char *) 0));
}
#endif
