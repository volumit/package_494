#ifndef NUMBERING_H
#define NUMBERING_H
#ifdef HTC_SUPPORT
extern void set_error_numbering (void);

extern int is_error_numbering (void);

extern const char* get_localedir (char*, char*, char*, char*);

extern void set_language_domain (const char*, const char*);

extern void check_numbering_option (int, char **);

extern void set_locale_numbering (const char*, const char*);
#endif
#endif 
