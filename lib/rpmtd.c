#include "system.h"

#include <rpm/rpmtd.h>
#include <rpm/rpmstring.h>

#include "debug.h"

rpmtd rpmtdNew(void)
{
    rpmtd td = xmalloc(sizeof(*td));
    rpmtdReset(td);
    return td;
}

rpmtd rpmtdFree(rpmtd td)
{
    /* permit free on NULL td */
    if (td != NULL) {
	/* XXX should we free data too - a flag maybe? */
	free(td);
    }
    return NULL;
}

void rpmtdReset(rpmtd td)
{
    assert(td != NULL);

    memset(td, 0, sizeof(*td));
    td->ix = -1;
}

void rpmtdFreeData(rpmtd td)
{
    assert(td != NULL);

    if (td->freeData) {
	free(td->data);
    }
    rpmtdReset(td);
}

rpm_count_t rpmtdCount(rpmtd td)
{
    assert(td != NULL);
    return td->count;
}

rpmTag rpmtdTag(rpmtd td)
{
    assert(td != NULL);
    return td->tag;
}

rpmTagType rpmtdType(rpmtd td)
{
    assert(td != NULL);
    return td->type;
}

int rpmtdInit(rpmtd td)
{
    assert(td != NULL);

    /* XXX check that this is an array type? */
    td->ix = -1;
    return 0;
}

int rpmtdNext(rpmtd td)
{
    int i = -1;

    assert(td != NULL);
    if (++td->ix >= 0) {
	if (td->ix < td->count) {
	    i = td->ix;
	} else {
	    td->ix = i;
	}
    }
    return i;
}

const char * rpmtdGetString(rpmtd td)
{
    const char *str = NULL;

    assert(td != NULL);

    if (td->type == RPM_STRING_TYPE) {
	str = (const char *) td->data;
    } else if (td->type == RPM_STRING_ARRAY_TYPE) {
	/* XXX TODO: check for array bounds */
	int ix = (td->ix >= 0 ? td->ix : 0);
	str = *((const char**) td->data + ix);
    } 
    return str;
}

int rpmtdFromArgv(rpmtd td, rpmTag tag, ARGV_t argv)
{
    int count = argvCount(argv);
    rpmTagType type = rpmTagGetType(tag) & RPM_MASK_TYPE;

    if (type != RPM_STRING_ARRAY_TYPE || count < 1)
	return 0;

    assert(td != NULL);
    rpmtdReset(td);
    td->type = type;
    td->tag = tag;
    td->count = count;
    td->data = argv;
    return 1;
}

int rpmtdFromArgi(rpmtd td, rpmTag tag, ARGI_t argi)
{
    int count = argiCount(argi);
    rpmTagType type = rpmTagGetType(tag) & RPM_MASK_TYPE;
    rpmTagReturnType retype = rpmTagGetType(tag) & RPM_MASK_RETURN_TYPE;

    if (type != RPM_INT32_TYPE || retype != RPM_ARRAY_RETURN_TYPE || count < 1)
	return 0;

    assert(td != NULL);
    rpmtdReset(td);
    td->type = type;
    td->tag = tag;
    td->count = count;
    td->data = argiData(argi);
    return 1;
}
