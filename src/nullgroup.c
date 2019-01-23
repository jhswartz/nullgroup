#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kallsyms.h>
#include <linux/livepatch.h>
#include <linux/cred.h>

static int patch_in_group_p(kgid_t grp);

static int (*groups_search_function)(const struct group_info *, kgid_t) = NULL;
static const kgid_t NULLGROUP_KGID = { .val = 65533 };

static struct klp_func patch_functions[] =
{
	{ .old_name = "in_group_p", .new_func = patch_in_group_p },
	{ NULL }	
};

static struct klp_object patch_objects[] =
{
	{ .funcs = patch_functions },
	{ NULL },
};

static struct klp_patch patch =
{
	.mod = THIS_MODULE,
	.objs = patch_objects
};

static int patch_in_group_p(kgid_t grp)
{
	const struct cred *cred = current_cred();
	int retval = 1;

	if (gid_eq(grp, NULLGROUP_KGID))
	{
		retval = 0;
	}

	else if (!gid_eq(grp, cred->fsgid))
	{
		retval = groups_search_function(cred->group_info, grp);
	}

	return retval;
}

static int __init nullgroup_init(void)
{
	groups_search_function = (void *)kallsyms_lookup_name("groups_search");

	if (!groups_search_function)
	{
		return -1;
	}

	if (klp_register_patch(&patch))
	{
		return -1;
	}

	if (klp_enable_patch(&patch))
	{
		klp_unregister_patch(&patch);
		return -1;
	}

	return 0;
}

static void __exit nullgroup_exit(void)
{
	klp_disable_patch(&patch);
	klp_unregister_patch(&patch);
}

MODULE_VERSION("0.0.1");
MODULE_AUTHOR("Justin Swartz <justin.swartz@risingedge.co.za>");
MODULE_LICENSE("GPL");
MODULE_INFO(livepatch, "Y");

module_init(nullgroup_init);
module_exit(nullgroup_exit);
