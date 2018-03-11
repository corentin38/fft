/**
 * @autor <corentin@marciau.fr>
 */

#include "util.h"

int ispow2 (int x) {
	return x!=0 && !(x & (x-1));
}
