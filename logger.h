/**
 * @autor <corentin@marciau.fr>
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

extern int verbose;

void logm (const char *fmt, ...);
void logw (const char *fmt, ...);
void loge (const char *fmt, ...);

#endif /* __LOGGER_H__ */
